# rdma-core

!!! quote

    - [Mellanox Adapters Programmer’s Reference Manual](https://network.nvidia.com/files/doc-2020/ethernet-adapters-programming-manual.pdf)：该手册适用于 CX4，详细地描述了 IB Verbs 编程的所有细节，细致到每个字段的定义。
    - [RDMA Aware Networks Programming User Manual - NVIDIA Docs](https://docs.nvidia.com/networking/display/rdmaawareprogrammingv17)：包含 RDMA 架构概述和 IB Verbs、RDMACM 的 API 文档。**该文档第八章包含了各层次 API 编程的例子，具有比较详细的注释，适合初学者学习。**本文中的部分编程示例来自该手册。

## libibverbs

### SRQ

SRQ 作为 `struct ibv_qp_init_attr` 中的一个**可选**字段。

## librdmacm

- 头文件：`rdma/rdma_cma.h`、`rdma_verbs.h`
- 软件包名：`librdmacm-dev`

librdmacm 提供以 `rdma_` 为前缀的另一套 API：

- Connection Manager，负责管理 RDMA 连接。它包装了交换 QP、Key 等信息的过程，减少了代码量，让 RDMA 编程更加简单。它的接口与 Socket 比较类似：

    ```c
    rdma_listen()
    rdma_connect()
    rdma_accept()
    ```

- RDMA Verbs，本质上是建立在 libibverbs 上的一层包装。

以 SRQ 的创建为例，我们看看具体的包装过程：

```c title="librdmacm/cma.c"
int rdma_create_srq(struct rdma_cm_id *id, struct ibv_pd *pd,
    struct ibv_srq_init_attr *attr) {
    ret = rdma_create_srq_ex(id, &attr_ex);
}
int rdma_create_srq_ex(struct rdma_cm_id *id, struct ibv_srq_init_attr_ex *attr) {
    struct ibv_srq *srq;
    srq = ibv_create_srq_ex(id->verbs, attr);
}
```

### 地址绑定

假设你调用 `rdma_bind_addr()` 时遇到了 ENODEV（No such device (19)）的问题，接下来一步步分析问题出现的位置。

`rdma_bind_addr()` 根据 `af_ib_support` 分支为两个路径，出问题的路径在 IB 上：

```text
rdma_bind_addr() → rdma_bind_addr2() → ucma_query_addr() → ucma_get_device()
```

```c
static int ucma_get_device(struct cma_id_private *id_priv, __be64 guid,
               uint32_t idx)
{
    struct cma_device *cma_dev;
    int ret;

    pthread_mutex_lock(&mut);
    cma_dev = ucma_get_cma_device(guid, idx);
    if (!cma_dev) {
        pthread_mutex_unlock(&mut);
        return ERR(ENODEV);  // 这里返回 ENODEV
    }
    // ...
}
```

其中 `ucma_get_cma_device()` 返回空有两种情况：

```c
static struct cma_device *ucma_get_cma_device(__be64 guid, uint32_t idx)
{
    struct cma_device *cma_dev;

    // 第一次查找：在现有设备列表中查找
    list_for_each(&cma_dev_list, cma_dev, entry)
        if (!cma_dev->is_device_dead && match(cma_dev, guid, idx))
            goto match;

    // 如果第一次查找失败，同步设备列表
    if (sync_devices_list())
        return NULL;  // 同步失败，返回 NULL

    // 第二次查找：在更新后的设备列表中查找
    list_for_each(&cma_dev_list, cma_dev, entry)
        if (!cma_dev->is_device_dead && match(cma_dev, guid, idx))
            goto match;
    
    cma_dev = NULL;  // 两次查找都失败，设置为 NULL

match:
    if (cma_dev)
        cma_dev->refcnt++;
    return cma_dev;
}
```

- `sync_devices_list()` 函数可能失败的原因：

    ```c
    static int sync_devices_list(void)
    {
        struct ibv_device **new_list;
        int i, j, numb_dev;

        new_list = ibv_get_device_list(&numb_dev);
        if (!new_list)
            return ERR(ENODEV);  // 获取设备列表失败

        if (!numb_dev) {
            ibv_free_device_list(new_list);
            return ERR(ENODEV);  // 系统中没有 RDMA 设备
        }
        // ...
    }
    ```

- `match()` 函数找不到匹配的设备：

    ```c
    static bool match(struct cma_device *cma_dev, __be64 guid, uint32_t idx)
    {
        if ((idx == UCMA_INVALID_IB_INDEX) ||
            (cma_dev->ibv_idx == UCMA_INVALID_IB_INDEX))
            return cma_dev->guid == guid;

        return cma_dev->ibv_idx == idx && cma_dev->guid == guid;
    }
    ```

    匹配失败的原因：

    - 请求的 guid 与系统中任何设备的 GUID 都不匹配
    - 请求的 idx 与系统中任何设备的索引都不匹配
    - GUID 和索引都不匹配

若要了解具体是哪个地方出错，可以：

- 重新编译 rdma-core 启用调试信息，使用 GDB 调试
- 模仿上面的代码写个例程，把信息打出来看看

暂时停止在这里，以后有空再探究。
