# UCX

## 基础知识

### 整体结构

UCX 文档首页有这样的一幅图：

![ucx/arch](https://openucx.readthedocs.io/en/master/_images/UCX_Layers.png)

然而对应到具体，特别是阅读源码时，还将遇到几个抽象层次。这里挑出一些来讲讲：

```text
src
|-- tools
|   |-- perf
|   |-- profile
|   `-- vfs：实现了类似 kobject 和 sysfs 对应的 vfs 接口，用于调试和管理
|-- ucg：集合通信
|   |-- api
|   |-- base
|-- ucm：负责内存管理
|   |-- api
|   |-- bistro
|   |-- cuda
|   |-- event
|   |-- malloc
|   |-- mmap
|   |-- ptmalloc286
|   |-- rocm
|   |-- util
|   `-- ze
|-- ucp：处理上层协议抽象，如消息匹配、RPC、建链等
|   |-- am
|   |-- api
|   |-- core
|   |-- dt
|   |-- proto
|   |-- rma
|   |-- rndv
|   |-- stream
|   |-- tag
|   `-- wireup
|-- ucs：UCX 内部提供的服务（service），包括数据结构、算法等
|   |-- algorithm
|   |-- arch
|   |-- async
|   |-- config
|   |-- datastruct
|   |-- debug
|   |-- memory
|   |-- profile
|   |-- signal
|   |-- stats
|   |-- sys
|   |-- time
|   |-- type
|   `-- vfs
`-- uct：传输层，调用底层的原语，如 IB Verbs
    |-- api
    |-- base
    |-- cuda
    |-- ib
    |-- rocm
    |-- sm
    |-- tcp
    |-- ugni
    `-- ze
```

我们具体看一下 UCT 有哪些可选组件：

- CUDA
    - CUDA Copy
    - CUDA IPC
    - GDR Copy
- InfiniBand
    - RC
        - Verbs
    - MLX5
        - DC
        - DV
        - GGA
        - RC
        - UD
- TCP

## 源码阅读

### 调试

环境变量 `UCX_LOG_LEVEL=trace`，见 [Frequently Asked Questions — OpenUCX documentation](https://openucx.readthedocs.io/en/master/faq.html?highlight=ucx_log_level)。

在 OpenMPI 中，还可以使用 `--mca pml_ucx_verbose 100` 检查 OpenMPI 如何调用 UCX。

### 类型系统

UCX 使用 C 编写，而在其数据结构中又广泛使用 OOP 和泛型编程，这就意味着宏的大量使用。让我们先接触相关的宏，它们定义在 [`src/ucs/type/class.h`](https://github.com/openucx/ucx/blob/master/src/ucs/type/class.h) 中，请你借助 AI 自行了解其工作原理。

```c
struct ucs_class {
    const char               *name;
    size_t                   size;
    ucs_class_t              *superclass;
    ucs_class_init_func_t    init;
    ucs_class_cleanup_func_t cleanup;
};
// OOP 系统
UCX_CLASS_DEFINE(_type)
UCX_CLASS_DECLARE(_type, _super) // 继承关系
UCX_CLASS_CALL_SUPER_INIT
// 对象通用接口 
UCX_CLASS_INIT
UCX_CLASS_NEW
UCX_CLASS_DELETE
```

以 `UCX_CLASS_DECLARE` 为例，展开如下：

```c
extern ucs_class_t parent_class_class;
ucs_class_t my_class_class = {
    "my_class",
    sizeof(my_class),
    &parent_class_class,
    (ucs_class_init_func_t)(my_class_init),
    (ucs_class_cleanup_func_t)(my_class_cleanup)
};
```

可以看到，这些宏一起实现了类似 C++ 的 OOP 类型系统，实现了通用构造函数、析构函数。类型之间的继承关系将 `ucx_class_t` 实例组织为树状结构，从而实现了继承和多态等特性。

!!! example

    以 RC Verbs 类型的网络接口为例，我们来具体看看该类型系统的使用。

    UCX IB 模块的继承关系如下：

    ```text
    uct_base_iface_t (UCT 基础接口)
        ↓
    uct_ib_iface_t (IB 基础接口 - src/uct/ib/base/)
        ↓
    uct_rc_iface_t (RC 基础接口 - src/uct/ib/rc/base/)
        ↓
    uct_rc_verbs_iface_t (RC Verbs 具体实现 - src/uct/ib/rc/verbs/)
    ```

    在每个类型的文件中，你都可以看见 `UCS_CLASS_INIT_FUNC` 的具体定义，它们一级一级地向上调用父类的构造函数。

    这些网络接口本身使用了类型系统，但相关操作没有借助通用宏，而是显式地做定义。例如：

    ```c
    typedef struct uct_rc_iface_ops {
        uct_ib_iface_ops_t                  super;
        uct_rc_iface_init_rx_func_t         init_rx;
        uct_rc_iface_cleanup_rx_func_t      cleanup_rx;
        // ...
    } uct_rc_iface_ops_t;
    static uct_rc_iface_ops_t uct_rc_verbs_iface_ops = {
        .super = {
            .super = {
                .iface_estimate_perf   = uct_rc_iface_estimate_perf,
                .iface_vfs_refresh     = uct_rc_iface_vfs_refresh,
                // ...
            },
            .create_cq      = uct_ib_verbs_create_cq,
            .destroy_cq     = uct_ib_verbs_destroy_cq,
            // ...
        },
        .init_rx         = uct_rc_iface_verbs_init_rx,
        .cleanup_rx      = uct_rc_iface_verbs_cleanup_rx,
        // ...
    };
    ```

    可以将上层的类看作抽象基类，它们定义为类型（`ops_t`），没有实现具体功能；子类定义为类型的实例（`ops`），对所有字段进行填充。

    这样的函数指针表实现了多态。

### 网络接口

本节让我们以 RC Verbs 接口为例，通过几个问题引导，探究 UCX 是如何初始化网络接口的。

#### `iface_t` 每一层做了哪些工作？

从最顶层的 `uct_iface_t` 开始：

1. **`uct_iface_t`**  

   - 基础网络接口初始化，验证并设置接口的操作函数表（`ops`）。
   - 确保所有必要的操作函数（如`ep_flush`、`ep_fence`等）均已定义。
   - 主要功能是为接口提供通用的操作函数框架。

2. **`uct_base_iface_t`**  

   - 继承自`uct_iface_t`，扩展了基础接口的功能。
   - 初始化内存分配方法、错误处理机制、工作线程等。
   - 设置接口的统计信息和性能计数器。
   - 主要功能是为接口提供更高级的通用功能，如内存管理和错误处理。

3. **`uct_ib_iface_t`**  

   - 继承自`uct_base_iface_t`，专为 InfiniBand 设备设计。
   - 初始化 IB 设备的端口、队列对（QP）、完成队列（CQ）等硬件资源。
   - 配置路径 MTU、流量控制（FC）参数、全局地址等。
   - 主要功能是为 IB 设备提供底层硬件资源的初始化和配置。

4. **`uct_rc_iface_t`**  

   - 继承自`uct_ib_iface_t`，专为可靠连接（RC）模式设计。
   - 初始化 RC 模式的发送和接收队列、原子操作处理函数、流控（FC）机制等。
   - 配置 QP 的重试次数、RNR 超时等参数。
   - 主要功能是为 RC 模式提供特定的可靠性和流控支持。

5. **`uct_rc_verbs_iface_t`**  

   - 继承自`uct_rc_iface_t`，专为 Verbs API 实现。
   - 初始化 Verbs 特有的资源，如短描述符内存池、内联工作请求（WR）等。
   - 配置最大内联数据大小、发送 SGE 数量等硬件限制。
   - 主要功能是为 Verbs API 提供优化的资源管理和性能调优。

#### SRQ 是在哪一层配置的？

看一下数据结构：

```c
struct uct_ib_iface {
    uct_base_iface_t          super;

    struct ibv_cq             *cq[UCT_IB_DIR_LAST];
    struct ibv_comp_channel   *comp_channel;
    // ...
}
struct uct_rc_iface {
    uct_ib_iface_t              super;

    struct {
        /* Credits for completions.
         * May be negative in case mlx5 because we take "num_bb" credits per
         * post to be able to calculate credits of outstanding ops on failure.
         * In case of verbs TL we use QWE number, so 1 post always takes 1
         * credit */
        signed                  cq_available;
        ssize_t                 reads_available;
        ssize_t                 reads_completed;
        // ...
    } tx;

    struct {
        ucs_mpool_t          mp;
        uct_rc_srq_t         srq;
    } rx;
    // ...
}
```

可以看到从 `rc_iface` 这一层开始有 SRQ，容易找到其初始化函数：`uct_rc_iface_init_rx()`，该函数直接调用 `ibv_create_srq()` 进行创建。
