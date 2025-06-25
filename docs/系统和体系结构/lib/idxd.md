# Intel DSA

- [Intel® Data Streaming Accelerator User Guide](https://www.intel.com/content/www/us/en/content-details/759709/intel-data-streaming-accelerator-user-guide.html)
- [Intel® Data Streaming Accelerator Architecture Specification](https://www.intel.com/content/www/us/en/content-details/857060/intel-data-streaming-accelerator-architecture-specification.html)

## 概述

DSA 加速节点内外各种内存的数据传输和转换操作。内存的类型例：

- 本地持久/非持久内存
- MMIO（设备的内存）
- CXL 等远端内存

DSA 相比历代芯片的优势：

- 支持跨地址空间操作。

    应用场景：

    - VM 的检查点、迁移
    - vSwitch 的数据转发、IPC 通信

    原理：

    - PASID 字段
    - 利用 PCIe 的 ATS、PRS 功能

## idxd 驱动 - 用户态

- [](https://github.com/intel/idxd-config)

该驱动管理 DSA 和 IAA 设备。

### SVM

Shared Virtual Memory：设备工作在与应用程序相同的虚拟地址空间中，此时使用 PASID 区分不同应用的内存空间。背后的原理：

> they use the PCI Express Address Translation Services (ATS) and Page Request Services (PRS) capabilities to implement recoverable device page faults

需要阅读 Architecture Specification 了解上面的整个过程，为什么这样做可以不 pin 内存。

### sysfs 接口

sysfs 接口：`/sys/bus/dsa`，配置后设备出现在 `/dev/dsa/wqD.Q`，程序向该设备文件提交任务。

一般使用 `accel-config` 工具配置设备，更加方便。写一个 `.conf` 文件然后用它加载。

### 设备控制

结构：

- group 组织 WQs 和 engines，起到 isolation 作用

Read Buffer：通过控制 group 的 read buffer 可以近似控制带宽，可配置两个指标：

- allowed
- reserved

## idxd 驱动 - 内核态

## DSA 编程

### Portal 映射

程序必须先将 Portal 映射到自己的地址空间，然后才能提交 descriptor。

要映射 IO 空间的地址，程序必须具有 `CAP_SYS_RAW_IO` 权限。否则只能调用操作系统驱动的 `write()` 系统调用提交 descriptor。

设备文件的 mmap 函数见 `drivers/dma/idxd/cdev.c`。

### descriptor 提交

底层有两种方式：

- `enqcmd(s)` 用于 SWQ，其中 `s` 后缀的用于 supervisor 模式
- `movdir64b` 用于 DWQ

两种指令的格式都是 `r, [addr]`，其中 `r` 存放 Portal 的地址，`addr` 存放 descriptor 的地址。

考虑用户态程序的接口：

1. 使用驱动提供的 `write()` 系统调用，也就是用户态程序打开 cdev 然后写入 descriptor。

    驱动程序执行下列操作：

    - `copy_from_user`
    - 根据 WQ 类型选择 `enqcmds` 或 `movdir64b`

    源代码见 `drivers/dma/idxd/cdev.c`。

2. 用 mmap 把 cdev 映射到内存空间，直接使用 `enqcmd`/ `movdir64b` 指令提交 descriptor。

