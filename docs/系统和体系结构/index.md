# 系统和体系结构

!!! quote

    - [总结 | 计算机体系结构基础](https://foxsen.github.io/archbase/%E6%80%BB%E7%BB%93.html)
    - [15-213/15-503 Introduction to Computer Systems](https://www.cs.cmu.edu/~213/index.html)

> 计算机体系结构（Computer Architecture）是描述**计算机各组成部分及其相互关系**的一组规则和方法，是程序员所看到的计算机属性。计算机体系结构主要研究内容包括：
>
> - **指令系统结构**（Instruction Set Architecture，ISA）
> - **计算机组织结构**（Computer Organization）
> - **微体系结构**（Micro-architecture）：处理器的组织结构
> - **并行体系结构**并行计算机的组织结构
>
> 冯诺依曼结构的存储程序和指令驱动执行原理是现代计算机体系结构的基础。

除了上述内容，计算机体系结构还有一些前置知识：

- **数字电路**：计算机体系结构的基础，它研究逻辑门、触发器等基本电路元件，用于实现二进制运算和存储。计算机体系结构则在此基础上设计处理器、内存等硬件组件，并定义指令集和系统架构，以构建完整的计算机系统。简言之，数字电路提供底层硬件支持，计算机体系结构则负责系统级设计与优化。

    - FPGA
    - 硬件描述语言：Verilog

> The ICS course provides a programmer's view of **how computer systems execute programs, store information, and communicate.** It enables students to become more effective programmers, especially in dealing with issues of performance, portability and robustness. It also serves as a foundation for courses on compilers, networks, operating systems, and computer architecture, where a deeper understanding of systems-level issues is required. Topics covered include:
>
> - machine-level code and its generation by optimizing compilers
> - performance evaluation and optimization
> - computer arithmetic
> - memory organization and management
> - networking technology and protocols
> - supporting concurrent computation

从著名的《深入理解计算机系统》一书对应的课程简介可以看到，计算机系统是硬件和系统软件的结合体，它构建在计算机体系结构之上，为应用程序提供完整的运行环境。计算机系统主要研究内容包括：

- **操作系统（Operating System）**
    - 进程管理与调度
    - 内存管理
    - 文件系统
    - 设备驱动
    - 系统安全
- **计算机系统基础**
    - 信息表示与处理
    - 程序的机器级表示
    - 处理器体系结构
    - 程序优化
    - 虚拟内存
- **分布式系统**
    - 分布式计算
    - 分布式存储
    - 分布式一致性
    - 容错机制
