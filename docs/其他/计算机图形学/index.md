# 计算机图形学

## Linux 图形栈：一个像素的奇幻漂流

!!! abstract

    在介绍 OpenGL 前，先了解一下 Linux 图形栈的组成或许更好。这样能够更好地理解 OpenGL 所处的位置及其作用。

!!! quote

    - [:simple-bilibili: 一个像素的奇幻漂流](https://www.bilibili.com/video/BV1Ap4y1V7Wp)：来自 PCLT 实验室图形栈小组的技术分享，整体的内容框架值得学习。遗憾的是讲得感觉并不好，内容比较杂糅，没有主次。EP 中有手撕代码，挺有价值的。
    - [A brief introduction to the Linux graphics stack – Developer Log](https://blogs.igalia.com/itoral/2014/07/29/a-brief-introduction-to-the-linux-graphics-stack/)：Linux 图形栈的简单历史。通过这篇文章可以了解到 Linux 图形栈如何由最早的通过 X server 的间接渲染，演化到使用 DRI 的直接渲染，再产生 Mesa 3D 项目，最终形成现代的 Linux 图形栈。
    - [An introduction to the Linux graphics stack › FLUSP - FLOSS at USP](https://flusp.ime.usp.br/blogs,/kernel-graphics/an_introduction_to_the_linux_graphics_stack/)：对现代 Linux 图形栈的全局概览。介绍了窗口系统、Mesa、DRM 和 KMS 是如何协作的。
    - [Linux graphic stack · Nathan Gauër | Blog](https://studiopixl.com/2017-05-13/linux-graphic-stack-an-overview)：对上文的一些补充。
    - [Explanations - X Window System Basics](https://magcius.github.io/xplain/article/x-basics.html)：对 X Window System 的全面介绍。阅读完这篇文章，应该理解 X Window System、Xorg、X11、Xlib、xcb 等名词的含义和关系。

## 底层 3D 图形库概述

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
    ![graphics_api_history](library.assets/graphics_api_history.png)
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
