# 代理技术

## 代理、网关和隧道

- 代理：具有转发功能的应用程序
    - 特点：不改变 URI，添加 Via 信息。
    - 作用：缓存、访问控制。
    - 正向代理：代理客户端进行请求，如 VPN。
    - 反向代理：代理服务器接受请求，隐藏真实服务器，如负载均衡、缓存代理。
    - 透明代理：转发请求或响应时不对报文进行修改。
- 网关：转发通信数据的服务器
    - 可以使用不同的通信协议，比如访问数据库。
- 隧道：建立通信线路确保安全
    - 使用加密手段进行通信，如 SSL。
    - 本身是透明的，不会对报文进行修改。

## 相关概念、协议

如果没有特别说明，这些协议都在 OSI Layer 5（会话层）之上。

??? note "SOCKS5"

    SOCKet Secure。

    ==todo==
    
    - SOCKS4：只支持 TCP。
    - SOCKS5：添加对 DNS 解析、UDP 和 IPv6 的支持
    
    !!! info "[RFC 1928 - SOCKS Protocol Version 5](https://datatracker.ietf.org/doc/html/rfc1928)"

??? note "SSL"

    Secure Sockets Layer，安全套接层。

    ==todo==

??? note "TLS"

    Transport Layer Security，安全传输层协议。

    ==todo==

根据分层模型，HTTP 代理只能代理应用层的 HTTP 协议，而 SOCKS 代理可以代理几乎所有 TCP 和 UDP 流量。

??? note "TUN/TAP 设备"

    > TUN/TAP provides packet reception and transmission for user space programs. It can be seen as a simple Point-to-Point or Ethernet device, which, instead of receiving packets from physical media, receives them from user space program and instead of sending packets via physical media writes them to the user space program.

    简单来说， TUN/TAP 设备是一个虚拟网络设备，可以在用户空间和内核空间之间传递数据包。TUN 处理 IP 数据包，TAP 处理以太网帧。

    !!! info "[Linux Kernel Documentation: TUN/TAP](https://www.kernel.org/doc/Documentation/networking/tuntap.txt)"

??? note "NAT"

    Network Address Translator，网络地址转换。

    ==todo==

??? note "ACL"

    Access Control List，访问控制列表。

    在交换机、路由器等网络设施上都可以使用 ACL 进行访问控制。在代理软件上，ACL 可以作为一种简单的分流措施，    Shadowsocks 就采用这种方式。

??? note "PAC"

    Proxy Auto-Config，代理自动配置。

    ==todo==

## HTTPS 代理

代理没有目标服务器的证书和私钥，因此不会做中间人角色。HTTPS 代理只做 TCP 上的数据转发，不过多了 HTTP CONNECT 方法，用于建立隧道。

- 客户端向代理发起 CONNECT 请求，要求代理与目标服务器建立 TCP 连接。
- 代理建立连接后，返回 200 OK 响应。
- 接下来客户端和服务器间开始建立 SSL 连接，代理只是简单地转发数据。

## 新型代理

HTTP、SOCKS5 等代理进行交互时都是明文的，容易被监测。下面的这些新型代理对客户端与代理服务器之间的通信进行保护，防止监测。

- [协议：Shadowsocks](https://shadowsocks.org/doc/what-is-shadowsocks.html)：仅对流量加密，特征明显，容易被识别。
- [客户端：ShadowsocksRShadowsocksR](https://github.com/breakwa11/shadowsocks-rss/)：加入流量混淆，增加识别难度。
- [客户端：V2Ray](https://github.com/v2fly/v2ray-core)：将加密流量放入 HTTPS，V2Ray 服务器与正常 HTTPS 服务器共用通道，增加识别难度。
    - 派生项目：[Xray](https://github.com/XTLS/Xray-core)。
    - [协议：VMess](https://www.v2ray.com/developer/protocols/vmess.html)
- [协议：Trojan](https://github.com/trojan-gfw/trojan)：使用 TLS 加密，伪装成 HTTPS 流量。

大部分代理软件都采用下图的工作方式：本地设置一个普通的代理服务器，由自身通过专用加密方式与远程代理服务器通信。

![img](proxy.assets/proxy_workflow)

## DNS 解析

!!! info "[Sukka's Blog: 浅谈在代理环境中的 DNS 解析行为](https://blog.skk.moe/post/what-happend-to-dns-in-proxy/)"

!!! note "Fake-IP"

    !!! info "[RFC 3089 - A SOCKS-based IPv6/IPv4 Gateway Mechanism](https://datatracker.ietf.org/doc/html/rfc3089)"
