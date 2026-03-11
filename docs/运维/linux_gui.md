# Linux 图形界面

## 概述

<figure markdown="span">
    <center>
    ![](https://upload.wikimedia.org/wikipedia/commons/2/2d/The_Linux_Graphics_Stack_and_glamor.svg)
    </center>
    <figcaption>
    Linux DRI 架构
    <br /><small>
    [Direct Rendering Infrastructure - Wikipedia](https://en.wikipedia.org/wiki/Direct_Rendering_Infrastructure)
    </small>
    </figcaption>
</figure>

网络上有相当丰富的 Linux 图形栈资料，让我们来阅读一下。

!!! quote "(2014)[A brief introduction to the Linux graphics stack – Developer Log](https://blogs.igalia.com/itoral/2014/07/29/a-brief-introduction-to-the-linux-graphics-stack/)"

    Linux 图形栈的简单历史。

    - 最初，X Server 独占显卡的访问权限，应用使用 Xlib 通过 X11 协议向 X Server 发送绘制指令。对于 3D 游戏，OpenGL 通过 GLX 发送到 X Server，再进行翻译。
        - 优点：X Server 处理资源占用和同步问题，图形栈简单。
        - 缺点：X Server 逐渐成为瓶颈。
    - DRI 使得用户态应用程序能够直接访问显卡渲染，架构如下：
        - 内核中 Direct Rendering Manager（DRM）负责管理硬件，向用户态提供接口
        - 客户端通过 libdrm 使用这套 API
        - Mesa 基于 DRI 实现了 OpenGL API，提供 libGL
    
    这位作者后续的几篇博客系统地介绍了 Mesa，我们将在后文学习。

!!! quote "(2022)[An introduction to the Linux graphics stack](https://crosscat.me/an-introduction-to-the-linux-graphics-stack/)"

    窗口系统、Mesa、DRM 和 KMS 是如何协作的。

    - DRM 负责渲染，而显示模式设置（分辨率、帧率）由 Kernel Mode Setting（KMS）完成。

其他补充材料：

- [Direct Rendering Infrastructure - Wikipedia](https://en.wikipedia.org/wiki/Direct_Rendering_Infrastructure)：Wiki 中的几幅图很好地展示了 DRI 的整体结构。
- [:simple-bilibili: 一个像素的奇幻漂流](https://www.bilibili.com/video/BV1Ap4y1V7Wp)：来自 PCLT 实验室图形栈小组的技术分享，整体的内容框架值得学习。遗憾的是讲得感觉并不好，内容比较杂糅，没有主次。EP 中有手撕代码，挺有价值的。
- [Freedesktop](https://www.freedesktop.org/wiki/Software/)：该组织维护了 Linux 图形栈和桌面环境的大部分组件，例如 DRM、Mesa、X11。

## 内核图形系统

### DRM

### KMS

## 3D 图形库

!!! quote

    - [List of 3D graphics libraries - Wikipedia](<https://en.wikipedia.org/wiki/List_of_3D_graphics_libraries>)
    - [A Comparison of Modern Graphics APIs - Alain Galvan](https://alain.xyz/blog/comparison-of-modern-graphics-apis)
    - [The story of WebGPU: The successor to WebGL - Medium](https://eytanmanor.medium.com/the-story-of-webgpu-the-successor-to-webgl-bf5f74bc036a)
    - :simple-bilibili: Up 主 [Redknot-乔红](https://space.bilibili.com/38154792) 制作的系列图形 API 科普视频：
        - [为什么游戏总要编译着色器？](https://www.bilibili.com/video/BV1zi421h7tJ)：3D 图形接口的发展历史（主要为 OpenGL），着色器语言。
        - [SteamDeck 搭载 Linux，凭什么可以玩 Win 游戏？](https://www.bilibili.com/video/BV1VeHFeTEjo)：现代着色器语言 HLSL、GLSL，中间格式 SPIR-V，Wine 和 Proton 如何实现 Direct3D 的转换。

总体上，这些图形库的关系如下：

<figure markdown="span">
    <center>
    ![graphics_api_history](linux_gui.assets/graphics_api_history.png)
    </center>
    <figcaption>
    3D 图形库的发展历史
    <br /><small>
    [Building New 3D Web Games With Cocos Creator and WebGPU - COCOS](https://www.cocos.com/en/post/ODdxxWGryD6DiM6wPJ3yhPklSzCLCCxE)
    </small>
    </figcaption>
</figure>

### OpenGL 与 Vulkan

总体来说，Vulkan 的设计理念更新，跨平台兼容性更好，对硬件的控制更细致，性能更高，是未来的必然选择。但 OpenGL 仍然有其优势，比如更简单易用，对于一些简单的 3D 游戏或应用，OpenGL 仍然是一个不错的选择。

目前，入门 OpenGL 最好的书本应该是 [OpenGL Programming Guide: The Official Guide to Learning OpenGL, Version 4.5 with SPIR-V](https://archive.org/details/openglprogrammin0000kess)，其中文版为 [OpenGL 编程指南 (原书第 9 版)](https://book.douban.com/subject/27123094/)。如果要在 Windows 上进行开发，[Computer Graphics Programming in OpenGL with C++](https://terrorgum.com/tfox/books/computergraphicsprogrammminginopenglusingcplusplussecondedition.pdf) 提供了较为详细的 Windows 开发环境配置。

!!! info "硬件支持情况"

    Khronos 开发的所有 API 都有 Adopter Program：如果某公司实现了 Khronos 标准的 API，则必须通过 Khronos 的一致性测试，才能使用相关标准的名字和标志。

    - [OpenGL Conformant Products - Khronos](https://www.khronos.org/conformance/adopters/conformant-products/opengl)：从 OpenGL 4.4 开始，Khronos 启动了 Adopter Program。硬件制造厂商可以向 Khronos 提交 OpenGL 4.4 及更高版本的一致性测试。我们可以在其中看到的产品包括 2024 年的 Apple M2（OpenGL 4.6）到 2013 年的 GT 465（OpenGL 4.4）。
    - [OpenGL ES](https://www.khronos.org/conformance/adopters/conformant-products/opengles)
    - [Vulkan Conformant Products - Khronos](https://www.khronos.org/conformance/adopters/conformant-products/vulkan)

    此外，[gpuinfo.org](https://gpuinfo.org/) 是一个社区维护的 Khronos API 数据库。

### OpenGL 支持

!!! quote

    - [What are GLAD, GLFW and OpenGL?. What is OpenGL? - Medium](https://matt-pinch.medium.com/what-are-glad-glfw-and-opengl-569136024c87)

OpenGL 有许多变体和依赖，本节我们来梳理一下。

首先是 OpenGL 相关 API。OpenGL 标准开头就进行了梳理（见 1.3 Related APIs）：

- OpenGL 常见于桌面端，提供 Java、C、Python 绑定。
- OpenGL ES 在 OpenGL 的基础上进行了增删，为嵌入式系统定制。
- WebGL 基于 OpenGL ES，为浏览器定制，使用 JavaScript 调用。

在 OpenGL 规范之外，是具体的实现。

- GLFW（Graphics Library Framewor）为 OpenGL、OpenGL ES 和 Vulkan 提供桌面端的窗口管理、输入处理等功能。在 Linux 系统上，它支持 X11 和 Wayland。

### Mesa

Mesa 是 OpenGL 的 Linux 实现。

- [Diving into Mesa – Developer Log](https://blogs.igalia.com/itoral/2014/08/08/diving-into-mesa/)
- [Driver loading and querying in Mesa – Developer Log](https://blogs.igalia.com/itoral/2014/09/04/driver-loading-and-querying-in-mesa/)
- [An eagle eye view into the Mesa source tree – Developer Log](https://blogs.igalia.com/itoral/2014/09/08/an-eagle-eye-view-into-the-mesa-source-tree/)
- [Setting up a development environment for Mesa – Developer Log](https://blogs.igalia.com/itoral/2014/09/15/setting-up-a-development-environment-for-mesa/)
- [A brief overview of the 3D pipeline – Developer Log](https://blogs.igalia.com/itoral/2014/11/11/a-brief-overview-of-the-3d-pipeline/)
- [An introduction to Mesa’s GLSL compiler (I) – Developer Log](https://blogs.igalia.com/itoral/2015/03/03/an-introduction-to-mesas-glsl-compiler-i/)
- [An introduction to Mesa’s GLSL compiler (II) – Developer Log](https://blogs.igalia.com/itoral/2015/03/06/an-introduction-to-mesas-glsl-compiler-ii/)

## 窗口系统（Window System）

### X Window System

!!! quote

    - [magcius/xplain: Interactive demos](https://github.com/magcius/xplain)
- [Explanations - X Window System Basics](https://magcius.github.io/xplain/article/x-basics.html)：对 X Window System 的全面介绍。阅读完这篇文章，应该理解 X Window System、Xorg、X11、Xlib、xcb 等名词的含义和关系。

## 桌面环境（Desktop Environment）

### GNOME

### KDE





## 中文相关

### 输入法

#### 输入法框架

!!! quote

    - [Fcitx or Ibus on KDE, especially in Wayland : r/kde](https://www.reddit.com/r/kde/comments/x3wg7d/fcitx_or_ibus_on_kde_especially_in_wayland/)

输入法框架应当根据桌面环境选择。根据个人使用经验，在 KDE 和 GNOME 下应选择 fcitx5。

#### Rime 和 Rime-ice

!!! quote

    - [iDvel/rime-ice: Rime 配置：雾凇拼音 | 长期维护的简体词库](https://github.com/iDvel/rime-ice)

!!! TOOD

    配置 rime-ice

```shell
apt install fcitx5-rime
git clone https://github.com/iDvel/rime-ice.git Rime --depth 1
```

### 字形显示问题

!!! quote

    - [Localization/Simplified Chinese - ArchWiki](https://wiki.archlinux.org/title/Localization/Simplified_Chinese)
    - [Linux 下的字体调校指南 - Leo's Field](https://szclsya.me/zh-cn/posts/fonts/linux-config-guide/)
    - [[Bug]: Some flatpak apps can't choose right font for specific language when using ttc or otc fonts · Issue #4865 · flatpak/flatpak](https://github.com/flatpak/flatpak/issues/4865)

- 原因：默认字体按照字典序排列，`ja-JP` 在 `zh_...` 前，所以显示为日文字形。
- 解决办法：

    - 用户级别：

        创建 `~/.config/fontconfig/fonts.conf` 文件，指定默认字体族。

    - Flatpak：

        为特定应用添加环境变量 `LC_ALL=zh_CN.UTF-8`

## Flatpak

对于桌面端 GUI 应用，我尽可能选择 Flatpak。它优雅地分层解决了 GUI 应用对 DE、图形驱动程序的脆弱的依赖问题，同时提供了沙盒环境。可以通过 flatseal 控制 Flatpak 应用的环境和权限。

例外：

- Steam：应当使用发行版官方打包版本。Flatpak 的容器化会导致 Wine 出问题。

!!! TODO

    wayland QQ 剪贴板问题，暂时关闭 wayland 模式，等待上游 PR 合并。[Wayland clipboard sync (xvfb -> Wayland) by taoky · Pull Request #225 · flathub/com.qq.QQ](https://github.com/flathub/com.qq.QQ/pull/225)。

## 零散问题

- 切换是否启动图形界面：

    ```text
    systemctl set-default multi-user.target
    systemctl set-default graphical.target
    ```
