---
tags:
  - draft
---

# Linux 网络

!!! todo

    - [ ] 内核网络栈
    - [ ] nftables
    - [ ] NetworkManager

## Linux 网络原理

!!! quote

    - [The Journey of a Packet Through the Linux Network Stack - Dartmouth College](https://www.cs.dartmouth.edu/~sergey/netreads/path-of-packet/Lab9_modified.pdf)：数据包从硬件中断开始如何经过 Linux 系统，各个函数调用负责什么
    - [Linux Kernel Networking - Illinois University](https://caesar.cs.illinois.edu/courses/CS598.S11/slides/raoul_kernel_slides.pdf)：Linux 内核网络栈的介绍
    - [Linux Networking Part 1 : Kernel Net Stack - GitHub](https://amrelhusseiny.github.io/blog/004_linux_0001_understanding_linux_networking/004_linux_0001_understanding_linux_networking_part_1/)：主要是讲概念，不涉及代码

### 内核态网络栈

### 用户态网络栈

## Linux 网络实践

### Linux 网络设备管理

- 网络设备： `/sys/class/net` ，它是到实际设备的链接，如果你需要找到真实设备的位置（比如有多个网卡时）很好用。

```shell
$ ls /sys/class/net -lah
lrwxrwxrwx  1 root root 0 Apr  6 22:50 eth0 -> ../../devices/pci0000:00/0000:00:03.0/virtio0/net/eth0/
lrwxrwxrwx  1 root root 0 Apr  6 22:50 lo -> ../../devices/virtual/net/lo/
$ sysctl -w net.ipv6.conf.enp8s0.disable_ipv6=1
```

- 域名解析：`/etc/resolv.conf`
    - 如果使用 `resolvconf`，则变成一个符号链接。
- DHCP 客户端：`/etc/dhcp/dhclient.conf`

!!! warning "坑点"

    > Some outdated guides instruct to restart the networking service to apply changes to /etc/network/interfaces, however this was deprecated because it is possible that not all interfaces will be restarted. Instead use ifup and ifdown to apply changes to each interface.

    重启 `networking` 服务可能导致部分接口未重启。

### Linux 网络防火墙




