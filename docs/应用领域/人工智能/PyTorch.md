# PyTorch

版本：2025.07.05 v2.7.1 稳定版

## 编译构建

## torch.Tensor

tensor.backward()如何调用autograd引擎

## nn.Module

forward() → autograd.Function → C++算子

## JIT

## 并行与分布式

### torch.multiprocessing


### torch.distributed

该模块用于多机分布式训练。它将相互协作的进程组成 ProcessGroup，提供集合通信和点对点通信的能力。未指定时，集合通信发生在默认的 PG，称为 world。

使用方式：

```python
.init_process_group(
    backend='nccl',
    world_size=...,
    rank=...
)
.all_gather(list, obj, group)
```

#### 集合通信

C++ 源码位于 `torch/csrc/distributed/c10d/ProcessGroup*.cpp`。其中：

- `ProcessGroup.cpp` 定义所有 `ProcessGroup` 的通用接口（如 `broadcast()` 等），是一个抽象基类。
- `ProcessGroupNCCL.cpp` 实现由 NCCL 支持的分布式通信。

以 NCCL 实现为例，先看用法：

```cpp
ProcessGroupNCCL pg(store, rank, size);
std::shared_ptr<WorkNCCL> work = pg.allreduce(tensors);
```

所有集合通信最终都会以下面的方式调用一个通用的 `collective()` 接口，举例：

```cpp
c10::intrusive_ptr<Work> ProcessGroupNCCL::allgather(
    std::vector<std::vector<at::Tensor>>& outputTensors,
    std::vector<at::Tensor>& inputTensors,
    const AllgatherOptions& opts) {

    return collective(
        inputTensor,
        outputFlattened,
        [&](at::Tensor& input,
            at::Tensor& output,
            ncclComm_t comm,
            at::cuda::CUDAStream& stream) {
            if (!avoidRecordStreams_) {
                c10::cuda::CUDACachingAllocator::recordStream(
                    output.storage().data_ptr(), stream);
            }
            return ncclAllGather(
                input.data_ptr(),
                output.data_ptr(),
                input.numel(),
                getNcclDataType(input.scalar_type()),
                comm,
                stream.stream());
        },
        // ...
```

`collective()` 用于包装集合通信，负责创建 `WorkNCCL` 并将其放入队列、执行具体的 NCCL 调用：

```cpp
template <typename Fn, typename PreProcess, typename PostProcess>
c10::intrusive_ptr<Work> ProcessGroupNCCL::collective(
    std::vector<at::Tensor>& inputs,
    std::vector<at::Tensor>& outputs,
    Fn fn,
    PreProcess pre,
    PostProcess post,
    OpType opType,
    const char* profilingTitle,
    bool avoidRecordStreams,
    bool nanCheck) {

    auto work = initWork(
        device, rank_, opType, false, profilingTitle, inputs, outputs, enqueue);

	C10D_NCCL_CHECK(
        fn(inputs[0], outputs[0], comm, ncclStream),
        ncclComm->getNcclCommFailureReason());
      
	workEnqueue(work);
```

#### 超时检测

`ProcessGroupNCCL` 中有：

- 一个工作队列 `list<WorkNCCL> workMetaLis_`，以及保护它的锁和条件变量
- 一个原子变量 `heartbeat_`，这是心跳计数

`ProcessGroupNCCL` 的构造函数在独立的线程中启动 `ncclCommaWatchdog()` 进行监控。该函数又分为两个线程：

- `watchdogHandler()`：
    - 轮询工作队列，调用其中每个元素的 `.checkTimeout()` 检查是否超时。
    - 完成检测后，递增心跳变量
- `heartbeatMonitor()`：周期性检查心跳变量

各组件的相互作用可梳理如下图：

![pytorch.assets/ProcessGroupNCCL.drawio](pytorch.assets/ProcessGroupNCCL.drawio)

#### PG 初始化和 NCCL 抽象

底层的 NCCL 使用 communicator 组织通信，这并不是在 PG 的构造函数中创建的，而是等待第一次通信时创建。这一逻辑实现在 `collective()` 通用接口中：

```cpp
if (ncclComm == nullptr) {
    ncclComm = initNCCLComm(key, device, opType);
}
  
std::shared_ptr<NCCLComm> ProcessGroupNCCL::initNCCLComm(
    const std::string& deviceKey,
    at::Device& device,
    OpType opType,
    int p2pRank,
    bool isSendRecvSelf);
```

可以看到 PyTorch 又将 NCCL 抽象出一个类 `NCCLComm`，封装在 `torch/csrc/cuda/nccl` 和 `torch/csrc/distributed/c10d/NCCLUtils.cpp` 中，限制了 PyTorch 能使用的 nccl API 范围，也增强了 OOP 的能力。

`initNCCLComm()` 中有非常多种创建 communicator 的方式，比如 split 之类的，我们暂不深究，仅看最简单直接构造函数。

```cpp
std::shared_ptr<NCCLComm> NCCLComm::create(
    int numRanks,
    int rank,
    ncclUniqueId commId,
    at::DeviceIndex deviceIndex) {
    at::cuda::OptionalCUDAGuard gpuGuard(deviceIndex);
    auto comm = std::make_shared<NCCLComm>();
    C10D_NCCL_CHECK(
        ncclCommInitRank(&(comm->ncclComm_), numRanks, commId, rank),
        std::nullopt);
    comm->ncclId_ = commId;
    comm->rank_ = rank;
    comm->deviceIndex_ = deviceIndex;
    comm->initialized_ = true;
    // Old style comm is always blocking.
    comm->nonBlocking_ = false;
    return comm;
}
```

调用转交给了 `ncclCommInitRank()`。

