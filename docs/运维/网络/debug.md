# 调试

## [lldpd](https://lldpd.github.io/)

在系统上安装并启用 `lldpd` 服务后，即可收发 LLDP 帧。

```shell
sudo apt install lldpd
sudo systemctl enable lldpd
sudo systemctl start lldpd
```

`lldpd` 守护进程默认在后台向所有接口收发 LLDP 帧。可以使用 `lldpcli` 命令与守护进程交互。这一交互界面类似于 Cisco 设备的 CLI，你可以使用 `?` 查看帮助。

```shell
$ lldpcli
[lldpcli] # show neighbors
-------------------------------------------------------------------------------
LLDP neighbors:
-------------------------------------------------------------------------------
Interface:    fwpr109p0, via: LLDP, RID: 1, Time: 0 day, 00:00:27
  Chassis:
    ChassisID:    mac e8:61:1f:3e:fb:1b
    SysName:      ***
    SysDescr:     Debian GNU/Linux 12 (bookworm) Linux 6.8.8-2-pve #1 SMP PREEMPT_DYNAMIC PMX 6.8.8-2 (2024-06-24T09:00Z) x86_64
    MgmtIP:       **
    MgmtIface:    2
    MgmtIP:       ***
    MgmtIface:    2
    Capability:   Bridge, on
    Capability:   Router, on
    Capability:   Wlan, off
    Capability:   Station, off
  Port:
    PortID:       mac e2:a4:5e:7e:c0:d0
    PortDescr:    fwln109i0
    TTL:          120
-------------------------------------------------------------------------------
```

## [tcpdump](https://www.tcpdump.org/)

!!! quote

    - [](https://danielmiessler.com/blog/tcpdump)

```shell
tcpdump -i <interface> -w <file>
```

表达式：

```text
host 192.168.1.100
port 80
src host 192.168.1.100 and \( port 80 or port 443 \)
tcp
udp
icmp
net 192.168.1.0/24
port 67 or port 68
```

## [iperf3](https://iperf.fr/)

测带宽

## [iptraf-ng](https://github.com/iptraf-ng/iptraf-ng)

实时流量监控
