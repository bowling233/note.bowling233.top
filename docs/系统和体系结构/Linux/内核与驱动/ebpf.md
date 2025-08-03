# eBPF

!!! quote

    这方面最好的教材是《BPF Performance Tools》，请阅读该书。本篇笔记仅记录工作中解决问题时的实操经验。

[Learn eBPF Tracing: Tutorial and Examples](https://www.brendangregg.com/blog/2019-01-01/learn-ebpf-tracing.html)。

## 内核支持情况

| 内核版本 | 功能 |
| - | - |
| 5.3 | [有界循环](https://lwn.net/Articles/773605/) |

## 基本概念

- **事件驱动**：在内核/应用程序**事件**发生时运行一小段程序
- 从概念上实现为一个虚拟机，实际上 JIT 后直接在处理器上运行

| | 静态插桩 | 动态插桩 |
| --- | --- | --- |
| 对象 | 开发者维护的事件名字 |内核或应用函数的开始或结束位置 |
| 无法工作的情况 | | 软件版本变更、重命名、编译器内联优化等 |

### ftrace

### kprobe/uprobe

!!! quote

    - [Linux uprobe: User-Level Dynamic Tracing](https://www.brendangregg.com/blog/2015-06-28/linux-ftrace-uprobe.html)

    Linux 3.14 得到完善支持。

插桩的原理是将目标位置的内容替换为中断指令，由中断处理函数检查断点是否由 kprobe 注册，然后执行 kprobe 处理函数。

如果是 kretprobe，则替换返回地址为 trampoline 函数。

### tracepoint/USDT

内核中有 100+ 跟踪点，在版本间稳定性比 probe 好。格式为 `subsystem:eventname`。

在内核编译时，跟踪点是 nop 指令。启用时，替换为 jump 指令，遍历一个回调函数数组，该数组由跟踪器动态插入回调函数。

在用户空间，USDT 有许多实现，比如：

- Facebook 的 Folly 库：[folly/folly/tracing/README.md at main · facebook/folly](https://github.com/facebook/folly/blob/main/folly/tracing/README.md)
- rdma-core 使用的 [LTTNG](https://lttng.org/)



### `perf_events`



## 工具

### bcc

BCC 能够在函数的任意偏移处插桩，而 bpftrace 只能在入口或出口插桩。

现成的工具：

- BCC：`/usr/share/bcc/tools/` 或 `/usr/sbin`，有时带有 `-bpfcc` 后缀。

### bpftrace

!!! quote

    - [docs | bpftrace](https://bpftrace.org/docs/latest)

    需要经常参考的地方：

    - 标准库内置变量、内置函数

- 常用工具位于：`/usr/share/bpftrace/tools/`。
- 库函数：

    ```text
    printf
    time
    join
    str
    ```

三种变量：

- 内置变量：

    ```text
    pid, tid, uid, username
    nsecs, elapsed
    cpu, comm
    kstack, ustack
    arg0, argN, retval
    func, probe, curtask, cgroup
    ```

- 临时变量：仅块内可用

    ```text
    $name
    ```

- 映射表变量：全局可用

    ```text
    @name[key1, key2[, ...]]
    ```

控制流：

- 在 5.3 引入有界循环支持前，只能使用 `unroll` 展开循环。

### libbpf

## 实例

### rdma-core



