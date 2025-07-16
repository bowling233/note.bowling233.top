# RDMA CM

!!! quote

    - [RDMA Aware Networks Programming User Manual - NVIDIA Docs](https://docs.nvidia.com/networking/display/rdmaawareprogrammingv17)：包含 RDMA 架构概述和 IB Verbs、RDMACM 的 API 文档。**该文档第八章包含了各层次 API 编程的例子，具有比较详细的注释，适合初学者学习。**本文中的部分编程示例来自该手册。

- 头文件：`rdma/rdma_cma.h`、`rdma_verbs.h`
- API 前缀：`rdma_`
- 软件包名：`librdmacm-dev`

librdmacm 提供比 IB Verbs 抽象层次更高的另一套 API：

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

## 例程

代码见 [`mckey.c`](./index.assets/code/mckey.c)。

RDMA_CM 用于管理 RDMA 连接，包装了使用 socket 编程交换 QP、R_Key 等信息的过程，减少代码量。它的接口与 Socket 类似：

与 Socket 编程的比较：

- 操作异步进行，通过 `rdma_event_channel` 进行事件通知。
- `rdma_cm_id`（identifier）与 `fd` 类似，用于标识连接。
- 使用 `rdma_bind_addr()` 将 `rdma_cm_id` 与 `sockaddr` 绑定，类似 `bind()`。

本例为多播通信，需要使用 `rdma_join_multicast()` 和 `rdma_leave_multicast()` 进出多播组。

```mermaid
flowchart TD
 subgraph s1["run()"]
  subgraph s2["connect_evnets() [LOOP]"]
   subgraph s5["join_handler()"]
    n14@{ shape: "rounded", label: "ibv_create_ah()" }
    n15@{ shape: "rounded", label: "inet_ntop()" }
   end
   subgraph s4["addr_handler()"]
    n12@{ shape: "rounded", label: "rdma_join_multicast()" }
    n13@{ shape: "rounded", label: "ibv_post_recv()" }
   end
   subgraph s3["cma_handler()"]
   end
   n9@{ shape: "rounded", label: "rdma_ack_cm_evnet()" }
   n11@{ shape: "rounded", label: "rdma_get_cm_event()" }
   n10@{ shape: "hex", label: "rdma_cm_event" }
  end
  n8@{ shape: "rounded", label: "rdma_bind_addr()" }
  n7@{ shape: "hex", label: "char *" }
  n6@{ shape: "rounded", label: "getaddrinfo()" }
  n5@{ shape: "hex", label: "sockaddr" }
  n2@{ shape: "rounded", label: "rdma_create_id()" }
  n1@{ shape: "hex", label: "rdma_cm_id" }
 end
 n3@{ shape: "hex", label: "rdma_event_channel" }
 n4@{ shape: "rounded", label: "rdma_create_event_channel" }
 n4@{ shape: "rounded", label: "rdma_create_event_channel()" } --- n3
 n3 --- n2
 n2 --- n1
 n6 --- n5
 n7 --- n6
 n5 ---|"src_addr"| n8
 n1 --- n8
 n11 --- n10
 n10 --- n9
 s3 --- s4
 s3 --- s5
 n10 --- s3
 n3 --- n11
 n1 --- s3
 n16@{ shape: "rounded", label: "rdma_leave_multicast()" }
 n1 --- n16
 n5 ---|"dst_addr"| n12
 n12 --- n16
```
