# 内核模块与设备驱动

## 使用和调试

寻找内核模块的帮助信息：

```shell
lsmod
modinfo
```

模块参数：

```shell
modprobe <module> param=value

# 内置在 initramfs 的模块
# /etc/modprobe.d/*.conf
options <module> param=value
# 更新 initramfs

# built-in 模块
# /etc/default/grub
GRUB_CMDLINE_LINUX_DEFAULT="... <module>.param=value ..."
```

Dynamic debug：

!!! quote

    - [Dynamic debug — The Linux Kernel documentation](https://www.kernel.org/doc/html/v5.0/admin-guide/dynamic-debug-howto.html)
    - [Dynamic debug—kernel debugging messages | System Analysis and Tuning Guide | SLES 15 SP7](https://documentation.suse.com/sles/15-SP7/html/SLES-all/cha-tuning-dynamic-debug.html)

```shell
grep <module> /sys/kernel/debug/dynamic_debug/control
echo "module <module> +p" > /sys/kernel/debug/dynamic_debug/control
```

## Userspace I/O (UIO)

https://egeeks.github.io/kernal/uio-howto/index.html

## Virtual Function I/O (VFIO)

https://docs.kernel.org/driver-api/vfio.html

https://zhuanlan.zhihu.com/p/689107103

在保证直通设备的DMA安全性同时可以达到接近物理设备的I/O的性能。 用户态进程可以直接使用VFIO驱动直接访问硬件，并且由于整个过程是在IOMMU的保护下进行因此十分安全， 而且非特权用户也是可以直接使用。 换句话说，VFIO是一套完整的用户态驱动(userspace driver)方案，因为它可以安全地把设备I/O、中断、DMA等能力呈现给用户空间。
