# Understanding Linux Network Internals

## 第一部分

### 第一章：引言

主要介绍内核编程的惯例。

- 两个缩写：BH（Bottom Half）和 IRQ（Interrupt Request）。
- Memory Cache：有些地方会经常使用 `kmalloc` 和 `kfree`。可以先分配一块 memory cache，需要时从里面取出，不需要时放回去。
    - 用到 memory cache 的地方包括 socket buffer、routing tables 等。
    - 相关的内核调用形如：`kmem_cache_create()`、`kmem_cache_alloc()`、`kmem_cache_free()`。
- 路由表等映射中的 cache 通常实现为哈希表，回忆数据结构课程上哈希表有关的内容：动态调整哈希表的大小、哈希冲突解决、如何优化哈希表等。
- 内核中的很多数据结构使用引用计数
    - 相关函数形如 `xxx_release()`、`xxx_hold()`。
    - 如果一个数据结构需要被移除，可以通过消息链告知持有引用计数的对象。
- 垃圾回收有两种形式，不同形式的回收判准不同：
    - 异步：定时器触发回收。
    - 同步：当内存不足时触发回收。
- 内核中广泛使用函数指针和虚函数表（Virtual Function Table, VFT）实现面向对象编程。
- 大端法与小端法：
    - TCP/IP 协议栈都使用大端法。
    - 网络相关编程都需要执行 Host 与 Network 字节序的转换。这些函数包括：

        ```text
        htons htonl ntohs ntohl
        ```

        `s` 表示 `short`，为 2 字节；`l` 表示 `long`，为 4 字节。

此外本章还介绍了 `goto`、向量化定义、预处理器指令、锁、时间等在内核中的使用，以及内核中的调试技术。

### 第二章：关键数据结构

#### `sk_buff`

存储数据包。其中的字段可以归为四类：

- Layout
- General
- Feature-specific
- Management functions

包向下层传递时，调用 `skb_reserve()` 为包头预留空间；向上层传递时，移动指针指向下一层协议的包头。

#### `net_device`

网络设备。
