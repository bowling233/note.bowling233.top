# RDMA

## [infiniband-diags](https://github.com/linux-rdma/rdma-core/blob/master/infiniband-diags/man/infiniband-diags.8.in.rst)

rdma-core 用户空间库提供了管理 RDMA 网络的工具集，这里记录常用的一些：

```bash
# 查询
ibstat
ibstatus
ibaddr
ibnodes
ibhosts
ibrouters
ibswitches
iblinkinfo
ibnetdiscover
# 查询当前 IB 子网管理的位置
sminfo
# 测试
ibping # 注意，需要在另一台机器上使用 ibping -S 作为 server，否则是 ping 不通的。
ibping -c <count> -L <lid>
ibroute
ibtracert
```

## DOCA

`show_gids`

---


### 实用工具



#### OpenSM

IB 管理交换机能够作为 Subnet Manager（SM）来管理 IB 网络。SM 负责发现和配置所有 InfiniBand 设备。

使用非网管 IB 交换机时，IB 网络中缺少 Subnet Manager（SM），此时所有 HCA 状态为 Initializing。可以在其中的一台主机上安装开源的 OpenSM：

```bash
sudo apt install opensm
sudo systemctl enable --now opensm
```

#### mstflint

!!! quote

    - [:simple-github: Mellanox/mstflint: Mstflint - an open source version of MFT (Mellanox Firmware Tools)](https://github.com/Mellanox/mstflint)

`mstflint` 是 Mellanox Firmware Flash Interface 的缩写，用于管理 Mellanox 网卡的固件。诸如切换 RoCE 与 InfiniBand 模式、更新固件等比较底层的操作都可以通过这个工具完成。

下面以将端口模式从 ETH(2) 切换到 IB(1) 为例演示命令用法：

```shell
$ mstconfig query
$ sudo mstconfig -d 4b:00.0 set LINK_TYPE_P1=1

Device #1:
----------

Device type:        ConnectX5
Name:               MCX555A-ECA_Ax_Bx
Description:        ConnectX-5 VPI adapter card; EDR IB (100Gb/s) and 100GbE; single-port QSFP28; PCIe3.0 x16; tall bracket; ROHS R6
Device:             4b:00.0

Configurations:                                     Next Boot       New
        LINK_TYPE_P1                                ETH(2)               IB(1)

 Apply new Configuration? (y/n) [n] : y
Applying... Done!
-I- Please reboot machine to load new configurations.
$ mstfwreset -d 4b:00.0 -l3 -y reset
```

其他命令见 mstflint 发布自带的手册。

??? info "老版本命令留档"

    MLNX_OFED 包含了 mstflint，但一般版本较老，与新版 mstflint 命令不兼容。这里仅作为留档记录一下。

    新版本相比老版本的变化：

    - 不需要执行 `mst start`，服务自动启动。
    - 命令名称发生变化，比如 `mlxconfig` 变为 `mstconfig`。

    ```shell
    # 查看所有设备
    mst status
    # 线缆
    mst cable add # 扫描线缆，线缆一般不会自动被添加
    mlxcables -d e3:00.0_cable_0 -q
    # 链路
    mlxlink
    # 配置
    mlxconfig -d <device> query # 查询详细信息
    Device #1:
    ----------
    Device type:    ConnectX5
    Name:           MCX555A-ECA_Ax_Bx
    Description:    ConnectX-5 VPI adapter card; EDR IB (100Gb/s) and 100GbE; single-port QSFP28; PCIe3.0 x16; tall bracket; ROHS R6
    Device:         31:00.0
    Configurations:                                      Next Boot
            MEMIC_BAR_SIZE                              0
            MEMIC_SIZE_LIMIT                            _256KB(1)
            HOST_CHAINING_MODE                          DISABLED(0)
            HOST_CHAINING_CACHE_DISABLE                 False(0)
            HOST_CHAINING_DESCRIPTORS                   Array[0..7]
            HOST_CHAINING_TOTAL_BUFFER_SIZE             Array[0..7]
            FLEX_PARSER_PROFILE_ENABLE                  0
            FLEX_IPV4_OVER_VXLAN_PORT                   0
            ROCE_NEXT_PROTOCOL                          254
            ESWITCH_HAIRPIN_DESCRIPTORS                 Array[0..7]
            ESWITCH_HAIRPIN_TOT_BUFFER_SIZE             Array[0..7]
            PF_BAR2_SIZE                                0
    ```

#### HCA 卡

使用 `ibstat` 命令可以查看 HCA 卡的状态。只要能在这里看到 HCA 卡，就说明驱动已经加载。

端口的状态有以下几种：

| State | Physical State | 说明 |
| --- | --- | --- |
| Down | Disabled | 未连接线缆 |
| Polling | Polling | 如果持续处于该状态说明 IB 子网没有 SM |
| Active | LinkUp | 连接正常 |

#### DHCP for IPoIB

我们使用 dnsmasq 作为 DHCP 服务器。在 dnsmasq 官方示例中有这样一段：

```text title="dnsmasq.conf"
# Always give the InfiniBand interface with hardware address
# 80:00:00:48:fe:80:00:00:00:00:00:00:f4:52:14:03:00:28:05:81 the
# ip address 192.168.0.61. The client id is derived from the prefix
# ff:00:00:00:00:00:02:00:00:02:c9:00 and the last 8 pairs of
# hex digits of the hardware address.
# dhcp-host=id:ff:00:00:00:00:00:02:00:00:02:c9:00:f4:52:14:03:00:28:05:81,192.168.0.61
```

可能是因为 InfiniBand 迭代，现在已经不适用了。我们在 NVIDIA 的某份文档（忘掉是哪份了）中看到了这样的写法：

```text
id:20 + 硬件地址后 8 对十六进制数字
```

这是可行的。

#### Benchmark

[`perftest`](https://github.com/linux-rdma/perftest) 软件包由 `linux-rdma` 维护，用于测试 InfiniBand 性能，可用于 RoCE。

```bash
# 客户端
ib_write_lat --ib-dev=mlx5_0 --ib-port=1 MAX01-ib -a -R
# 服务端
ib_write_lat --ib-dev=mlx5_0 --ib-port=1 -R -a

ib_send_lat     latency test with send transactions
ib_send_bw      bandwidth test with send transactions
ib_write_lat    latency test with RDMA write transactions
ib_write_bw     bandwidth test with RDMA write transactions
ib_read_lat     latency test with RDMA read transactions
ib_read_bw      bandwidth test with RDMA read transactions
ib_atomic_lat   latency test with atomic transactions
ib_atomic_bw    bandwidth test with atomic transactions
```

#### iproute2

强大的 iproute2 同样支持了 RDMA 网络接口的管理，提供了 `rdma` 命令，语法与 `ip` 命令类似：

```bash
ip link
rdma link
```

#### 其他

一个用于看 IB 卡实时流量的脚本：

```bash
#!/bin/bash

# Author: Chen Jinlong
# Usage: ib_monitor.sh [interval]

declare -A old_recv_bytes;
declare -A old_recv_packets;
declare -A old_xmit_bytes;
declare -A old_xmit_packets;

interval=$1
if [ -z $interval ]; then
    interval=1
fi

for ib_dev in $(ls /sys/class/infiniband/); do
    counter_dir="/sys/class/infiniband/$ib_dev/ports/1/counters"
    old_recv_bytes[$ib_dev]=$(cat $counter_dir/port_rcv_data)
    old_recv_packets[$ib_dev]=$(cat $counter_dir/port_rcv_packets)
    old_xmit_bytes[$ib_dev]=$(cat $counter_dir/port_xmit_data)
    old_xmit_packets[$ib_dev]=$(cat $counter_dir/port_xmit_packets)
done

while true; do
    printf "%-10s %12s %12s %12s %12s\n" Device recv_MBps recv_kpps xmit_MBps xmit_kpps
    for ib_dev in $(ls /sys/class/infiniband/); do
        counter_dir="/sys/class/infiniband/$ib_dev/ports/1/counters"
        new_recv_bytes=$(cat $counter_dir/port_rcv_data)
        new_recv_packets=$(cat $counter_dir/port_rcv_packets)
        new_xmit_bytes=$(cat $counter_dir/port_xmit_data)
        new_xmit_packets=$(cat $counter_dir/port_xmit_packets)

        recv_MBps=$(echo "scale=2; ( $new_recv_bytes - ${old_recv_bytes[$ib_dev]} ) / 256.0 / 1024.0 / $interval" | bc)
        recv_kpps=$(echo "scale=2; ( $new_recv_packets - ${old_recv_packets[$ib_dev]} ) / 1000.0 / $interval" | bc)
        xmit_MBps=$(echo "scale=2; ( $new_xmit_bytes - ${old_xmit_bytes[$ib_dev]} ) / 256.0 / 1024.0 / $interval" | bc)
        xmit_kpps=$(echo "scale=2; ( $new_xmit_packets - ${old_xmit_packets[$ib_dev]} ) / 1000.0 / $interval" | bc)

        printf "%-10s %12s %12s %12s %12s\n" $ib_dev $recv_MBps $recv_kpps $xmit_MBps $xmit_kpps

        old_recv_bytes[$ib_dev]=$new_recv_bytes
        old_recv_packets[$ib_dev]=$new_recv_packets
        old_xmit_bytes[$ib_dev]=$new_xmit_bytes
        old_xmit_packets[$ib_dev]=$new_xmit_packets
    done
    printf "\n"

    sleep $interval
done
```
