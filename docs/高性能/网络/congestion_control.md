# 流量/拥塞控制

- **拥塞（Congestion）**：交换机/路由器的收包持续大于发包，导致缓冲区溢出、出现丢包的现象。

## 拥塞通知（Congestion Notification）

### QCN/BCN

!!! quote

    - [Support - 05-QCN configuration- H3C](https://www.h3c.com/en/d_202110/1475565_294551_0.htm)

- **原理**：
    - 交换机检测到拥塞时，向**发送端**发送拥塞通知。
    - 发送端收到反馈包后，调整发送速率以缓解拥塞。
- **实现**：[IEEE 802.1Qau – Congestion Notification](https://1.ieee802.org/dcb/802-1qau/)
    - 使用源/目的 MAC 地址和流 ID 来标识流。
    - TODO 具体计算拥塞细节待补充。
- **优点**：基于反馈的拥塞控制，防止 PFC 级联导致全局阻塞（PFC storm）。

### ECN

!!! quote

    - [Support - Intelligent Lossless Network Technology White Paper-6W100- H3C](https://www.h3c.com/en/Support/Resource_Center/EN/Home/Public/00-Public/Technical_Documents/Technology_Literature/Technology_White_Papers/H3C_WP-18625/)

- **原理**：
    - 当路由器检测到拥塞时，在数据包头中设置 ECN 标志位。
    - **接收端**收到带有 ECN 标志的数据包后，**通知发送端**减缓发送速率。
- **实现**：
    - [RFC 3168: The Addition of Explicit Congestion Notification (ECN) to IP](https://www.rfc-editor.org/rfc/rfc3168)
    - IPv4 和 IPv6 使用 IP 头中 Traffic Class 字段的两个比特位来表示 ECN 状态。
    - ECN 有四种状态：不支持 ECN（00）、支持但未拥塞（01 或 10）、拥塞发生（11）。

### DCQCN

## 拥塞避免（Congestion Avoidance）



### Ethernet

- **原理**：IEEE 802.3 定义的 Pause mechanism。接收端发送 Pause 帧给发送端，要求其暂停发送数据一段时间，以缓解拥塞。
- **优点**：能预防丢包。
- **缺点**：导致服务中断，不可接受。

### PFC

- **原理**：
    - 相比 Ethernet Pause，能够划分不同优先级的流量队列，分别控制是否暂停。
    - 拥塞缓解时发送 Resume 帧恢复传输。
- **实现**：[802.1Qbb – Priority-based Flow Control](https://1.ieee802.org/dcb/802-1qbb/)
- **优点**：
    - 避免了整体暂停带来的服务中断问题。
    - 能够更细粒度地控制拥塞。
- **缺点**：
    - 粒度还是太粗了，无法区分同一优先级下的不同流量。后续改进包括逐流和逐包的流量控制。

#### PFC 存在的问题

这一问题有以下称呼：

- head-of-Line Blocking（队头阻塞）
- PFC Storm/Cascade

启用了 PFC 的无丢包网络引入了一种新的故障场景：

- 当一个端口上的某个队列无法从网络中接收任何流量时，它会不断向交换机发送暂停帧（pause frame）。
- 由于在无丢包网络中，交换机路径不会丢弃数据包，而是在其缓冲区填满时拒绝接收更多数据包，因此如果终端端口的队列长时间“卡住”，就会导致：
    - 目标交换机的缓冲区被占满，与之相关的转发路径上所有交换机的端口队列也可能被撑满。
    - 结果，沿着流量路径的所有交换机端口都会不断地观察到 PFC 暂停帧风暴（PFC storm），即暂停帧无休止地传播。

PFC Watchdog 的作用就是为了防止这种情况下拥塞在网络中扩散。当交换机在某个队列上检测到这种情况时：

- 会清空（flush）该队列中的所有数据包；
- 并暂时丢弃所有发往同一队列的新数据包；
- 直到 PFC 风暴现象被解除。

## CBFC

!!! quote

    - [InfiniBand Credit-Based Link-Layer Flow-Control ](https://www.ieee802.org/1/files/public/docs2014/new-dcb-crupnicoff-ibcreditstutorial-0314.pdf)

    

## 拥塞控制（Congestion Control）


