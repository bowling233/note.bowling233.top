# Linux

!!! quote

    - [Linux Roadmap - roadmap.sh](https://roadmap.sh/linux)
    - [Interactive map of Linux kernel](https://makelinux.github.io/kernel/map/)

## 时间线

Linux 如今仍然在快速发展着。由于 Linux 内核内部 API 并不稳定，因此书籍十分容易过时，而没有很好的后继者。这里对各个专题的书籍进行比较。我们来梳理 Linux 内核的重要时间线：

| 年份 | 版本 | 备注 |
| --- | --- | --- |
| 1991 | 0.01 | |
| 1994 | 1.0 | |
| 1996 | 2.0 | |
| 1999 | 2.2 | netlink (socket version) |
| 2001 | 2.4 | Netfilter |
| 2003 | 2.6 | Intel x86-64 支持<br />抢占（Preemption）、cgroup、Btrfs、ext4 stable、USB 3.0、nouveau<br />较多 Linux 书籍基于 2.6 系列内核 |
| 2011 | 3.0 | |
| 2014 | 3.13 | nftables |
| 2014 | 3.18 | eBPF、OverlayFS |
| 2015 | 4.0 | |
| 2018 | 4.15 | RISC-V |
| 2019 | 5.0 | |
| 2021 | 5.13 | |
| 2022 | 5.19 | LoongArch |
| 2022 | 6.0 | |
| 2022 | 6.1 | Rust |
| 2024 | 6.10 | |

注意到 Linux 2.6 内核从 2003 持续到了 2011 年，在这期间 Linux 的开发模式发生了转变：

> 2.6 marked a significant shift in the development model. Before this, major features would be developed in 2.odd and then possibly backported to 2.even. 2.6 marked the point where features were developed against the existing stable kernel and then landed if they proved sufficiently stable. The difference between 2.0.0 and 2.0.40 is small - the difference between 2.6.0 and 2.6.38 is huge. So, while you'd previously have seen 2.1, 2.3 and 2.5 development kernels, there was never a 2.7. 2.6 demonstrated that modern revision control systems allowed you to engage in active development against a stable target, removing the need for a separate development branch. That's why 2.6 was a turning point.
