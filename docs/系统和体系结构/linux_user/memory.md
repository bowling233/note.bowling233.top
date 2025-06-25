# 内存

## 系统调用

### mmap

先回顾内核 vma 的知识。

- `vma_area_struct`
    - `vma_operations_struct`：
        - `open`：VMA 的新引用产生时调用
        - `close`：VMA 被销毁时调用
        - `nopage`：命中 VMA 但页面不在内存时调用
        - `populate`：在初次访问前先准备好页面，相当于提前触发一次 page fault

`mmap` 让用户空间程序能直接访问：

- 设备内存。否则程序需要使用 `lseek/write` 系统调用访问设备内存。
- PCI 设备的 configuration register。否则程序需要使用 `ioctl` 系统调用访问。

Syscall 接口：

```c
mmap (caddr_t addr, size_t len, int prot, int flags, int fd, off_t offset)
```

File operation 接口：

```c
int (*mmap) (struct file *filp, struct vm_area_struct *vma);
```

设备驱动实现 `mmap` 主要完成以下内容：

- 设置合适的页表映射
    - `remap_pfn_range()`
    - `io_remap_pfn_range()`
- 需要时替换 `vma->vm_ops`

### madvise

### shmget/shmat

### `set_mempolicy`

<https://man7.org/linux/man-pages/man2/set_mempolicy.2.html>

```c
#include <numaif.h>
long set_mempolicy(int mode, const unsigned long *nodemask,
                          unsigned long maxnode);
```

设置该 thread 的内存分配策略。`nodemask` 是位掩码，`maxnode` 表示有多少位。

- 子线程继承。
- 分配模式：

    |模式 | 描述|
    |-|-|
    |`MPOL_DEFAULT`|默认从离 CPU 最近的节点分配内存|
    |`MPOL_BIND`|**只**从指定节点分配内存|
    |`MPOL_INTERLEAVE`|在`nodemask`指定的节点中轮流分配内存|
    |`MPOL_WEIGHTED_INTERLEAVE`|按`/sys/kernel/mm/mempolicy/weighted_interleave`配置的权重在节点间分配内存|
    |`MPOL_PREFERRED`|**优先**从指定节点分配内存|
    |`MPOL_PREFERRED_MANY`|类似`MPOL_PREFERRED`，但允许多个节点|
    |`MPOL_LOCAL`|从本地节点分配内存|
