# NVIDIA

!!! quote

    - [NVIDIA Data Center GPU Driver Documentation](https://docs.nvidia.com/datacenter/tesla/)

## Driver

## 单机：NVLink

NVLink 提供 GPU 之间的连接，NVSwitch 是用于多 GPU 全互联通信的交换芯片。到 2025 年，有四代 NVSwitch：

| NVSwitch 代数 | GPU |
| - | - |
| 一 | V100 |
| 二 | A100 |
| 三 | H100 |
| 四 | B200、B100 |

代数不同，GPU 和互联芯片的拓扑有变化。

在软件侧：

- 早于第四代的系统：由**内核驱动**和 **Fabric Manager** 组成
    - 内核驱动根据 FM 的请求执行底层硬件管理
    - FM 配置 NVSwitch 内存结构，使所有参与的 GPU 形成一个统一的内存结构，并监控支持该结构的 NVLink
    - FM 也负责 GPU、NVSwitch 等的路由、端口、驱动程序初始化
- 第四代系统：
    - NVIDIA 实现了跨 NVLink、InfiniBand 和以太网交换机的统一架构，第四代 NVSwitch 与 InfiniBand 交换机共享通用 IP 模块，主要集中在链路层和控制面
    - 引入 **NVLink Subnet Manager（NVLSM）** 与 FM 协同工作，它源自 Infiniband Subnet Manager
    - NVLSM 负责配置 NVSwitch 路由表，而 FM 负责 GPU 端路由、NVLink 配置，并提供分区管理的 API，FM 与 NVLSM 之间通过 IPC 交互
    - NVSwitch 不再作为 PCIe Bridge 设备被主机识别，而是通过 CX7 Bridge 连接到主机，**显示为 Infiniband Controller 设备**，提供一些 PF 用于管理

## 多机

!!! quote

    - [NVIDIA Linux Public Repository](https://linux.mellanox.com/public/repo/)


### MLNX OFED

!!! quote

    - [NVIDIA MLNX_OFED Documentation](https://docs.nvidia.com/networking/software/adapter-software/index.html)

### DOCA

!!! quote

    - [](https://docs.nvidia.com/doca/sdk/)
