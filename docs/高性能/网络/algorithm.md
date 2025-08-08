# 通信算法

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



### 通信算法


### 流水线

### 缓冲区



