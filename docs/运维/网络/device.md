# 设备与接口

!!! quote

    - [NetworkConfiguration - Debian Wiki](https://wiki.debian.org/NetworkConfiguration)

## 网络设备

`/sys/class/net` 包含了网络接口的所有信息。一些常用信息举例如下：

```text
/sys/class/net/enp175s0f0np0 -> ../../devices/pci0000:aa/0000:aa:01.0/0000:ab:00.0/0000:ac:02.0/0000:af:00.0/net/enp175s0f0np0
├── address 硬件地址
├── device -> ../../../0000:af:00.0 总线位置
│   ├── driver -> ../../../../../../bus/pci/drivers/mlx5_core 驱动名称
```

## 接口配置

虽然下文介绍的各类方式可能可以共存，但在一个系统上，应当只使用其中一种方式进行网络配置，以避免冲突。

### [ifupdown](https://github.com/ifupdown-ng/ifupdown-ng)

最传统的网络配置方式，支持的功能较少，不推荐使用。

- 命令：`ifup`、`ifdown`
- Systemd 服务 `networking`
- 配置文件：`/etc/network`

### [NetworkManager](https://networkmanager.dev/)

在桌面环境中较为常见，但笔者并不经常使用桌面，因此不详细介绍。

- 命令：`nmcli`、`nmtui`
- 配置文件：`/etc/NetworkManager`
- Systemd 服务：`NetworkManager`

NetworkManager 将网络抽象为设备和连接：

- **设备：**物理设备，如网卡。
- **连接：**设备的配置，如 IP 地址、DNS 服务器等。可以创建多个连接，但每个设备只能有一个连接。配置保存在 `/etc/NetworkManager/system-connections`

### [systemd-networkd](https://www.freedesktop.org/software/systemd/man/latest/systemd.network.html)

笔者并不喜欢 systemd 的配置文件格式，因此不详细介绍。

- 命令：`networkctl`
- 配置文件：`/etc/systemd/network`
- Systemd 服务：`systemd-networkd`

### [Netplan](https://netplan.io/)

Netplan 对网络配置进行了高层次的抽象，支持多种后端（NetworkManager、systemd-networkd）。配置文件使用 YAML 格式，易于阅读和编写。笔者推荐使用 Netplan 作为唯一的网络配置工具。

- 命令：`netplan`

    ```shell
    netplan apply
    netplan try
    netplan status
    netplan get
    ```

- 配置文件：`/etc/netplan`

    ```yaml
    network:
      ethernets:
        eth0:
          dhcp4: true
          routes:
            - to: default
              via: <gateway>
      tunnels:
        wg0:
          mode: wireguard/vxlan/gre/...
          ...
    ```

### [iproute2](https://wiki.linuxfoundation.org/networking/iproute2)

iproute2 是 Linux 下的网络配置工具，它适用于硬件、路由表等基础层面的管理。

!!! warning "请勿继续使用下表中 net-tools 的命令，它已经被弃用。请熟悉使用 iproute2。"

    | net-tools | iproute2 | 说明 |
    | --- | --- | --- |
    | `ifconfig` | `ip address` | 显示和配置网络地址 |
    | `ifconfig` | `ip link` | 显示和配置网络设备 |
    | `route` | `ip route` | 显示和配置路由表 |
    | `arp` | `ip neighbour` | 显示和配置 ARP 表 |
    | `netstat` | `ss` | 显示网络连接、路由、接口等信息 |
    | `brctl` | `bridge` | 桥接工具 |
    | `iptunnel` | `ip tunnel` | 配置隧道 |

网络接口的命令主要是 ip link

```shell
ip link set dev <interface> up/down
ip link set dev eth0 mtu 9000
ip addr add/del <address>/<prefix> dev <interface>
ip route add/del <network>/<prefix> via <gateway> dev <interface>
```
