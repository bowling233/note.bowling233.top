# 学术论文

主要关注五个会议中的相关论文：

- RTAS
- RTSS
- ASPLOS
- OSDI
- SOSP

## [RTAS 2025](https://ieeexplore.ieee.org/xpl/conhome/11018667/proceeding)

### [Arm DynamIQ Shared Unit and Real-Time: An Empirical Evaluation](https://arxiv.org/abs/2503.17038)

#### Introduction

在 RK3568、RK3588 和 NVIDIA Orin 三个芯片上，首次对 Arm DSU 在实时任务负载下的能力进行了评估。

DynamIQ Shared Unit：Arm 大小核（big.LITTLE）架构下的一种技术，提供 per-cluster 的 L3 Cache，还实现了多个 DynamIQ cluster 的互联。在大部分 Arm v8/v9 芯片中可以找到它。

对于实时系统来说很重要的一点：它可以把 L3 分成 four way-based partition。

这篇文章要考虑的问题：

- 基于硬件的 way partition 还是基于软件的 cache coloring 更好？
- 在 average case 外，实时系统还需要重点考虑 corner case，如何找到 DSU 的 corner case？

结果：比较复杂，DSU 的分区表现与相互干扰的工作负载及其强度有关。并且平台相关的优化，如 prefetch 和 write-streaming 对结果有重要影响。

贡献点：前述 + **充分利用 performance counter 来观测性能**（需要学习的点）

似乎 partition 技术可以分为 way 和 set 两类。

#### Related work

多核共享资源间的串扰对实时系统有严重影响

最坏执行时间（Worst-case execution time，WCET）

提升访问共享内存的可预测性，提出了多样的 cache 管理技术（这里引用了一堆文献）：

- cache partitioning：分成小块指定给任务或核心
    - 组相连（set-associative）架构下，可以分为按 way 或 set 划分。
        - way 划分必须改硬件
        - set 划分软硬件都能实现，知名的就是 page coloring

- cache locking：pin 住特定 cacheline，需要硬件支持，现成的 CPU 一般没有

locking 和 partitioning 还能结合，获得更高级的可预测性Real-time cache management framework for multi-core archi-
tectures（RTAS 2013）

DRAM 并不适合实时任务，内存控制器要重新访存请求提供上界。也有软件方案，通过 OS 划分内存 bank 减少核间串扰、基于 performance counter 的带宽约束等等。

#### background

系统参数：

- 64B 缓存行 -> [5:0] 表示 cacheline 内偏移
- L2 cache 1024 个 set -> [15:6] 表示 set index
- 高位为 tag

重点：**set index + offset 大于 pgsize 时，可以使用 page coloring**。这里 4KB 页，则颜色有 4bit = 16 种。

- L3 cache 16 个 way

**way partitioning**：

- 一组 4 个 way 可以指定给一个或多个 DSU scheme ID，每个 core 也可以分配到 scheme ID
- 效果：限定 L1/L2 逐出的数据可以分配到的 way，使 app 总是可以在 cache 中拿到所有 cache line

体系结构中访存的优化：

- 硬件 prefetch：预测访存模式，提前载入数据。降低延迟但引入更多内存操作。
- write-streaming 用于优化 large data transfer，write 可以 bypass L3
    - latency inconsistency under mem bw contention

These two technology is **unavoidable and activated automatically**.

A53:

- 

DSU:

- 16-way
- one or two slice
- can configure which address bit to use to select slice

#### setup

Focus:

- **isolation and real-time properties**.
- two implementations of cache-partitioning:
    - set partitioning: Jailhouse cache coloring
    - way partitioning: driver controlling DSU cache partitioning

Config:

- ZCU102: AMD
    - Cortex-A53:
        - L1: 32KB private
        - L2: 1MB shared
    - no DSU, use as **baseline**
- RK3568:
    - 4 x Cortext-A55:
        - L1: 32KB private
        - L2: missing
        - L3: 512KB shared (DSU)
- RK3588:
    - 4 x Cortex-A55:
        - L1: 32KB private
        - L2: 128KB private
    - 4 x Cortex-A76:
        - L1: 64KB private
        - L2: 512KB private
    - L3: 3MB shared (DSU)
- Orin:
    - 3 x 4 x Cortex-A78AE:
        - L1: 64KB private
        - L2: 256KB private
        - L3: 2MB per DSU
    - L4: 16-way set-associative 4MB

Evaluation:

- SD-VBS running alongside a **memory-intensive synthetic interference application on seperate cores**
- varing two factors:
    - cache allocation between the two
    - work-set size of interference task

1. no cache partition
2. partition using way and set, ratio: 1:3, 2:2, 3:1
3. working-set size: 8KB -> 8MB

To setup way partition:

- assign ID to `CLUSTERTHREADSID`
- assign way group in `CLUSTERPARTCR`
- by default access to PMU and cofiguration registers are disabled. Modify Arm Trusted Firmware(ATF) to enable: `SMEN`, `TSIDEN`, `CLUSTERPMUEN` bit in `ACTLR_EL2/3`

Performance Counters recording events:

Arm Core:

```text
# refills sourced from within the cluster (like the L3 cache or from another core) versus those from external sources
# assessing the locality of memory access and the pressure placed on the L1 cache by external memory transaction
L1D_CACHE_REFILL_INNER
L1D_CACHE_REFILL_OUTER
#  cycles in which the core operates in write-streaming mode, avoiding L3 allocations
# impact of sustained write operations on cache utilization and memory coherence
L3D_WS_MODE
```

DSU:

```text
# L3 prefetch transactions initiated by the CPU
SCU_PFTCH_CPU_ACCESS
SCU_PFTCH_CPU_HIT
SCU_PFTCH_CPU_MISS
# all read and write transactions directed to the DSU
# aggregated view of L3 activity
L3D_ACCESS
# DSU and the interconnect
# volume of external memory traffic
BUS_ACCESS
# allocations that do not involve refill
L3D_CACHE_ALLOCATE
```

RT-Bench framework

dataset: cif, vga

Interference-bench: generate read, write, modify and prefetch

- read: load full cacheline to L1
- write: store whole cacheline to memory
- modify: change single byte of cacheline. need to load and modify.
- prefetch: prefetch to L3


set partitioning using page coloring:

- set index can be computed by masking specific **coloring bits** of a given physical address

做了一个关于 bit assignment 的小实验，结论是：DSU 没有在 set index 计算过程中对 color bits 做 bit-scrambling

- bit-scrambling：用于对抗侧信道攻击的技术，可能对 addr 的一些位做 scrambling 使得 cache 的位置分布无法预测
- 使用 2MB 大页，这样拿到的一段物理内存是连续的且大于缓存大小，能够用于测试 set-index
- 连续载入 n 个**相同颜色的** 4KB 页面到 L3
- 再次 prefetch n 个**相同颜色的** 4KB 页面，观察 prefetch hit 和 miss 数量
- 关闭了硬件预取（`CPUECTRL.L3PCTL`），只使用软件预取

实验结果：访存超过 Color 大小时，prefetch miss 率增长，说明发生 cache thrashing

#### experiments and discussion

1. Way partition can increase conflict misses：显而易见，因为 associativity 变少了。




---
疑问：

- 什么是 write-streaming？
- cache locking 部分困难的原因没看懂
