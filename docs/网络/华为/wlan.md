# WLAN

!!! quote

    - [WLAN-FAT AP 介绍 - 华为](https://support.huawei.com/enterprise/zh/doc/EDOC1100033976/abe6c7b7)

无线局域网 WLAN（Wireless Local Area Network）广义上是指以无线电波、激光、红外线等来代替有线局域网中的部分或全部传输介质所构成的网络。本文介绍的 WLAN 技术是基于 802.11 标准系列的，即利用高频信号（例如 2.4GHz 或 5GHz）作为传输介质的无线局域网。

802.11 是 IEEE 在 1997 年为 WLAN 定义的一个无线网络通信的工业标准。此后这一标准又不断得到补充和完善，形成 802.11 的标准系列，例如 802.11、802.11a、802.11b、802.11e、802.11g、802.11i、802.11n、802.11ac 等。

除了 802.11（WLAN）外，无线局域网还涉及下列标准：

- **CAPWAP** 协议用来解决 AP 与 AC 之间的通信。
- **802.1X** 标准定义网络的用户接入认证机制。
- **OpenFlow** 协议用于控制 AC 的流表，实现对无线用户流量的精细化管理。

## WLAN 原理

### 基本概念

- **工作站 STA（Station）**：支持 802.11 标准的终端设备。
- **接入点 AP（Access Point）**：为 STA 提供基于 802.11 标准的无线接入服务，起到有线网络和无线网络的桥接作用。
- **虚拟接入点 VAP（Virtual Access Point）**：是 AP 设备上虚拟出来的业务功能实体。用户可以在一个 AP 上创建不同的 VAP 来为不同的用户群体提供无线接入服务。
- **服务集标识符 SSID（Service Set Identifier）**：表示无线网络的标识，用来区分不同的无线网络。例如，当我们在笔记本电脑上搜索可接入无线网络时，显示出来的网络名称就是 SSID。
    - **基本服务集标识符 BSSID（Basic Service Set Identifier）**，表示 AP 上每个 VAP 的数据链路层 MAC 地址。
    - **扩展服务集标识符 ESSID（Extended Service Set Identifier）**，是一个或一组无线网络的标识，图中所示的“guest”或“internal”。STA 可以先扫描所有网络，然后选择特定的 SSID 接入某个指定无线网络。通常，我们所指的 SSID 即为 ESSID。

    !!! note "简单理解，BSSID 是 MAC 地址，ESSID 是 WiFi 名称。"

- **基本服务集 BSS（Basic Service Set）**：一个 AP 所覆盖的范围。在一个 BSS 的服务区域内，STA 可以相互通信。
- **国家码**：用来标识射频所在的国家，它规定了射频特性，如功率、信道值和可用于帧传输的信道总数。在第一次配置设备之前，必须配置有效的国家码。

<figure markdown="span">
    <center>
    ![wlan_ssid](wlan.assets/wlan_ssid.png){ width=80% align=center}
    </center>
    <figcaption>
    WLAN 基本概念
    </figcaption>
</figure>

传统 ICT 厂商将 WLAN 架构分为两种：自治式架构（Fat AP）和集中式架构（Fit AP）。下文以华为为例，但概念基本通用。华为 AP 架构如下所示。

- 自治式网络架构：
    - 又称为胖接入点（FAT AP）架构。在该架构下，AP 实现所有无线接入功能，不需要 AC 设备。
    - WLAN 早期广泛采用自治式架构。FAT AP 的功能强大，独立性好，但是设备结构复杂，价格昂贵，难于管理。随着企业大量部署 AP，对 AP 的配置、升级软件等管理工作给用户带来很高的操作成本和维护成本，自治式架构应用逐步减少。
- 瘦接入点（FIT AP）架构
    - 在该架构下，通过 AC 集中管理和控制多个 AP，所有无线接入功能由 AP 和 AC 共同完成。
    - AC 集中处理所有的安全、控制和管理功能，例如移动管理、身份验证、VLAN 划分、射频资源管理和数据包转发等。
    - FIT AP 完成无线射频接入功能，例如无线信号发射与探测响应、数据加密解密、数据传输确认等。
    - AP 和 AC 之间采用 CAPWAP 协议进行通讯，AP 与 AC 间可以跨越二层网络或三层网络。

<figure markdown="span">
    <center>
    ![wlan_huawei_arch](wlan.assets/wlan_huawei_arch.png)
    </center>
    <figcaption>
    华为 AP 模式
    </figcaption>
</figure>

!!! note "简单理解：路由器自带的无线功能为 Fat AP，作为 AC 控制 AP 则为 Fit AP。"

### STA 接入过程（802.11）

STA 接入过程分为三个阶段：扫描阶段、链路认证阶段和关联阶段。

#### 扫描阶段

STA 可以通过主动扫描和被动扫描获取到周围的无线网络信息：

<div class="grid cards" markdown>

- **主动扫描：指定 SSID**

    ---

    ![wlan_access_active_1](wlan.assets/wlan_access_active_1.png)

    客户端发送携带有指定 SSID 的 Probe Request：STA 依次在每个信道发出 Probe Request 帧，寻找与 STA 有相同 SSID 的 AP，只有能够提供指定 SSID 无线服务的 AP 接收到该探测请求后才回复探查响应。

- **主动扫描：广播**

    ---

    ![wlan_access_active_2](wlan.assets/wlan_access_active_2.png)

    客户端发送广播 Probe Request：客户端会定期地在其支持的信道列表中，发送 Probe Request 帧扫描无线网络。当 AP 收到 Probe Request 帧后，会回应 Probe Response 帧通告可以提供的无线网络信息。

- **被动扫描**

    ---

    ![wlan_access_passive](wlan.assets/wlan_access_passive.png)

    STA 在每个信道上侦听 AP 定期发送的 Beacon 信标帧（信标帧中包含 SSID、支持速率等信息），以获取 AP 的相关信息。当用户需要节省电量时，可以使用被动扫描。

</div>

#### 链路认证阶段

为了保证无线链路的安全，接入过程中 AP 需要完成对 STA 的认证。802.11 链路定义了两种认证机制：开放系统认证和共享密钥认证。

开放系统认证 (Open System Authentication) 即不认证，任意 STA 都可以认证成功。

<figure markdown="span">
    <center>
    ![wlan_access_osa](wlan.assets/wlan_access_osa.png)
    </center>
    <figcaption>
    开放系统认证
    </figcaption>
</figure>

因为本文介绍的 WPA/WPA2 安全策略仅支持开放系统认证，故略去共享密钥认证的介绍。

#### 关联阶段

终端关联过程实质上是链路服务协商的过程。完成链路认证后，STA 会继续发起链路服务协商，具体的协商通过 Association 报文实现。

<figure markdown="span">
    <center>
    ![wlan_access_association](wlan.assets/wlan_access_association.png)
    </center>
    <figcaption>
    关联阶段
    </figcaption>
</figure>

1. STA 向 AP 发送 Association Request 请求，请求帧中会携带 STA 自身的各种参数以及根据服务配置选择的各种参数（主要包括支持的速率、支持的信道、支持的 QoS 的能力以及选择的接入认证和加密算法）。

2. AP 收到关联请求后判断是否需要进行用户的接入认证，并回应 Association Response。

    STA 收到 Association Response 后，判断是否需要进行用户的接入认证：

    - 如果不需要，STA 可以访问无线网络；
    - 如果需要，STA 继续发起用户接入认证请求，用户接入认证成功后，STA 才可以访问无线网络。

### WLAN 安全策略

!!! note

    对于个人/家庭场景，一般采用的安全策略为 WPA2-PSK 接入认证 + CCMP（AES）数据加密。对于企业场景，认证策略一般会升级为 WPA2-802.1X 接入认证。

WLAN 安全提供了 WEP、WPA、WPA2 和 WAPI 四种安全策略机制。每种安全策略体现了一整套安全机制，包括无线链路建立时的**链路认证方式**，无线用户上线时的用户**接入认证方式**和无线用户传输数据业务时的**数据加密方式**。

目前，应用最广泛的是 WPA/WPA2。它比 WEP 更安全，但比 WAPI 更简单。本文仅介绍 WPA/WPA2。WPA/WPA2 安全策略涉及了链路认证阶段、接入认证阶段、密钥协商和数据加密阶段。其中，链路认证阶段 WPA/WPA2 仅支持开放式系统认证。

#### 接入认证阶段

WPA/WPA2分为企业版和个人版：

- WPA/WPA2企业版：采用WPA/WPA2-802.1X的接入认证方式，使用RADIUS服务器和可扩展认证协议EAP（Extensible Authentication Protocol）进行认证。用户提供认证所需的凭证，如用户名和密码，通过特定的用户认证服务器（一般是 RADIUS 服务器）来实现对用户的接入认证。下图展示基于 EAP-PEAP 的 802.1X 认证流程图。

    <figure markdown="span">
        <center>
        ![wlan_wpa2_8021x](wlan.assets/wlan_wpa2_8021x.png)
        </center>
        <figcaption>
        WPA2-802.1X 接入认证
        </figcaption>
    </figure>

- WPA/WPA2 个人版：对一些中小型的企业网络或者家庭用户，部署一台专用的认证服务器代价过于昂贵，维护也很复杂，因此，WPA/WPA2 提供了一种简化的模式，即 WPA/WPA2 预共享密钥（WPA/WPA2-PSK）模式，它不需要专门的认证服务器，仅要求在每个 WLAN 节点（WLAN 服务端、无线路由器、网卡等）预先输入一个预共享密钥即可。只要密钥吻合，客户就可以获得 WLAN 的访问权。由于这个密钥仅仅用于认证过程，而不用于加密过程，因此不会导致诸如使用 WEP 密钥来进行 802.11 共享认证那样严重的安全问题。

!!! note "这里的所谓“预共享密钥”就是我们所说的 WiFi 密码。"

802.1X 认证可以支持对有线用户和无线用户进行身份认证，而 PSK 认证则是专门针对无线用户的认证方法。

#### 密钥协商阶段

在 802.11i 里定义了两种密钥层次模型，一种是成对密钥层次结构，主要用来保护 **STA 与 AP 之间往来的数据**；一种是群组密钥层次结构，主要用来描述 STA 与 AP 之间的**广播或组播数据**。

密钥协商阶段根据接入认证生成的成对主钥 PMK（Pairwise Master Key）产生成对临时密钥 PTK（Pairwise Transient Key）和群组临时密钥 GTK（Group Temporal Key）。PTK 用来加密单播报文，GTK 用来加密组播和广播无线报文。PMK 一般是利用预共享密钥和 SSID 通过哈希算法计算出来的。

密钥协商包括单播密钥协商和组播密钥协商过程，与密码学中常见的密钥协商过程类似，此处暂略。

#### 数据加密阶段

WPA/WPA2支持TKIP和CCMP两种加密算法。

- TKIP 加密算法

    区别于 WEP 共用一个共享密钥，TKIP 采用一套动态密钥协商和管理方法，每个无线用户都会动态地协商一套密钥，保证了每个用户使用独立的密钥。每个用户的密钥是由密钥协商阶段协商出来的 PTK、发送方的 MAC 地址和报文序列号计算生成的，通过这种密钥混合的防护方式来防范针对 WEP 的攻击。

    TKIP 采用信息完整性校验机制，一方面保证接收端接收报文的完整性；另一方面保证接收端和发送端数据的合法性。信息完整性校验码是通过密钥协商阶段协商出来的 MIC Key、目的 MAC 地址、源 MAC 地址和数据包计算生成的。

- CCMP 加密算法

    区别于 WEP 和 TKIP 采用的流密码机制，CCMP 采用了以高级加密标准 AES（Advanced Encryption Standard）的块密码为基础的安全协议。这种基于块密码的加密技术克服了 RC4 算法本身的缺陷，安全性更高。

## WLAN 实践

### 通用配置

配置：

```text
wlan global country-code CN # 缺省情况下，AP 设备的国家码标识为 CN。
```

检查：

```text
display wlan global
```

### Fat AP 原理

配置 AP 时，实际上指的是配置 VAP：

- VAP 是 AP 上的业务功能实体。用户可以在 AP 的射频上创建不同的 VAP，通过**为 AP 的射频绑定服务集和射频模板**，就可以创建 VAP。
- VAP 下发 AP 时，与 VAP 绑定的服务集参数作为 VAP 的参数一起下发到 AP，AP 根据 VAP 配置的参数提供给用户不同的业务。

配置过程中涉及到两种逻辑接口：

- WLAN-Radio：射频接口，进行射频的相关配置，包括天线增益、功率、信道、实际可用天线数等。
- WLAN-BSS：一种虚拟的二层接口，类似于 hybrid 类型的二层以太网接口，具有二层属性，并可配置多种二层协议。创建并配置 WLAN-BSS 接口后，需要在服务集下绑定该接口。

还涉及到四种模板：

- WMM 模板：
    - **产生背景**：802.11 协议对于所有的应用都提供了相同质量的服务，但是事实上，不同的应用对于无线网络的要求也是不同的，因此 802.11 协议已经不能满足实际应用的需要。
    - **WMM 协议**：为了能够为不同的应用提供不同质量的服务，Wi-Fi 组织提出了一种无线 QoS 协议 **Wi-Fi 多媒体标准 WMM（Wi-Fi Multimedia）**，将数据报文按照优先级从高到低分为 4 个接入类 AC（Access Category）：AC_VO(语音)、AC_VI(视频)、AC_BE(尽力而为)、AC_BK(背景)，高优先级的 AC 占用信道的机会大于低优先级的 AC。WMM 模板实现了 WMM 协议，通过创建 WMM 模板，使 AP 或客户端优先级高的报文优先占用无线信道，保证语音、视频在无线网络中有更好的质量。
- 射频模板：
    - **内容**：射频速率、射频信道模式、射频功率模式、丢包/错包率门限、冲突率门限、分段门限、RTS/CTS 门限、是否支持短前导码、DTIM 周期、beacon 帧周期、WMM 名称或 ID 等。如果将某个射频模板绑定到某个射频中，则该射频就继承在射频模板里配置的所有参数。
    - **只有绑定了 WMM 模板的射频模板才可以被射频绑定。**
- 安全模板：
    - **内容**：安全模板提供了 WEP、WPA/WPA2、WAPI 四种安全策略机制。每种安全策略实现了一整套安全机制，包括无线链路建立时的链路认证方式，无线用户上线时的用户接入认证方式和无线用户传输数据业务时的数据加密方式。
    - **必要性**：WLAN 技术使用无线射频信号作为业务数据的传输介质，这种开放式的信道使攻击者很容易对无线信道中传输的业务数据进行窃听或篡改。因此，安全性是 WLAN 最为重要的因素。通过安全模板，您可以选择一种安全策略，更好的保护用户敏感数据的安全和用户的隐私。创建安全模板时，如果不配置任何安全策略，则模板内默认指定了安全策略为 open-system，即用户在搜索到无线网络时，不需要认证，可以直接访问网络。使用默认的安全策略存在较大的安全隐患，**非法用户可能通过无线方式 telnet 登录到设备进行业务修改配置**。可通过配置更安全的安全策略来降低此类安全隐患，例如配置 WPA/WPA2 的安全策略。
- 流量模板：
    - **内容**：创建流量模板可以实现为某个无线网络定制特定的优先级映射和流量监管功能。
    - **优先级映射**：如果 STA 和 AP 都使能了 WMM 功能，则 STA 发送的报文中携带有优先级字段。为了保证端到端的 QoS，保持报文在整个传输过程中的优先级，需要配置在转换过程中不同报文的优先级字段的映射关系。
    - **流量监管**：为了保护有限的网络资源，需要对无线用户进入 WLAN 网络的流量进行监管，即限制无线终端发送报文的速率。

整体架构如下图所示：

<figure markdown="span">
    <center>
    ![wlan_fat_arch](wlan.assets/wlan_fat_arch.png)
    </center>
    <figcaption>
    Fat AP 组成
    </figcaption>
</figure>

### Fit AP 原理

WLAN 早期广泛采用自治式架构。FAT AP 的功能强大，独立性好，但是设备结构复杂，价格昂贵，难于管理。随着企业大量部署 AP，对 AP 的配置、升级软件等管理工作给用户带来很高的操作成本和维护成本，自治式架构应用逐步减少。

目前暂未配置 Fit AP，故不展开介绍。

### Fat AP 配置

缺省情况下，设备存在 WMM 模板、射频模板、安全模板和流量模板，用户可以直接使用，不需要创建新模板。具体如下：

- 缺省 WMM 模板：ID 为 0，名称为 wmmf；
- 缺省射频模板：ID 为 0，名称为 radiof，缺省射频模板默认已绑定缺省 WMM 模板；
- 缺省安全模板：ID 为 0，名称为 secf；
- 缺省流量模板：ID 为 0，名称为 traf；
- 创建服务集后，服务集默认绑定了缺省的流量模板和安全模板。

如下所示：

```text
[Huawei-wlan-view]dis this
[V200R010C10SPC700]
#
wlan
 wmm-profile name wmmf id 0
 traffic-profile name traf id 0
 security-profile name secf id 0
 radio-profile name radiof id 0
  wmm-profile id 0
#
return
```

实际配置时，一般只需要新建安全模板，不需要新建 WMM 和流量射频模板。

```text
[Huawei-wlan-view]security-profile name wlantest
[Huawei-wlan-sec-prof-wlantest]security-policy wpa2
[Huawei-wlan-sec-prof-wlantest]wpa2 authentication-method psk pass-phrase cipher ******** encryption-method ccmp
```

!!! note "关于双频"

    一个 WLAN-BSS 接口只能被一个服务集绑定，一个服务集又只能被一个 WLAN-Radio 绑定。因此，如果需要在一个 AP 上配置双频 WLAN，则 WLAN-BSS 和服务集都需要分别创建。

    但对于不同服务集，SSID 可以相同，从而实现“双频合一”。下文就展示了这种配置。

接下来配置 WLAN-BSS 接口：

```text
# 2.4GHz
[Huawei]interface wlan-bss 0
[Huawei-Wlan-Bss0]port hybrid tagged vlan 1
# 5GHz
[Huawei]interface wlan-bss 1
[Huawei-Wlan-Bss1]port hybrid tagged vlan 1
```

接下来将安全模板和 WLAN-BSS 接口绑定到服务集上。：

```text
# 2.4GHz
[Huawei-wlan-view]service-set name wlantest
[Huawei-wlan-service-set-wlantest]ssid wlantest
[Huawei-wlan-service-set-wlantest]security-profile name wlantest
[Huawei-wlan-service-set-wlantest]wlan-bss 0
# 5GHz
[Huawei-wlan-view]service-set name wlantest-5G
[Huawei-wlan-service-set-wlantest-5G]ssid wlantest
[Huawei-wlan-service-set-wlantest-5G]security-profile name wlantest
[Huawei-wlan-service-set-wlantest-5G]wlan-bss 1
```

最后，进入射频接口绑定射频模板和服务集，就完成了 VAP 的创建。射频接口 `0/0/0` 为 2.4GHz，`0/0/1` 为 5GHz。

```text
# 2.4GHz
[Huawei]interface Wlan-Radio 0/0/0
[Huawei-Wlan-Radio0/0/0]radio-profile id 0
[Huawei-Wlan-Radio0/0/0]service-set name wlantest
# 5GHz
[Huawei]interface Wlan-Radio 0/0/1
[Huawei-Wlan-Radio0/0/1]radio-profile id 0
[Huawei-Wlan-Radio0/0/1]service-set name wlantest-5G
```

配置完成后，我们就能使用 [CellularZ](http://www.cellularz.fun/) 等网络工具看到工作在两个频段的 WLAN 了：

| 名称 | MAC | 强度 | 频率 | 信道 | 带宽 |
| --- | --- | --- | --- | --- | --- |
| wlantest | 7c:d9:a0:00:00:00 | -27 | 2412 | 1 | 20 |
| wlantest | 7c:d9:a0:00:00:00 | -43 | 5825 | 165 | 20 |

检查：

```text
[Huawei-wlan-view]display this
[V200R010C10SPC700]
#
wlan
 wmm-profile name wmmf id 0
 traffic-profile name traf id 0
 security-profile name secf id 0
 security-profile name wlantest id 1
  security-policy wpa2
  wpa2 authentication-method psk pass-phrase cipher ******** encryption-method ccmp
 service-set name wlantest id 0
  Wlan-Bss 0
  ssid wlantest
  traffic-profile id 0
  security-profile id 1
 service-set name wlantest-5G id 1
  Wlan-Bss 1
  ssid wlantest
  traffic-profile id 0
  security-profile id 1
 radio-profile name radiof id 0
  wmm-profile id 0
#
return
[Huawei-Wlan-Radio0/0/0]display this
[V200R010C10SPC700]
#
interface Wlan-Radio0/0/0
 radio-profile id 0
 service-set id 0 wlan 1
#
return
```

### Fit AP 配置

暂略。

## 802.1X 原理

!!! quote

    - [802.1X 认证基础 - 华为](https://support.huawei.com/enterprise/zh/doc/EDOC1100086515)
    - [锐捷网络：校园网基于 802.1x 无感知认证_天极网](https://net.yesky.com/enterprise/79/93751579.shtml)
    - [802.1x 认证 | 干货解读 - 带你读懂无线 802.1x 认证 - 锐捷网络](https://www.ruijie.com.cn/jszl/88849/)

802.1X 协议是一种基于端口的网络接入控制协议（Port based networkaccess control protocol）。“基于端口的网络接入控制”是指在局域网接入设备的端口这一级验证用户身份并控制其访问权限。802.1X 协议为二层协议，不需要到达三层。认证报文和数据报文通过逻辑接口分离。

### 802.1X 认证过程

802.1X 系统为典型的 Client/Server 结构，包括三个实体：客户端、接入设备和认证服务器。

- 客户端一般为一个用户终端设备，用户可以通过启动客户端软件发起 802.1X 认证。客户端必须支持局域网上的可扩展认证协议 EAPoL（Extensible Authentication Protocol over LANs）。
- 接入设备通常为支持 802.1X 协议的网络设备，它为客户端提供接入局域网的端口，充当客户端和认证服务器之间的中介，从客户端请求身份信息，并与认证服务器验证该信息。根据客户端的身份验证状态控制其对网络的访问权限。
- 认证服务器用于实现对用户进行认证、授权和计费，通常为 RADIUS 服务器。

802.1X 认证系统使用可扩展认证协议 EAP（Extensible Authentication Protocol）来实现客户端、设备端和认证服务器之间的信息交互。EAP 协议可以运行在各种底层，包括数据链路层和上层协议（如 UDP、TCP 等），而不需要 IP 地址。因此使用 EAP 协议的 802.1X 认证具有良好的灵活性。

- 在客户端与设备端之间，EAP 协议报文使用 EAPoL（EAP over LANs）封装格式，直接承载于 LAN 环境中。
- 在设备端与认证服务器之间，用户可以根据客户端支持情况和网络安全要求来决定采用的认证方式。
    - **EAP 终结方式**中，EAP 报文在设备端终结并重新封装到 RADIUS 报文中，利用标准 RADIUS 协议完成认证、授权和计费。
    - **EAP 中继方式**中，EAP 报文被直接封装到 RADIUS 报文中（EAP over RADIUS，简称为 EAPoR），以便穿越复杂的网络到达认证服务器。

EAP 报文类型：Request、Response、Success、Failure 四种。
EAPoL、EAPoR 分别将 EAP 报文封装在 LAN 和 RADIUS 报文中。

802.1X 认证有以下触发方式：

- 客户端发送 EAPoL-Start 报文触发认证。
- 客户端发送 DHCP/ARP/DHCPv6/ND 或任意报文触发认证。
- 设备发送 EAP-Request/Identity 报文触发认证。

### EAP-PEAP-MSCHAPv2

EAP-TLS

## CAPWAP 原理

!!! quote

    - [无线 CAPWAP 隧道技术——理论篇 - 锐捷](https://www.ruijie.com.cn/jszl/86721/)

CAPWAP 是 Control And Provisioning of Wireless Access Points Protocol Specification 的缩写，意为无线接入点的控制和配置协议，是无线局域网内最重要的技术之一。

CAPWAP 协议用于 AC 对其所关联的 AP 的集中管理和控制，为 AP 和 AC 之间的互通性提供了一个通用封装和传输机制。CAPWAP 协议主要具备以下几个功能：

- AP 对 AC 的自动发现；
- AP 和 AC 的状态机运行和维护；
- AC 对 AP 进行管理、业务配置下发；
- STA 数据封装 CAPWAP 隧道进行转发。

### DTLS 协议

简而言之，DTLS 是基于 UDP 场景下数据包可能丢失或重新排序的现实情况下，为 UDP 定制和改进的 TLS 协议。在 [RFC 5415 - Control And Provisioning of Wireless Access Points (CAPWAP) Protocol Specification](https://datatracker.ietf.org/doc/html/rfc5415#section-2.4) 中强调了 DTLS 与 CAPWAP 之间的紧密关系：

> DTLS is used as a tightly integrated, secure wrapper for the CAPWAP protocol.  In this document, DTLS and CAPWAP are discussed as nominally distinct entities; however, they are very closely coupled, and may even be implemented inseparably.

### CAPWAP 协议报文

CAPWAP 协议有两种类型的报文：CAPWAP 控制报文和数据报文。控制报文主要携带的是信息要素，用于 AC 对于 AP 工作参数的配置和 CAPWAP 隧道的维护；数据报文主要携带终端发送的数据报文，用于传输终端的上层数据。控制报文和数据报文分别传输在不同的 UDP 端口，控制报文使用端口 5246，数据报文使用端口 5247。

## CAPWAP 实践

以我校玉泉 32 舍的无线网络为例，分析 CAPWAP 的实际应用。该区域部署了 H3C 的无线终结者方案，AP 由 WTU430-EI（以下简称 WTU）承担。

### H3C 无线终结者

!!! quote

    - [H3C 无线终结者部署手册 -6W103](https://www.h3c.com/cn/d_202208/1677294_30005_0.htm)

- 产生原因：密集房间场景中，信号衰减会导致 AP 信号覆盖的距离受限，同时多个房间共享一个 AP，性能也会遇到瓶颈。
- 华三的方案：采用 AC+WT+WTU 三层组网架构，解决信号覆盖和性能瓶颈的问题，满足大容量、高密度、高信号强度、低成本部署的用户需求，实现每房间的无线千兆接入速率。

![wtu](https://resource.h3c.com/cn/202208/31/20220831_7732991_x_Img_x_png_0_1677294_30005_0.png)

!!! note "没怎么看懂，这哪里创新了，感觉就只是弄了个弱版 AP。"

根据华三产品文档，WTU430-EI 只能工作在 Version 1 模式下：

![version1](https://resource.h3c.com/cn/202208/31/20220831_7733006_x_Img_x_png_9_1677294_30005_0.png)

从配置手册来看，集中转发和本地转发两种模式的差别不大。事实上这两种模式分别对应 RFC 5415 中的 Split MAC、Local MAC 两种 CAPWAP 架构。通过抓包我们看到仅有 STA 的 Association Request/Response 被封装在 CAPWAP 隧道中，而 STA 接入后的报文出现在 VLAN 中，因此可以推测部署采用的是本地转发模式（即 Local MAC 架构）。

VLAN 中的部分数据包是用了 DTLS，但并不是全部，且通信双方 IP 不都在内网，大概只是某些应用在使用？

注意文末 FAQ 中关于 PoE 的说明：

> - WT 能作为普通交换机 PoE 使用吗？
>
>     不能。WT 采用非标准 PoE（24V）给 WTU 供电，WT 直连 WTU 的端口也不能作为普通的网口使用，不能直接接 PC 或直连交换机，只能用来连接 WTU。
>
> - WTU 能直接连接普通 PoE 交换机使用吗？
>
>     不能。WT 采用非标准 PoE（24V）给 WTU 供电，普通 PoE 交换机无法给 WTU 供电，WT 和 WTU 只能配合在一起使用；另外，对于工作在超瘦模式的 WTU，其部分功能也需要 WT 来完成。

### WTU 上线过程

受电后，首先发送几个 LLDP 帧：

```text
System Description = H3C Comware Platform Software, Software Version 7.1.064, Release 2457P31\r\nH3C WTU430-EI\r\nCopyright (c) 2004-2022 New H3C Technologies Co., Ltd. All rights reserved.
    0000 110. .... .... = TLV Type: System Description (6)
    .... ...0 1010 0100 = TLV Length: 164
    System Description: H3C Comware Platform Software, Software Version 7.1.064, Release 2457P31\r\nH3C WTU430-EI\r\nCopyright (c) 2004-2022 New H3C Technologies Co., Ltd. All rights reserved.
```

从不带 VLAN 的控制网络通过 DHCP 获取 IP 地址。

```text
Dynamic Host Configuration Protocol (ACK)
    Message type: Boot Reply (2)
    Hardware type: Ethernet (0x01)
    Hardware address length: 6
    Hops: 0
    Transaction ID: 0xc2f2f8fb
    Seconds elapsed: 0
    Bootp flags: 0x8000, Broadcast flag (Broadcast)
        1... .... .... .... = Broadcast flag: Broadcast
        .000 0000 0000 0000 = Reserved flags: 0x0000
    Client IP address: 0.0.0.0
    Your (client) IP address: 10.181.70.***
    Next server IP address: 0.0.0.0
    Relay agent IP address: 0.0.0.0
    Client MAC address: NewH3CTechno_**:**:** (44:1a:fa:**:**:**)
    Client hardware address padding: 00000000000000000000
    Server host name not given
    Boot file name not given
    Magic cookie: DHCP
    Option: (53) DHCP Message Type (ACK)
        Length: 1
        DHCP: ACK (5)
    Option: (1) Subnet Mask (255.255.254.0)
        Length: 4
        Subnet Mask: 255.255.254.0
    Option: (3) Router
        Length: 4
        Router: 10.181.70.254
    Option: (51) IP Address Lease Time
        Length: 4
        IP Address Lease Time: 1 day (86400)
    Option: (59) Rebinding Time Value
        Length: 4
        Rebinding Time Value: 21 hours (75600)
    Option: (58) Renewal Time Value
        Length: 4
        Renewal Time Value: 12 hours (43200)
    Option: (54) DHCP Server Identifier (10.181.70.254)
        Length: 4
        DHCP Server Identifier: 10.181.70.254
    Option: (43) Vendor-Specific Information
        Length: 17
        Value: 800f0000030ab500220ab500230abbd702
    Option: (255) End
        Option End: 255
    Padding: 00
```

其回复报文中包含 Vendor Specific Option 43。搜索可以发现，华为、华三和思科等都会采用 Option 43 作为 AP 寻找 AC 的方式。

!!! info

    - H3C: [DHCP Option 43 Vendor-Specific Configuration Examples-5W100](https://www.h3c.com/en/Support/Resource_Center/EN/Home/Public/00-Public/Technical_Documents/Configure___Deploy/Configuration_Examples/H3C_CE-10948/#:~:text=If%20a%20DHCP%20client%20requests,can%20associate%20with%20the%20AC.)
    - Huawei: [How to Configure Option 43 When Huawei APs Are Connected to DHCP Servers of Different Vendors](https://support.huawei.com/enterprise/en/doc/EDOC1100202779)
    - Cisco: [Configure DHCP OPTION 43 for Lightweight Access Points](https://www.cisco.com/c/en/us/support/docs/wireless-mobility/wireless-lan-wlan/97066-dhcp-option-43-00.html)

华三支持 Option 43 配置为 PXE 或 ACS 格式。阅读文档示例，我们可以发现这是 PXE 模式：

> To configure a PXE server address sub-option that contains IPv4 AC addresses 10.23.200.1 and 10.23.200.2, use the following settings:
>
> - Sub-option type: `80`.
> - Sub-option length: `0b`. The length of `0000020a17c8010a17c802` is 11 bytes.
> - Sub-option value: `0000020a17c8010a17c802`. `0000` is a fixed part, `02` is the number of IPv4 AC addresses, `0a17c8010a17c802` is AC addresses `10.23.200.1` and `10.23.200.2` in hexadecimal format.
>
> When you configure the PXE server address sub-option in the DHCP address pool view of the DHCP server, the configuration is option 43 hex `800b0000020a17c8010a17c802`.

我们可以解析出三个 AC 的地址分别为：`10.181.0.34`、`10.181.0.35`、`10.187.215.2`。

可以看到，AC 与 AP 处于不同的网段，这意味着 AC 很有可能控制多个网段的 AP。

在完成 DHCP 地址 ARP 后，向所有 AC 发送 CAPWAP-Control - Discovery Request 报文。

## OpenFlow 原理

!!! quote

    - [OpenFlow 是什么？OpenFlow 和 SDN 之间是什么关系？ - 华为](https://info.support.huawei.com/info-finder/encyclopedia/zh/OpenFlow.html)
    - [「OpenFlow」协议入门 - Chentingz](https://chentingz.github.io/2019/12/30/%E3%80%8COpenFlow%E3%80%8D%E5%8D%8F%E8%AE%AE%E5%85%A5%E9%97%A8/)
    - [OpenFlow Switch Specification Version 1.5.1](https://opennetworking.org/wp-content/uploads/2014/10/openflow-switch-v1.5.1.pdf)

斯坦福大学的 Clean Slate 项目首先提出了 OpenFlow，然后提出了 SDN。因此 OpenFlow 也是 SDN 架构的第一个南向接口标准。

传统网络中，路由器在三层基于 IP 地址转发，交换机在二层基于 MAC 地址转发。而在 SDN 网络（以 OpenFlow 为例）中，网络设备的转发行为由控制器下发的流表决定，网络设备根据流表项对数据包进行匹配和处理，从而实现灵活的网络管理和控制。

在 OpenFlow 网络中，网络设备（交换机、路由器等）被称为数据平面设备（Data Plane Device），负责数据包的转发和处理；而控制器（Controller）则负责网络的管理和控制，向数据平面设备下发流表项和策略。

## OpenFlow 实践

- TCP 连接 6633 端口，发起了两个连接（可能是一个 SSID 一个）
- Server 侧发送 OFPT_HELLO，客户端回复
- FEATURE_REQUEST/FEATURE_REPLY
- ECHO_REQUEST/ECHO_REPLY
- 上报 PORT_STATUS
- 下发 MULTIPART_REQUEST 和 OFPMP_TABLE_FEATURES

当 STA 完成链路认证、关联阶段后，STA 继续发起用户接入认证请求。此时，因为 AP 上没有关于该 STA 的流表项，AP 将包含 EAP 报文的无线帧封装在 OpenFlow 的 PACKET IN 报文中发送给 AC 进行处理。

## 杂项

进展：

- 已观测到 EAP 报文（包含身份信息）出现在 OpenFlow 发往 AC 的 PACKET IN 报文中。这意味着认证过程中无流表，报文需送控制器处理。
- EAP 过程使用 TLS 加密，似乎是属于 EAP-PEAP-MSCHAPv2。需要研究怎么解密。
- 在开放无线网 1610 VLAN 中，有多个不同网段的 DHCP 服务器，这点很奇怪。
