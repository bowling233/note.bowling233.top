# The Debian Administrator's Handbook

读本书的原因：本书由 Debian 的开发者编写，遵守和 Debian 自身一样的社区流程，内容质量有保障。想要了解 Debian 的运作机制，这本书是最好的选择。

## 前言

- Debian 11 的新变化：
    - 支持 UEFI，更容易在开启 Secure Boot 的设备上安装。
    - AppArmor 默认启用。

## 第一章：Debian 项目

- 一些开源文化的关系：
    - GNU 项目由 FSF 赞助和维护。Debian 是 GNU 项目。

> - Debian 的体积庞大，现在所指的 Debian 应视为 meta-distribution，在此之上构建了 sub-project，如 Debian Science、Debian Edu 等。

- Debian 对自由软件质量有很高的要求：
    - 如果关键软件包有 bug 未被修复，Debian 将不惜推迟发行时间。有 bug 的可选包会被移除。
    - Debian 开发者没有日程表，不需要赶 deadline，由此保证软件质量。
    - 确保软件质量的团队是 quality assurance (QA)，他们维护 [https://tracker.debian.org/](https://tracker.debian.org/)。

> - Debian 曾经尝试移除 non-free 部分，但失败了，此后 non-free 不再被尝试移除，因为包含很多有用的东西。

- 满足下面条件的软件被认为足够 free（满足的许可证包括 GPL、BSD），可以被 Debian 包含。non-free 不是 Debian 官方的一部分。
    - Free redistribution
    - Source code
    - Derived works
    - Integrity of the author's source code
    - No discrimination against persons or groups
    - No discrimination against fields of endeavor
    - Distribution of license
    - License must not be specific to a product
    - License must not restrict other software
    - Example licenses

> - copyleft 是用 copyright（版权）造的词（left 与 right 相对），意指在分发作品的原始或修改版本时，必须保留所有最初的自由。copyleft 许可证的例子有 GPL 等。
> - sid 名称来源于某个动画电影中的角色，它经常损坏玩具；此外 sid 也是 Still In Development 的缩写。

- Debian 包维护者通常将 Debian 特定的文档放置在 `/usr/share/doc/<package>/README.Debian`，在配置前最好阅读一下。`example` 文件夹下也有一些示例配置文件。
- 提交更改：

    使用 `diff` 和 `patch`：

    ```shell
    diff -u file.old file.new > file.patch
    patch -p0 file.old < file.patch
    ```

    对应到 `git` 的命令为 `git diff` 和 `git apply`。

    使用邮件列表时，更经常使用 `git format-patch` 和 `git am`。

    Debian 托管于 GitLab，服务器为 [salsa.debian.org](https://salsa.debian.org)。可以使用 pull requests 提交更改。

- 安装 `how-can-i-help` 后，每次调用 APT 时都会显示本地安装的软件包是否有需要帮助的问题，这为 newcomer 提供了贡献的机会。
- Debian System Administrators (DSA) 是 Debian 的运维团队，负责 GitLab 实例、CI 平台等基础设施的运维。

本章最重要的是 1.6 节：发行版的生命周期。

- Experimental：独立的发行版，经常包含对基础软件包的重要修改，可能产生严重影响，不能集成到 unstable 版本。其中软件包永远不会迁移到其他版本。
- Unstable：一般软件包从这里开始。
    - 维护者创建源码包放置到 `ftp-master.debian.org`。
    - ftpmaster 检查和验证 Debian Policy 等。
    - 自动构建平台自动为所有支持的架构编译软件包。
    - 软件包在 Unstable 上可用。
    - 用户反馈 bug。

    ![unstable](https://debian-handbook.info/browse/stable/images/autobuilder.png)

- Migrate to  Testing：
    - 每天，一个程序自动按可量化的判准选择一组 Unstable 软件包包括到 Testing。判准包括：
        - 所有支持的架构上成功编译。
        - 比当前在 Testing 中的 bug 数少。
        - 在 Unstable 中等待至少 5 天。
        - Testing 中依赖可用。
        - 自动质量检测。
    - 然而，这是一个 NP 完全问题（图分团问题），使用启发式算法和一些人工辅助完成。

    ![testing](https://debian-handbook.info/browse/stable/images/release-cycle.png)

- Promotion from Testing to Stable
    - 如果 Testing 中的包有可以改进的地方，维护者需要从 Unstable 重新开始。
    - Release Manager 预先公告 freeze period，期间阻止新版本，只接受 bug 修复。
    - Release Manager 选择时间，将 Testing 中的包拷贝到 Simple。
    - 大版本有修订，如 `10.1` 等，这些更新只会包含安全补丁和重要的修正。

    ![stable](https://debian-handbook.info/browse/stable/images/package-lifecycle.png)

- Oldstable and Oldoldstable
    - 稳定版的生命周期为 5 年，新版本约每 2 年发布一次，因此同时最多有 3 个支持版本，称为 Stable、Oldstable 和 Oldoldstable。

## 第二、三章：案例分析

本书是基于一个特定情景的，本章简单描述了这一情景。读者作为一家小型公司的系统管理员，需要从头开始设计和构建公司的信息系统。第三章描述了如何迁移到 Debian，并与 Windows、Mac OS X 等系统共存。

一些迁移时重要信息和工具：

- `/etc/debian_version` 是 `base-files` 包的一部分。
- `/usr/local` 一般是手动编译安装的软件。
- `cruft-ng` 列出不属于任何软件包的文件。
- `debsums` 检查软件包持有文件的校验码（MD5）。
- `apt-show-versions` 可以检查版本，并找到没有来源的包（手动获取下载的）。

## 第四章：Debian 的安装

!!! todo

## 第五章：包管理系统

### 二进制包结构

- Debian 包设计为能够在任何 UNIX 系统上使用 `tar` 等工具解压。
    - 即使 `dpkg` 损坏或不可用，也能手动安装软件包。

    ```shell
    ar x dpkg_1.20.9_amd64.deb
    tar -C / -p -xJf data.tar.xz
    ```

    - 如果发生了更深层的损坏，如关键库文件被删除，可以使用静态的 `busybox-static`，它非常 self-contained。
- Debian 包的结构：
    - `control.tar.xz`：软件包的元数据，如名称、版本和依赖关系，还有安装前后的脚本。

        ```shell
        apt-cache show apt
        ```

        一些重要字段：

        ```text
        Pre-Depends Depends Recommends Suggests Enhances
        Conflicts Breaks 
        Provides
        ```

        `Recommends` 会被默认安装。`Pre-Depends` 要求在 pre-installation 脚本执行前安装完成。

        `apt` 和 `dpkg` 在面对冲突时的处理方式不同，APT 倾向于提出解决方案，而 dpkg 倾向于拒绝。

    - `data.tar.xz/bz2/gz`：软件包的实际文件。
    - `debian-binary`：版本号。
- 虚包（virtual package）并不真实存在。它有两个作用：
    - 关联到一个通用的服务。例如：`mail-transport-agent` 可以由 `exim4` 或 `postfix` 提供。
    - 完全取代一个软件包。
- 元包（meta-package）是一个实际的包（有 `.deb` 文件），但只是用来**表达依赖关系**。

### 源码包结构
