# systemd-networkd

## Netlink

https://stackoverflow.com/questions/18858190/whats-in-include-uapi-of-kernel-source-project

```c
#include <linux/if_bonding.h>
```

Interaction with Kernel via Netlink

src/network/networkd-setlink.c: Contains handlers like link_set_bond_handler to communicate with kernel netlink APIs for bond setup.
src/basic/include/linux/if_bonding.h: Includes kernel-level definitions for bonding, such as modes and states.

## 网络管理

Netdev and Network Management

src/network/networkd-network.h: Describes the Network structure, including bond-related configurations.
src/network/generator/network-generator.c: Contains functions like network_set_bond for applying bond configurations.

## bond 设备初始化过程

`src/network/netdev/bond.c` 中有：

- `bond_init()`：该函数注册为 `bond_vtable.init`，在 `netdev_load_one()` 中通过 `NETDEV_VTABLE(netdev)->init(netdev)` 调用。
- `netdev_bond_fill_message_create()`：从 `NetDev` 中获取属性，构造 Netlink 消息。该函数注册为 `bond_vtable.fill_message_create`，调用链如下：

  ```c
  NETDEV_VTABLE(netdev)->fill_message_create(netdev, link, m);
    netdev_create_message() src/network/netdev/netdev.c
      independent_netdev_create(NetDev *netdev)
        netdev_request_to_create(NetDev *netdev)
      stacked_netdev_create(NetDev *netdev, Link *link, Request *req)
        link_request_stacked_netdev(Link *link, NetDev *netdev)
  ```
