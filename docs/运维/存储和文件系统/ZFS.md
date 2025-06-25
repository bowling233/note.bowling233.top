# ZFS

!!! quote

    - [Oracle ZFS](https://docs.oracle.com/en/operating-systems/solaris/oracle-solaris/11.4/manage-zfs/index.html)
    - [OpenZFS](https://openzfs.github.io/openzfs-docs/Getting%20Started/index.html)

- **历史**：ZFS 由 Sun 公司开发，随着 OpenSolaris 开源。当 Oracle 收购 Sun 后，ZFS 转为闭源。ZFS 的开源分支从 OpenSolaris 派生而来，现由 [:simple-github: OpenZFS](https://github.com/openzfs) 维护。
- **发展情况**：ZFS 现有两种实现，一种是 OpenZFS（大多数人使用的），一种是向 Oracle 公司购买存储服务。这两种实现的大部分用法一致，但有一些细微不同（比如属性设置）。Oracle 的文档很适合入门，OpenZFS 的文档阅读难度较高。可以按照 Oracle 的文档来学习，但遇到不同的用法时应当查 OpenZFS 文档。

## ZFS 原理

!!! quote

    - [ZFS 分層架構設計 - Farseerfc 的小窩](https://farseerfc.me/zhs/zfs-layered-architecture-design.html)：关于 ZFS 原理的全面的索引，给出了非常多的参考资料和视频链接。

- 底层：SPA
    - VDEV：多设备存储池的抽象。设备类型：
        - stripe
        - mirror
        - raidz
        - l2arc：用 SSD 分担内存 ARC 的负担，增加缓存命中率。
        - slog
    - ZIO：I/O 调度器，使用流水线
    - MetaSlab：块分配器
    - ARC：类似于 Linux page buffer cache
- 上层：
    - ZIL：ZFS Intent Log，记录两次完整事务语义提交之间的日志，用来加速实现 fsync 之类的文件事务语义。断电后重放 ZIL。
- 对外接口：

## ZFS 功能

- **擦洗（Scrub）**：
    - 检查存储池中数据块的完整性
    - 为了应对**静默数据损坏（bit rot）**，建议至少每月对 ZFS 存储池进行一次擦洗。

    ```shell
    systemctl enable zfs-scrub-monthly@<pool_name>.timer
    ```

- **修剪（Trim）**：
    - 通过 TRIM 命令来通知 SSD 哪些数据块不再使用，SSD 可以将这些数据块标记为可用空间。

