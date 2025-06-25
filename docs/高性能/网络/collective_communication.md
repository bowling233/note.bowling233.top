# 集合通信

MPI 实现一般都会为集合通信提供不同的算法，以适应不同的通信模式。这些算法在特定场景下可能会有不同的性能表现，因此需要根据实际情况进行调优。

一篇经典的例子是：[GitHub: OpenMPI Bcast and Allreduce much slower than Intel-MPI (unrelated to EFA)](https://github.com/aws/aws-parallelcluster/issues/1436)。这位作者在 AWS 集群上测试了 OpenMPI 和 IntelMPI 各种算法的性能，给出了详细的测试结果。

![Bcast algorithm comparison](https://user-images.githubusercontent.com/25473287/68506534-d4827e00-0237-11ea-8d04-e6b1836d96b8.png)

可以看出性能差距还是比较明显的，因此算法调优是非常重要的。

=== "OpenMPI"

    - 列出可用的集合通信算法：

    ```bash
    ompi_info --param coll tuned --level 9
    MCA coll tuned: parameter "coll_tuned_allreduce_algorithm" (current
                              value: "ignore", data source: default, level: 5
                              tuner/detail, type: int)
                              Which allreduce algorithm is used. Can be locked
                              down to any of: 0 ignore, 1 basic linear, 2
                              nonoverlapping (tuned reduce + tuned bcast), 3
                              recursive doubling, 4 ring, 5 segmented ring. Only
                              relevant if coll_tuned_use_dynamic_rules is true.
                              Valid values: 0:"ignore", 1:"basic_linear",
                              2:"nonoverlapping", 3:"recursive_doubling",
                              4:"ring", 5:"segmented_ring", 6:"rabenseifner"
    ```

    - 通过 MCA 参数设置算法，以 recursive doubling 为例：

    ```bash
    --mca coll_tuned_use_dynamic_rules 1 \
    --mca coll_tuned_allreduce_algorithm 3 \
    --mca coll_tuned_allreduce_algorithm_segmentsize 4096 \
    --mca coll_tuned_allreduce_algorithm_tree_fanout 4
    ```

    - 设置 Bcast 算法：

    ```bash
    orterun \
    --mca coll_tuned_use_dynamic_rules 1 \
    --mca coll_tuned_bcast_algorithm $algo \
    ```

=== "Intel MPI"

## 理论知识

!!! quote

    - [scc.ustc.edu.cn/zlsc/cxyy/200910/MPICH/](https://scc.ustc.edu.cn/zlsc/cxyy/200910/MPICH/)

- 熟悉下列通信原语的语义：

    - Broadcast
    - Scatter
    - Gather
    - AllGather
    - Reduce
    - ReduceScatter
    - AllReduce
    - AllToAll

具备以上基础知识后，我们在 [Demystifying NCCL: An In-depth Analysis of GPU Communication Protocols and Algorithms](https://arxiv.org/html/2507.04786v2) 的带领下，以 NCCL 为例，系统地学习通信算法的各个方面。

集合通信算法：根据拓扑中链路的延迟和带宽，为数据选择合适的路径，规划计算的顺序，实现集合通信原语。

## 论文研读

### TACCL

TACCL 能够针对指定的硬件配置和集合通信原语，自动选择最优的通信算法。

该论文的创新点在于用户提供 communication sketech，以减少搜索空间，从而解决算法选择这个 NP 问题，能够扩展到多个节点。

- TACCL 将问题建模为混合整数线性规划（Mixed Integer Linear Programming, MILP）问题
    - 先做路由，再做排序，最后做批处理
    - 先求解 bandwidth-relaxed 版本得到 routing，再求解 bandwidth-constrained 版本得到 scheduling
- Communication Sketech 包含四部分信息：
    - 限定逻辑拓扑
    - 标记交换机
    - 提示拓扑和通信原语的对称性
    - 输入数据大小

```
$ mpirun -np <ngpus> -x LD_LIBRARY_PATH=msccl/build/lib/:$LD_LIBRARY_PATH -x NCCL_DEBUG=INFO -x NCCL_DEBUG_SUBSYS=INIT,ENV -x MSCCL_XML_FILES=<taccl-ef> -x NCCL_ALGO=MSCCL,RING,TREE  nccl-tests/build/<nccl-test-binary> -b 128 -e 32MB -f 2 -g 1 -c 1 -n 100 -w 100 -G 100 -z 0
```

### [](https://arxiv.org/html/2408.11008v1)


