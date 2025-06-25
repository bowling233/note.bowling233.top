---
tags:
    -   不完善
---
# ISA

## 概述

指令集架构从总体上可以分为精简指令集（RISC）和复杂指令集（CISC）。ARM、MIPS、RISC-V 属于 RISC，x86 属于 CISC。

闭源的指令集有 x86、ARM。开源的指令集有 MIPS、RISC-V。设计 ARM 处理器需要购买授权。

## RISC-V 32

!!! abstract "内容来源"

    -   课程 计算机组成
    -   书籍 计算机组成与设计：硬件/软件接口（RISC-V 版）

- 32 个 64 位寄存器 `x0-x31`，其中 `x0` 恒为 0。
- $2^{61}$ 字节虚拟地址空间，`Memory[0]-Memory[2^61-1]`。
- 32 位称为 word，64 位称为 doubleword。
- 回忆 R、I、S、U 指令的格式。
- 以下是一些常用指令：

    ```text
    add, sub, 
    ld, lb, lbu
    sd
    slli, srli, srai
    sll, srl, sra
    and, or, xor
    beq, bne, blt, bge, bltu, bgeu
    lr.d, sc.d
    mul
    mulh, mulhsu, mulhu
    ```

- 如何实现锁（原子化操作）？

??? note "RISC-V 指令集助记卡"

    <embed src="../attachment/riscv指令集助记卡.pdf" style="width:100%; height:80vh;" type="application/pdf">

    <embed src="../attachment/riscv编程助记卡.pdf" style="width:100%; height:80vh;" type="application/pdf">

??? note "RISC-V 指令集标准"

    <embed src="../attachment/riscv标准.pdf" style="width:100%; height:80vh;" type="application/pdf">

## x86-64

!!! abstract "内容来源"

    -   书籍 深入理解计算机系统

- 两种格式：
    - ATT 格式：GCC、OBJDUMP 等软件使用。
    - Intel 格式：Intel 和微软的文档使用。
    - 对于多操作数指令，两种格式列出操作数顺序相反。
    - `gcc -S -masm=intel` 可以输出 Intel 格式。

!!! tip "笔记中采用 ATT 格式"

- 16 个 64 位寄存器 `rax-r15`。
    - 历史：
        - 8086：8 个 16 位。
        - IA32：扩展到 32 位。
        - x86-64：64 位。新增 8 个。
- 几种寻址模式：`Imm(Rb, Ri, S)`、`$Imm`。

??? note "x86-64 指令集助记卡"

    <embed src="../attachment/x86-64指令集助记卡.pdf" style="width:100%; height:80vh;" type="application/pdf">

## MIPS

!!! abstract "内容来源"

    -   书籍 计算机组成与设计：硬件/软件接口

??? note "MIPS 指令集助记卡"

    <embed src="../attachment/MIPS指令集助记卡.pdf" style="width:100%; height:80vh;" type="application/pdf">
