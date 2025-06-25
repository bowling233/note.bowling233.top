# OpenWRT

## Immortal OpenWRT

[Project ImmortalWrt](https://github.com/immortalwrt) 是 OpenWRT 的中国分支，对国内开发者社群热门的开发板、软件包等提供了更好的支持。

笔者在使用时注意到其与 OpenWRT 官方有以下不同：

- 固件支持：一些开发板的 `kmod` 在 OpenWRT 源中没有，只有 ImmortalWRT 有。对于 `wireguard` 等软件包会造成影响。
- 软件包：OpenWRT 官方源中的软件包显著更少。

## 根目录扩容

??? quote

    - [OpenWrt 存储空间扩容的两种方案 - 喵斯基部落](https://www.moewah.com/archives/4719.html)
    - [OpenWrt 安装后扩容（非 overlay） - 听海](https://seahi.me/263.html)

OpenWRT 镜像刷写到硬盘后只占很小一部分，需要进行扩容并迁移系统，以完整利用硬盘的存储空间。

基本流程：

- 新建分区
- LuCI 上生成挂载点配置，重新设置根目录
- 根据给出的指令迁移根目录

## 软件包

```text
opkg update/upgrade/install/remove
opkg search/info
opkg list-upgradable | cut -f 1 -d ' ' | xargs -r opkg upgrade
/etc/opkg/distfeeds.conf
```

零散软件：

| 软件包名称 | 项目地址 | 说明 |
| ---------- | -------- | - |
| `luci-theme-argon` | [jerrykuku/luci-theme-argon: Argon is a clean and tidy OpenWrt LuCI theme that allows users to customize their login interface with images or videos. It also supports automatic and manual switching between light and dark modes.](https://github.com/jerrykuku/luci-theme-argon) | 不在 Opkg 仓库，按 README 安装 |
| `luci-app-lldpd` | | 方便地查看 LLDP 信息 |





### 开发工具

[[OpenWrt Wiki] Building OpenWrt ON OpenWrt](https://openwrt.org/docs/guide-developer/toolchain/building_openwrt_on_openwrt)

```text
opkg install pkg-config make gcc diffutils autoconf automake check git git-http patch libtool-bin \
grep rsync tar python3 getopt procps-ng-ps gawk sed xz unzip gzip bzip2 flock wget-ssl \
perl perlbase-findbin perlbase-pod perlbase-storable perlbase-feature perlbase-b perlbase-ipc perlbase-module perlbase-extutils perlbase-time perlbase-json-pp python3 \
coreutils-nohup coreutils-install coreutils-sort coreutils-ls coreutils-realpath coreutils-stat coreutils-nproc coreutils-od coreutils-mkdir coreutils-date coreutils-comm coreutils-printf coreutils-ln coreutils-cp coreutils-split coreutils-csplit coreutils-cksum coreutils-expr coreutils-tr coreutils-test coreutils-uniq coreutils-join \
libncurses-dev zlib-dev musl-fts libzstd \
joe joe-extras bash htop whereis less file findutils findutils-locate chattr lsattr xxd
```

## 构建

[[OpenWrt Wiki] Building a single package](https://openwrt.org/docs/guide-developer/toolchain/single.package)

## UCI

统一配置界面（Unified Configuration Interface，UCI）是 OpenWRT 用于统一管理

### 配置比较、备份和还原

[useful one-liner: diff-overlay : r/openwrt](https://www.reddit.com/r/openwrt/comments/j4hyfu/useful_oneliner_diffoverlay/)

## 网络配置

### 链路聚合

### DHCP 和 DNS

??? quote

    - [package: dnsmasq-full - OpenWRT](https://openwrt.org/packages/pkgdata/dnsmasq-full)
    - [package: dnsmasq - OpenWRT](https://openwrt.org/packages/pkgdata/dnsmasq)
    - [Dnsmasq Official Website](https://thekelleys.org.uk/dnsmasq/doc.html)
    - [Human-Readable DHCP Options for DNSMASQ · Kuan-Yi Li's Blog](https://blog.abysm.org/2020/06/human-readable-dhcp-options-for-dnsmasq/)

OpenWRT 自带 dnsmasq，但缺少一些功能（如 DHCPv6），一般将其替换为 dnsmasq-full。

先考虑 DHCP 的设置：

- 在静态地址的 interface 中可以进行配置。
- 重点是配置 DHCP Options。`dnsmasq --help dhcp` 列出 Options 的 human-readable 名称，可以直接使用。注意使用 human-readable 名称是需要加前缀 `option:`。

然后考虑 DNS 的配置：

- 几个关键选项：filter

### 监控与统计

#### collectd

适合收集长时间的数据。

```text
luci-app-statistics
collectd
collectd-mod-cpu
collectd-mod-curl
collectd-mod-dhcpleases
collectd-mod-disk
collectd-mod-dns
collectd-mod-interface
collectd-mod-load
collectd-mod-memory
collectd-mod-ping
collectd-mod-uptime
```

#### 带宽

!!! quote

    - [[OpenWrt Wiki] Bandwidth Monitoring Guide](https://openwrt.org/docs/guide-user/services/network_monitoring/bwmon)

```text
luci-app-nlbwmon
```
