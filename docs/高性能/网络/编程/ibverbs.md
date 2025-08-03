# IB Verbs

!!! quote

    - 阅读 [InfiniBand: An Introduction + Simple IB verbs program with RDMA Write - Service Engineering (ICCLab & SPLab)](https://blog.zhaw.ch/icclab/infiniband-an-introduction-simple-ib-verbs-program-with-rdma-write/)，了解 PD、MR、QP、CQ、WR、SGE、WC 等基本概念。
    - 阅读 [RDMA Tutorial - Netdev](https://netdevconf.info/0x16/slides/40/RDMA%20Tutorial.pdf)，其中介绍了 `ipv_pd` 等重要的 API。
    - 阅读 [Introduction to Programming Infiniband RDMA · Better Tomorrow with Computer Science](https://insujang.github.io/2020-02-09/introduction-to-programming-infiniband/)，这篇文章逐步讲解了如何编写一个简单的 RDMA 程序，并给出了详细的代码。
    - 阅读 [InfiniBandTM Architecture Specification Volume 1](https://www.afs.enea.it/asantoro/V1r1_2_1.Release_12062007.pdf)，规范中的第十章定义了软件接口，即 IB Verbs。
    - [Mellanox Adapters Programmer’s Reference Manual](https://network.nvidia.com/files/doc-2020/ethernet-adapters-programming-manual.pdf)：该手册适用于 CX4，详细地描述了 IB Verbs 编程的所有细节，细致到每个字段的定义。
    - [RDMA Aware Networks Programming User Manual - NVIDIA Docs](https://docs.nvidia.com/networking/display/rdmaawareprogrammingv17)：包含 RDMA 架构概述和 IB Verbs、RDMACM 的 API 文档。**该文档第八章包含了各层次 API 编程的例子，具有比较详细的注释，适合初学者学习。**本文中的部分编程示例来自该手册。

- 头文件：`infiniband/verbs.h`
- API 前缀：`ibv_`
- 软件包：`libibverbs-dev`

## 例程

代码见 [`RDMA_RC_example.c`](./index.assets/code/RDMA_RC_example.c)。

准备阶段：

- `resource_create()`：创建资源，包括 PD、MR、QP、CQ 等。
- `connect_qp()`：通信双方交换信息，包括 LID、QP_NUM、RKEY 等，将 QP 状态更改为 INIT、RTR、RTS。
    - `sock_sync_data()`：通过 TCP 通信交换信息。
    - `modify_qp_to_init()`
    - `post_receive()`：预置接收队列，也可以放在通信阶段。
    - `modify_qp_to_rtr()`
    - `modify_qp_to_rts()`
    - 同步点

```mermaid
flowchart TD
 subgraph s1["resource_create()"]
  n27@{ shape: "rounded", label: "ibv_query_port()" }
  n26@{ shape: "hex", label: "ibv_port_attr" }
  n25@{ shape: "hex", label: "ibv_mr" }
  n17@{ shape: "hex", label: "char *" }
  n16@{ shape: "rounded", label: "ibv_get_device_name()" }
  n15@{ shape: "rounded", label: "ibv_get_device_list()" }
  n14@{ shape: "hex", label: "ibv_device" }
 n1@{ shape: "hex", label: "ibv_pd" }
 n2@{ shape: "rounded", label: "ibv_alloc_pd()" }
 n3@{ shape: "hex", label: "ibv_context" }
 n4@{ shape: "hex", label: "buf" }
 n3 --- n2
 n2 --- n1
 n5@{ shape: "rounded", label: "ibv_open_device()" }
 n5 --- n3
 n6@{ shape: "rounded", label: "ibv_create_cq()" }
 n3 --- n6
 n7@{ shape: "hex", label: "mr_flags" }
 n8@{ shape: "rounded", label: "ibv_reg_mr" }
 n4 --- n8
 n7@{ shape: "fr-rect", label: "mr_flags<br/>=IBV_ACCESS_REMOTE_READ|..." } --- n8
 n1 --- n8@{ shape: "rounded", label: "ibv_reg_mr()" }
 n9@{ shape: "hex", label: "ibv_qp_init_attr" }
 n10@{ shape: "hex", label: "ibv_cq" }
 n6 --- n10
 n9@{ shape: "hex", label: "ibv_qp_init_attr" }
 n10 ---|"send_cq, recv_cq"| n9
 n11@{ shape: "fr-rect", label: "qp_type<br/>=IBV_QPT_RC" }
 n11 --- n9
 n12@{ shape: "rounded", label: "ibv_create_qp()" }
 n9 --- n12
 n13@{ shape: "hex", label: "ibv_qp" }
 n12 --- n13
 end
 n15 --- n14@{ shape: "hex", label: "ibv_device **" }
 n14 --- n16
 n16 --- n17
 n14 --- n5
 subgraph s2["connect_qp()"]
  n36@{ shape: "rounded", label: "sock_sync_data()" }
  subgraph s3["cm_con_data_t"]
   n24@{ shape: "hex", label: "buf" }
   n23@{ shape: "hex", label: "lid" }
   n22@{ shape: "hex", label: "qp_num" }
   n20@{ shape: "hex", label: "rkey" }
   n21@{ shape: "hex", label: "gid" }
  end
  n18@{ shape: "hex", label: "ibv_gid" }
  n19@{ shape: "rounded", label: "ibv_query_gid()" }
 end
 n3 --- n19
 n19 --- n18
 n18 --- n21
 n8 --- n25
 n25 --- n20
 n4 --- n24
 n13 --- n22
 n27 --- n26
 n3 --- n27
 n26 --- n23
 n29 --- n30
 n13 --- n30
 n28 --- n29
 subgraph s5["post_receive()"]
  n33@{ shape: "rounded", label: "ibv_post_recv" }
  n32@{ shape: "hex", label: "ibv_recv_wr" }
  n31@{ shape: "hex", label: "ibv_sge" }
 end
 n25 ---|"lkey"| n31
 n4 --- n31
 subgraph s4["modify_qp_to_init, rts()"]
  n28@{ shape: "fr-rect", label: "qp_state<br/>=IBV_QPS_INIT" }
  n30@{ shape: "rounded", label: "ibv_modify_qp()" }
  n29@{ shape: "hex", label: "ibv_qp_attr" }
 end
 n31 --- n32
 n32 --- n33
 subgraph s6["modify_qp_to_rtr()"]
  n34@{ shape: "rounded", label: "Rounded Rectangle" }
  n35@{ shape: "hex", label: "ibv_qp_attr" }
 end
 n22 --- n35
 n23 --- n35
 n35 --- n34@{ shape: "rounded", label: "ibv_modify_qp()" }
 s3 --- n36
```

通信阶段：

- `post_send()`：创建并发送 WR，WR 的类型取决于 `opcode`。
- `poll_completion()`：轮询得到 WC。

```mermaid
flowchart TD
 subgraph s1["post_send()"]
  n12@{ shape: "rounded", label: "ibv_post_send()" }
  n7@{ shape: "fr-rect", label: ".opcode<br/>IBV_WR_SEND<br/>IBV_WR_RDMA_READ<br/>IBV_WR_RDMA_WRITE" }
  n1@{ shape: "hex", label: "ibv_sge" }
  n2@{ shape: "hex", label: "ibv_send_wr" }
 end
 n3@{ shape: "hex", label: "ibv_mr" }
 n3 ---|".lkey"| n1
 n4@{ shape: "hex", label: "buf" }
 n4 --- n1
 n1 --- n2
 subgraph s2["IBV_WR_SEND only"]
  n5@{ shape: "hex", label: ".rkey" }
  n6@{ shape: "hex", label: ".remote_addr" }
 end
 s2 ---|".wr.rdma"| n2
 n7 --- n2
 subgraph s3["poll_completion()"]
  n11@{ shape: "rounded", label: "assert()" }
  n10@{ shape: "rounded", label: "ibv_poll_cq()" }
  n9@{ shape: "hex", label: "ibv_wc" }
 end
 n8@{ shape: "hex", label: "ibv_cq" }
 n9 --- n10
 n8 --- n10
 n10 ---|".status == IBV_WC_SUCCESS"| n11
 n2 --- n12
```

该程序演示了下面的操作：

- `resource_create()`：服务端把 `SEND operation` 字符串放在缓冲区 `res->buf` 中。
- `connect_qp()`：交换资源信息，远端信息放入 `res->remote_props`。交换内容包括 `res->buf` 的地址。Client 向 Server 发送一个 Receive。
- `post_send()`：Server 发送一个 Send。该 WR 的构成：
    - `.sg_list->addr` 为 `res->buf`，即 Server 的缓冲区地址。
    - `.wr.rdma.remote_addr` 为 `res->remote_props.addr`，即 Client 的缓冲区地址。
- `poll_completion()`：Client 收到并显示信息 `SEND operation`。
- Server 再将缓冲区内容修改为 `RDMA read operation`。
- `post_send()`：Client 发送一个 read 操作，读取到 `RDMA read operation`。因为这是单边操作，Server 不会知道。
- Client 将缓冲区内容修改为 `RDMA write operation`。
- `post_send()`：Client 发送一个 write 操作，写入到 Server 的缓冲区。
- Server 打印缓冲区内容，为 `RDMA write operation`。

## 能力归纳

梳理 IB Verbs 的全链路，各调用可能需要网卡提供不同的能力：

- `ibv_reg_mr()` 访问控制：

    ```c
    enum ibv_access_flags {
        IBV_ACCESS_LOCAL_WRITE		= 1,
        IBV_ACCESS_REMOTE_WRITE		= (1<<1),
        IBV_ACCESS_REMOTE_READ		= (1<<2),
        IBV_ACCESS_REMOTE_ATOMIC	= (1<<3),
        IBV_ACCESS_MW_BIND		= (1<<4),
        IBV_ACCESS_ZERO_BASED		= (1<<5),
        IBV_ACCESS_ON_DEMAND		= (1<<6),
        IBV_ACCESS_HUGETLB		= (1<<7),
        IBV_ACCESS_FLUSH_GLOBAL		= (1 << 8),
        IBV_ACCESS_FLUSH_PERSISTENT	= (1 << 9),
        IBV_ACCESS_RELAXED_ORDERING	= IBV_ACCESS_OPTIONAL_FIRST,
    };
    ```

- `struct ibv_qp_init_attr`：

    ```c
    enum ibv_qp_type {
        IBV_QPT_RC = 2,
        IBV_QPT_UC,
        IBV_QPT_UD,
        IBV_QPT_RAW_PACKET = 8,
        IBV_QPT_XRC_SEND = 9,
        IBV_QPT_XRC_RECV,
        IBV_QPT_DRIVER = 0xff,
    };
    ```

- `struct ibv_send_wr->opcode`

    ```c
    const char *ibv_wr_opcode_str(enum ibv_wr_opcode opcode);
    enum ibv_wr_opcode {
        IBV_WR_RDMA_WRITE,
        IBV_WR_RDMA_WRITE_WITH_IMM,
        IBV_WR_SEND,
        IBV_WR_SEND_WITH_IMM,
        IBV_WR_RDMA_READ,
        IBV_WR_ATOMIC_CMP_AND_SWP,
        IBV_WR_ATOMIC_FETCH_AND_ADD,
        IBV_WR_LOCAL_INV,
        IBV_WR_BIND_MW,
        IBV_WR_SEND_WITH_INV,
        IBV_WR_TSO,
        IBV_WR_DRIVER1,
        IBV_WR_FLUSH = 14,
        IBV_WR_ATOMIC_WRITE = 15,
    };
    ```

- 

## SRQ

SRQ 作为 `struct ibv_qp_init_attr` 中的一个**可选**字段。

## `ibv_wr_*`



## `*_ex`
