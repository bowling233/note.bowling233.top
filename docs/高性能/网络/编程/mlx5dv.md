# `mlx5dv` 与 DevX

!!! quote

    - [Mellanox/devx](https://github.com/Mellanox/devx)
    - [mlx5dv(7) — libibverbs-dev — Debian testing — Debian Manpages](https://manpages.debian.org/testing/libibverbs-dev/mlx5dv.7.en.html)
    - [rdma-core/providers/mlx5/man at master · linux-rdma/rdma-core](https://github.com/linux-rdma/rdma-core/tree/master/providers/mlx5/man)

- 头文件：`infiniband/mlx5dv.h`
- API 前缀：`mlx5dv_`

`mlx5dv`（Direct Verbs）API 支持从用户空间直接访问 mlx5 驱动设备的资源，绕过 `libibverbs` 的数据通路，直接暴露低级别的数据通路。

!!! tip "兼容性"

    从 Connect-IB 开始的所有 Mellanox 网卡设备（包括 Connect-IB、ConnectX-4、ConnectX-4Lx、ConnectX-5 等）都实现了 mlx5 API。

DevX API 疑似是 Mellanox 开发的 `mlx5dv` 的前身，其仓库上次更新时间为 2018 年，当年 `mlx5dv` 也并入了 `rdma-core`。现在 DevX 成为了 `mlx5dv` 的一部分。

这部分的文档少得可怜。目前仅有的文档来自 `rdma-core/providers/mlx5/man` 的 mlx5 Programmer's Manual。我们先对 DevX API 的各个部分建立大概的印象：

- DevX（见 `mlx5dv_devx_obj_create`）：目标是使用户空间驱动程序尽可能独立于内核。当设备提供新的功能和命令时，不需要内核更改就能使用。
    - DEVX 对象代表底层的固件对象。
    - 创建 DEVX 对象后，可以通过 `mlx5dv_devx` API 进行查询、修改或销毁。

    !!! tip "这一部分的数据结构都是固件相关的二进制数据，编程时采用大量宏定义和位操作。需要仔细查看具体的设备文档。"

- Direct Rule（见 `mlx5dv_dr_flow`）：让应用程序直接控制设备的所有包转发（packet steering）功能，定义复杂的流表规则，包括计数、封装、解封装等。

## DMA 示例

使用 `mlx5dv` API 执行 DMA 拷贝：

- Host：
    - 用 IB Verb 创建设备上下文、PD
    - 使用 `mlx5dv_devx_umem_reg()` 注册用户 DMA 内存，获得一个 `struct mlx5dv_devx_umem`。
    - 使用 `MLX5_CMD_OP_CREATE_MKEY` 调用 `mlx5dv_devx_obj_create()` 获得 MKEY。
    - 使用 `mlx5dv_devx_general_cmd()` 执行 `MLX5_CMD_OP_ALLOW_OTHER_VHCA_ACCESS`，允许其他 VHCA 访问内存。
- Device：
    - 用 IB Verb 创建设备上下文、PD 和 MR。
    - 从 Host 获取相关数据
    - 使用 `MLX5_CMD_OP_CREATE_GENERAL_OBJECT` 和 `MLX5_GENERAL_OBJ_TYPE_MKEY` 调用`mlx5dv_devx_obj_create()` 创建 MR 别名。
