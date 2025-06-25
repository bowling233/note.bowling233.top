# Logical Volume Manager

!!! quote

    - [Configuring and managing logical volumes | Red Hat Product Documentation](https://docs.redhat.com/en/documentation/red_hat_enterprise_linux/9/html/configuring_and_managing_logical_volumes/index)

LVM 结构：

- 物理卷（Physical Volume，PV）可以是磁盘、磁盘分区或其他存储设备。
- 卷组（Volume Group，VG）是一个或多个 PV 的集合。
- 逻辑卷（Logical Volume，LV）是从 VG 中划分出来的虚拟磁盘，可以在上面创建文件系统。

LVM 原理：

- 扩展（Extent）是 LVM 中的基本分配单位，物理扩展（Physical Extent，PE）和逻辑扩展（Logical Extent，LE）默认大小为 4MiB。
- 在 VG 中创建 LV 时，会在 PV 中分配 PE，LV 中的 LE 和 PE 一一对应。

管理 LVM：

- 需要安装 `lvm2` 包。
- PV

    - 初始化物理卷时会写入一些元数据。

    ```bash
    pvcreate /dev/sdb
    pvs
    pvremove /dev/sdb
    ```

- VG

    ```bash
    vgcreate vg0 /dev/sdb
    vgs
    vgrename vg0 vg1
    vgextend vg1 /dev/sdc
    vgmerge vg1 vg2
    vgsplit vg1 vg2 /dev/sdb
    vgremove vg1
    ```

    移除 PV 时，需要先迁移数据：

    ```bash
    pvmove /dev/sdb
    vgreduce vg1 /dev/sdb
    ```

    将 VG 迁移到其他系统：

    ```bash
    vgchange --activate n vg1
    vgexport vg1
    vgimport vg1
    vgchange --activate y vg1
    ```

- LV

    ```bash
    lvcreate --name lv0 --size 10G vg0
    lvcreate --stripes 2 --stripesize 64K --name lv1 --size 10G vg0
    lvcreate --type raid1 --mirrors 1 --name lv2 --size 10G vg0
    lvcreate --type raid6 --stripes 2 --stripesize 64K --name lv2 --size 10G vg0
    ```

快照：

- 创建快照就是创建一个 LV 的只读副本，引用原 LV 的数据。
- 当原 LV 的数据发生变化时，CoW 机制将原始数据拷贝到快照 LV 中。随着时间推移，快照 LV 中的数据会越来越多，超出快照 LV 的大小后该快照失效。
- 可以手动扩展快照 LV 的大小，或者在 `/etc/lvm/lvm.conf` 中配置自动扩展。

```bash
lvcreate --snapshot --size 10G --name snap0 vg0/lv0
lvextend --size +10G vg0/snap0
```

```text title="lvm.conf"
snapshot_autoextend_threshold = 70
snapshot_autoextend_percent = 20
```

- 合并快照 LV 和原 LV 就是恢复操作：

    ```bash
    lvchange --activate n vg0/snap0
    lvchange --activate n vg0/lv0
    lvconvert --merge snap0
    lvchange --activate y vg0/lv0
    ```

缓存：

- `dm-cache`

    ```bash
    lvcreate --type cache-pool --name CachePoolName --size Size VolumeGroupName /FastDevicePath
    lvconvert --type cache --cachepool VolumeGroupName/CachePoolName VolumeGroupName/LogicalVolumeName
    ```

- `dm-writecache`

    ```bash
    lvcreate --name CacheVolumeName --size Size VolumeGroupName /FastDevicePath
    lvconvert --type writecache --cachevol CacheVolumeName VolumeGroupName/LogicalVolumeName
    ```

RAID0：

    
