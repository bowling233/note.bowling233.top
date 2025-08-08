# 发行版与包管理

作为运维，常常需要和不同的 Linux 发行版打交道。在我看来，发行版之间的不同主要体现在软件包管理和 init 系统。可以说，包管理器代表着发行版在软件工程上的选择，背后的打包策略、软件源管理、依赖解决等等都是发行版的特色，即所谓“哲学”。

关于包管理器，Arch Wiki 上有一篇很好的对比总结：[pacman/Rosetta - ArchWiki](https://wiki.archlinux.org/title/Pacman/Rosetta)。其中对比了各个包管理器的命令，是在发行版中快速迁移的重要参考，故本文不会再对比介绍各包管理器的命令、配置等。

关于使用包管理器参与开发的各个流程，如 [构建](../../语言和软件开发/工具/build.md) 和 [调试](../../语言和软件开发/工具/debug.md)，请参考相应文档。

## 个人偏好

- 我个人最经常使用 Debian 系发行版，熟悉 APT 的管理模式。
- 对于桌面端 GUI 应用，我尽可能选择 Flatpak。它优雅地分层解决了 GUI 应用对 DE、图形驱动程序的脆弱的依赖问题，同时提供了沙盒环境。可以通过 flatseal 控制 Flatpak 应用的环境和权限。

## 通用软件包查询：Repology

近期我遇到了这样一个需求：构建尽可能一致的多发行版集群，以便于测试和开发。阻碍我实现这个目标的主要是软件包的不一致性。我需要一个工具，能够查询不同发行版的软件包信息。

有几个网站提供各发行版软件包的查询服务，例如：

- [Packages for Linux and Unix - pkgs.org](https://pkgs.org/)：API 需要付费，网页查询需要频繁验证。
- [Repology](https://repology.org/)：网页查询无需验证，API 使用每秒 1 次且经常 Forbidden。但提供每日离线数据下载。

在大多数场景下，Repology 提供的离线数据查询可能是最好的解决方法。发行版在一个生命周期内，软件包的版本变化不会太大，每月更新数据都足以应对。

## 发行版软件包维护

软件包维护是 Linux 发行版的核心工作之一。

目前我正在准备成为 Debian 维护者，这里记录一些相关的信息。

### 不同发行版的差异

- Service 相关：
    - [`postinstall.sh` should detect chrooted environments to prevent dpkg failures · Issue #767 · open-telemetry/opentelemetry-collector-releases](https://github.com/open-telemetry/opentelemetry-collector-releases/issues/767)

        > While deb packages in general enable and start services after installation, rpm packages don't do this and leave service enable and start to the sysadmin.

## Bootstrap

### Debian/Ubuntu

### CentOS/Fedora

### TencentOS/tlinux/OpenCloudOS

!!! quote

    - [tencentos](https://mirrors.cloud.tencent.com/help/tencentos.html)

腾讯基于 CentOS 开发

### OpenAnolis

阿里基于 CentOS 开发

## 完整性校验

包管理器肩负一个重要的职责，就是系统软件包的完整性校验。

输出格式为 9 个字符的字符串，可能包含属性标记符：

- `c` %config 配置文件
- `d` %doc 文档文件
- `g` %ghost 文件（即文件内容不包含在软件包载荷中）
- `l` %license 许可证文件
- `r` %readme 说明文件

后面跟随文件名。9 个字符中的每一个都表示文件属性与数据库中记录值的比较结果。单个 "."（句点）表示测试通过，单个 "?"（问号）表示无法执行测试（例如文件权限阻止读取）。其他字符表示相应 --verify 测试失败：

| 字符 | 含义 |
|------|------|
| `S` | 文件大小不同 |
| `M` | 模式不同（包括权限和文件类型） |
| `5` | 摘要（原 MD5 校验和）不同 |
| `D` | 设备主/次设备号不匹配 |
| `L` | readLink(2) 路径不匹配 |
| `U` | 用户所有权不同 |
| `G` | 组所有权不同 |
| `T` | 修改时间不同 |
| `P` | 能力（capabilities）不同 |

```bash
$ for p in $(rpm -q -a); do ret=$(sudo rpm -V $p); if [[ $ret != "" ]]; then printf "$p\n$ret\n"; fi; done
kernel-tlinux4-core-5.4.241-1.0017.4.tl3.x86_64
....L....    /boot/symvers-5.4.241-1-tlinux4-0017.4.gz # 文件链接改动
.M.......  g /lib/modules/5.4.241-1-tlinux4-0017.4/modules.alias # 文件权限改动
S.5....T.  c /etc/security/limits.conf # 文件内容改动
........P    /usr/sbin/mtr-packetkernel-tlinux4-5.4.119-1.0006.tl2.x86_64 # 文件 cap 改动
.......T.    /lib/modules/5.4.119-1-tlinux4-0006/modules.alias # 文件时间改动
$ for p in $(dpkg -l | grep ^ii | awk '{print $2}'); do ret=$(sudo dpkg -V $p); if [[ $ret != "" ]]; then printf "$p\n$ret\n"; fi; done
????????? c /etc/sudoers
????????? c /etc/sudoers.d/README
missing     /var/lib/polkit-1/localauthority (Permission denied)
```
