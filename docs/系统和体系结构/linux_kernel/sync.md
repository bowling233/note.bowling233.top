# 内核同步方法

## 概述

下列内容摘自《Linux Kernel Development》。

### Causes of Concurrency

> In user-space, the need for synchronization stems from the fact that programs are scheduled **preemptively** at the will of the scheduler. Because a process can be preempted at any time and another process can be scheduled onto the processor, a process can be involuntarily preempted in the middle of accessing a **critical region**.
>
> If the newly scheduled process then enters the same critical region (say, if the two processes manipulate the same shared memory or write to the same file descriptor), a **race condition** can occur. The same problem can occur with multiple single-threaded processes sharing files, or within a single program with **signals**, because signals can occur asynchronously.
>
> This type of concurrency — in which two things do not actually happen at the same time but **interleave** with each other such that they might as well — is called **pseudo-concurrency**.
>
> If you have a **symmetrical multiprocessing (SMP)** machine, two processes can actually be executed in a critical region at the exact same time. That is called **true concurrency**. Although the causes and semantics of true versus pseudo concurrency are different, they both result in the same race conditions and require the same sort of protection.

这段内容介绍了伪并发和真实并发的概念。对于本课程实验的单核系统来说，只存在伪并发的问题。

### Causes of Concurrency in the Kernel

> The kernel has similar causes of concurrency:
>
> - **Interrupts** — An interrupt can occur asynchronously at almost any time, interrupting the currently executing code.
> - **Softirqs and tasklets** — The kernel can raise or schedule a softirq or tasklet at almost any time, interrupting the currently executing code.
> - **Kernel preemption** — Because the kernel is preemptive, one task in the kernel can preempt another.
> - **Sleeping and synchronization with user-space** — A task in the kernel can sleep and thus invoke the scheduler, resulting in the running of a new process.
> - **Symmetrical multiprocessing** — Two or more processors can execute kernel code at exactly the same time.
>
> Kernel developers need to understand and prepare for these causes of concurrency.
>
> It is a major **bug** if:
>
> - An interrupt occurs in the middle of code manipulating a resource, and the interrupt handler can access the same resource.
> - Kernel code is **preempted** while accessing shared resources.
> - Kernel code **sleeps** while inside a critical section.
> - Two processors **simultaneously** access the same piece of data.
>
> With a clear picture of what data needs protection, it is not hard to provide the **locking** needed to keep the system stable. The hard part is **identifying** these conditions and realizing that to prevent concurrency, you need some form of protection.

这段内容介绍了内核中可能引发并发的几种情况：中断、软中断和任务、内核抢占、睡眠和用户空间的同步、多核系统等，并且举例说明了可能引发竞态条件的几种情况。

### 内核中的同步方法

Linux 内核提供的同步方法不再介绍：

- 原子操作
- 锁：
    - 自旋锁（Spinlock）
    - 互斥锁（Mutex）
    - 读写锁（RW Lock）
    - 本地锁（Local Lock）：用于保护 per CPU 数据
- [RCU](https://www.kernel.org/doc/html/next/RCU/whatisRCU.html)

#### RCU

适用场景：只读占多数。

API：五条原语，衍生 API 都靠这五条：

- reader：

    ```c
    rcu_read_lock()
    rcu_dereference()
    rcu_read_unlock()
    ```

- updater:

    ```c
    rcu_assign_pointer()
    ```

- reclaimer:

    ```c
    synchronize_rcu() // 同步
    call_rcu() // 异步
    ```



### 临界区嵌套处理

本节的分析参考了 [Four short stories about `preempt_count()`](https://lwn.net/Articles/831678/)。这篇文章介绍了内核开发者关于内核代码在不同上下文中行为设计的争论，值得一读。

内核的并发来源众多，临界区类型也不同。为了正确处理临界区的嵌套，Linux 内核为每个 Task 设置了计数器，分别追踪：

- Preempt 禁用的嵌套层数
- 被软件、硬件和 NMI 中断禁用的嵌套层数

与此相关的一堆辅助函数如下：

```text
preempt_count()
in_hardirq()
in_nmi()
irqs_disabled()
...
```

通过这个计数器，内核可以正确地处理不同类型临界区的嵌套问题。

### Slab

- Create：

    因该操作可能更改 Slab 分配器多层次的数据结构，使用**互斥锁**保护整个操作过程。

    ```c title="mm/slab_common.c"
    struct kmem_cache *__kmem_cache_create_args(const char *name,
            unsigned int object_size,
            struct kmem_cache_args *args,
            slab_flags_t flags)
    {
        mutex_lock(&slab_mutex);
        //...
        goto out_unlock;
    out_unlock:
        mutex_unlock(&slab_mutex);
        return s;
    }
    ```

- Alloc：支持快速和慢速两条路径

    ```text
    kmem_cache_alloc()
    → slab_alloc_node()
    ```

    - 快速路径：

        ```text
        kfence_alloc()
        ```

        使用原子操作分配本地 CPU 的缓存对象，无需锁。

    - 慢速路径：

        ```text
        __slab_alloc_node()
        → __slab_alloc()
        → ___slab_alloc()
        ```

        Slab 缓存中有一些 per CPU 数据，因此同时控制 **Local Lock 和中断**来防止**相同 CPU 上**的并发访问。

        ```c title="mm/slub.c"
        local_lock_irqsave(&s->cpu_slab->lock, flags);
        freelist = get_freelist(s, slab);
        lockdep_assert_held(this_cpu_ptr(&s->cpu_slab->lock));
        c->freelist = get_freepointer(s, freelist);
        local_unlock_irqrestore(&s->cpu_slab->lock, flags);
        return freelist;
        ```

        其中的 `get_freelist()` 内对全局/节点的 partial/full slab list 加**自旋锁**，防止**多 CPU 并发**修改 slab 列表。

        ```c title="mm/slub.c"
        bit_spin_lock(PG_locked, &slab->__page_flags);
        ```

### 调度

调度器中主要保护就绪队列（Runqueue）。每个 CPU 有一个独立的就绪队列，用于存放该 CPU 上可运行的进程。

`__schedule()` 中使用**自旋锁**保护就绪队列：

```c title="kernel/sched/core.c"
rq_lock(rq, &rf);
```

```c title="kernel/sched/sched.h"
static inline void rq_lock(struct rq *rq, struct rq_flags *rf)
    __acquires(rq->lock)
{
    raw_spin_rq_lock(rq);
    rq_pin_lock(rq, rf);
}
```

在上下文切换（`context_switch()`）完成后的 `finish_task_switch()` 中释放锁：

```c title="kernel/sched/core.c"
finish_lock_switch(rq);
```

中断也做了相应控制，在前一篇文章中介绍过。