# 互联网络

!!! quote

    - [ECE/CS 757 Spring 2017 | Advanced Computer Architecture II](https://ece757.ece.wisc.edu/)

互联（Interconnection）网络可以分为几类：

- **片上互联（On-chip Networks，OCN/NoC）**：微架构中各组件（如缓存、处理器等）的互联网络。例如 ARM 的 [Coherent Mesh Network](https://www.arm.com/products/silicon-ip-system/corelink-interconnect/cmn-600)。
- **节点内互联（Intra-node）**：节点内各组件的互联网络，例如 PCIe Switch。
- **节点间互联（Inter-node）**：节点间的互联网络，例如以太网。

## 片上互联

!!! quote

    - [The network-on-chip interconnect is the SoC - EDN](https://www.edn.com/the-network-on-chip-interconnect-is-the-soc/)
    - [Why network-on-chip has displaced crossbar switches at scale - EDN](https://www.edn.com/why-network-on-chip-has-displaced-crossbar-switches-at-scale/)
    - [NOC: Networks on Chip SoC Interconnection Structure](https://www.ecb.torontomu.ca/~courses/coe838/lectures/NoC_SoC-Interconnection_Structures-1.pdf)

- Ad hoc wiring
- Buses and crossbars
- Network-on-chip (NoC)

## 拓扑结构

!!! quote

    - [ECE 1749H: Interconnection Networks for Parallel Computer Architectures: Topology](https://ece757.ece.wisc.edu/lect09-interconnects-2-topology.pdf)

指标：

- **度（degree）**：每个节点的连接数，用于评估端口和链路数开销
- **跃点数（hop count）**：用于评估延迟
- **网络直径（diameter）**：网络中最大的最小跃点数
- **延迟（latency）**：包从入口进入网络，到从出口离开网络的时间
- **最大通道负载（Maximum Channel Load）**：
- **饱和（saturation）**：网络无法接收更多流量

常见拓扑结构：

- **环（ring）**：
- **网格（mesh）**：
- **环面（torus）**：

### CLOS

!!! quote

    - [Clos Network - HackMD](https://hackmd.io/@s1042992/clos)
    - [A study of non-blocking switching networks | Nokia Bell Labs Journals & Magazine | IEEE Xplore](https://ieeexplore.ieee.org/document/6770468)：CLOS 网络的原始论文，1953
    - [A scalable, commodity data center network architecture | ACM SIGCOMM Computer Communication Review](https://dl.acm.org/doi/10.1145/1402946.1402967)：将 CLOS 应用于数据中心网络，2008

- A study of non-blocking switching networks
    - 提出 CLOS 网络：

A scalable, commodity data center network architecture 将 CLOS 网络应用于数据中心。

- Fat-Tree 是 CLOS 的实例，