# RISC-V Assembly Language Programming: Unlock the Power of the RISC-V Instruction Set

## 第一章：准备开始

下面的汇编程序使用 Linux 系统调用输出字符串：

```assembly title="hello.S"
.global _start
_start:
    addi    a0, zero, 1
    la      a1, hello
    addi    a2, zero, 12
    addi    a7, zero, 64
    ecall
    addi    a0, zero, 0
    addi    a7, zero, 93
    ecall

.data
hello:
    .ascii "Hello RISC-V World!\n"
```

构建命令：

```shell
as -o hello.o hello.S
ld -o hello hello.o
```

运行一个 QEMU RISC-V 虚拟机：

- 去 [Ubuntu](https://ubuntu.com/download/risc-v) 下载预安装好的 QEMU RISC-V 虚拟机镜像
- 按照 [Ubuntu Wiki](https://wiki.ubuntu.com/RISC-V/QEMU) 的说明运行虚拟机

## 第二章：Load 与 Add

GNU Assembler 可以做一些东西：

- 自动立即数，比如会自动把 `add` 转成 `addi`：

    ```assembly
    add a0, a1, 1
    ```

- 如果提供十六进制值，应当符号扩展到 64 位，否则给警告：

    ```assembly
    addi x6, x0, 0xFFFFFFFFFFFFF90B # x6 = 0x90B
    ```

- 加载长立即数时需要注意的事情：
    - 分段加载，使用 `lui`、`slli` 和 `add` 三种指令。
    - `imm` 符号扩展，因此如果分段加载时有一个立即数值为负，那么它前面的常数必须 +1。例如：某一段为 `0x90A`，它是负数，则其前面一段应当由 `0x678` 改为 `0x679`。

## 第三章：使用工具

`make` 的一些语法：

```makefile
%.o: %.S
    as -o $@ $<
```

- `$@` 代表输出文件
- `$<` 代表源文件
- `%` 通配符

`gdb` 的一些命令：

- `list` 列出 10 行代码。
- `list <start>,<end>` 列出指定行范围的代码。
- `disassemble <func>` 反汇编函数。
- `info registers` 查看寄存器，简写 `i r`。
- `x /Nuf addr` 查看内存
    - `N` 为数量。
    - `f` 为格式。
        - `t` 为二进制。
        - `x` 为十六进制。
        - `d` 为十进制。
        - `i` 为指令。
        - `s` 为字符串。
    - `u` 为单位。
        - `b` 为字节。
        - `h` 为半字。
        - `w` 为字。
        - `g` 为双字。
    - `addr` 为地址。
    - 例子：`x /4ubfx _start`、`x /4ubfi _start`。

## 第四章：控制程序流

- Imm 部分可以放一些表达式：

    ```assembly
        addi    a0, zero, ('A' - 10)
    ```

- `objdump` 的一些参数：

    ```shell
    objdump -d -M no-aliases,numeric hello
    ```

## 第五章：感谢内存

```assembly
.byte 74, 0112, 0b101, 0x4A, 'J'
.short # 2byte
.word 0x12345678
.quad # 8byte
.octa # 16byte
.quad 0x1234567890ABCDEF
.ascii "Hello, World!\n"
.asciz
.float
.double
.fill repeat, size, value
.fill 10, 4, 0x12345678
.align 4 # 将下一片数据对齐到 4 字节边界
```

- 整数前可以放两种前缀：`-` 取二进制补码，`~` 取反码。
- 对齐数据通常更快，因为大多数内存总线只加载对齐的数据，而不对齐的数据需要两次加载。

循环：重复指令块：

```assembly
.rept count
#...
.endr

.rept 3
    .byte 0, 1, 2
.endr
```

- 汇编程序的段（section）：
    - 默认段是 `.text`，存放代码，不需要显式指明。其中存放的数据是只读的。若处理器支持内存保护，则写时发生运行时错误。
    - `.data` 存放数据，可读可写。注意立即数为 12 位，所以数据应当在需要的指令的 2048 字节范围内。为什么是 2048 而不是 4096？因为标准中定义立即数**总是符号扩展**。
    - **RISC-V 使用小端法，也就是逆序。**比如 1 平常写为 `0x00000001`，这是大端法，而在 RISC-V 中是 `0x01000000`。小端法的一个优点是更容易修改整数的大小。
- 将数据加载到寄存器
    - `la` 常常使用 PC 相对寻址，借助 `auipc` 和 `addi` 指令。GNU Assembler 有 `%pcrel_hi` 和 `%pcrel_lo` 内置函数，可以方便地计算相关的立即数值：

        ```assembly
        label:
            auipc  x5, %pcrel_hi(msg)
            addi   x5, x5, %pcrel_lo(label)
        .data
        msg:
            .asciz "Hello, World!."
        ```

        这两对指令必须相邻成对出现，否则报错。`%pcrel_hi` 接受所需数据的标签，`%pcrel_lo` 接受代码的标签，这样做是因为后续需要重定位。具体工作原理可参考 [What do `%pcrel_hi` and `%pcrel_lo` actually do? - Stack Overflow](https://stackoverflow.com/questions/65879012/what-do-pcrel-hi-and-pcrel-lo-actually-do)。

- `l` 命令的多种类型：

    ```text
    b bu h hu w wu d
    ```

- 结合使用（间接寻址）：

    ```assembly
    label:
        auipc x6, %pcrel_hi(mynumber)
        lw   x6, %pcrel_lo(label)(x6)
        /* 相当于：
        addi x6, x6, %pcrel_lo(label)
        lw   x6, 0(x6)
        */
    .data
        mynumber:
            .word 0x12345678
    ```

此处书中提到了 Address Relaxation 的优化措施，但没有详细说明，有一篇文章对此做了讲解：

!!! note "[Linker Relaxation in the RISC-V Toolchain](https://www.sifive.com/blog/all-aboard-part-3-linker-relaxation-in-riscv-toolchain)"

    Linker Relaxation 是一种在链接时（而不是编译时）优化程序的技术，它非常重要，甚至影响了 RISC-V 的设计。简单来说：

    - 两种无条件跳转 `jalr` **基址寻址**和 `jal` PC 相对寻址。使用 `auipc` + `jalr` 可以跳转 32 位有符号偏移量，而 `jal` 可以跳转 21 位有符号偏移量。
    - 编译器不知道两个标签之间的距离，所以必须生成 `auipc` + `jalr`，这样可以跳转长距离。但显然，两条指令比一条指令慢。
    - 于是我们让编译器生成重定位 relaxation 指令，让链接器在链接时优化这些指令。

    该优化对于函数跳转、访问全局变量等都有很大帮助。文章中具体讲解了很多细节，值得研究。

    GNU Assembler 可以通过 `-mno-relax` 关闭这个优化。链接器创建符号 `$__global_pointer$`，指向全局数据段的第 2048 字节，这样相比全局数据段的第 0 个字节，能够扩大单条指令的跳转范围。GNU Assembler 假设 `x3`(`gp`) 指向该地址。

- 使用 Relaxation 后：

    ```assembly
    _start:
    .option push # push 和 pop 用于暂存汇编器选项，这里用来暂存 norelax 选项。
    .option norelax # 如果不加，gp 会被优化掉
    1:  auipc gp, %pcrel_hi(_global_pointer$)
        addi  gp, gp, %pcrel_lo(1b)
    .option pop
    # ...
    1:  auipc x5, %pcrel_hi(msg) # 转化为 auipc  x5, gp, (__global_pointer$ - msg)
        addi  x5, x5, %pcrel_lo(1b)
    .data
    .msg .asciz "Hello, World!"
    ```

    `1b` 表示符号 `1` 上一次出现的地方。

## 第六章：函数与栈

栈是计算机科学中的一种数据结构，提供两个功能：

- 函数调用间保存寄存器状态
- 在函数运行期间提供内存

RISC-V ABIs Specification 规定了栈的工作原理。

- Linux 中，在控制转交到 `_start` 前，栈会被设置好。
- RISC-V 标准规定 `sp` 总是 16 字节对齐，这意味着只能按 16 的倍数加减。否则会导致**总线错误**。
- 约定 `sp` 指向栈中的最后一个元素，向下增长（内存递减方向）。
- 在栈上分配空间（即调整栈指针 sp 的值）**需要在其他操作之前执行**。原因是，当发生中断（interrupt）时，处理器会暂停当前程序的执行，转而执行中断处理程序。中断处理程序可能会使用栈来保存和恢复数据。如果在进入中断处理程序之前没有正确调整栈指针，可能会导致重要的数据（例如局部变量、返回地址等）被中断处理程序意外覆盖，从而引发程序错误。

跳转有省略写法：

```assembly
jal offset # 相当于 jal x0, offset
jalr rs # 相当于 jalr ra, 0(rs)
```

最常见的函数结构：

```assembly
    jal myfunc
myfunc:
    addi sp, sp, -16
    sd  ra, 8(sp)

    ld ra, 8(sp)
    addi sp, sp, 16
    ret
```

- 栈帧：
    - 函数运行过程中可能需要分配更多内存，使用 `sp` 会导致栈指针混乱。
    - 在函数的开始处将 `fp` 设置为 `sp` 值，这样就不用担心未来对 `sp` 的更改。通常在该操作后，开始本地变量的初始化。

- `.EQU` 指令，仅用于定义数字：

    ```assembly
    .EQU    stacksize, 16
    ```

- 强大的宏：`.MACRO macroname parameter1, parameter2, ...`，宏内部用 `\parameter` 引用参数。

    ```assembly title="uppermacro.S"
    .MACRO toupper instr, outstr
        la a0, \instr
        la a1, \outstr
        mv t4, a1
        # ...
        sub a0, a1, t4
    .ENDM
    ```

    ```assembly title="main.S"
    .include "uppermacro.S"
    .global _start
    _start:
        toupper tststr, buffer
        #...
    .data
    tststr:
        .asciz "Hello, World!"
    buffer:
        .fill 256, 1, 0
    ```

    - 与函数调用相比，宏产生更大的代码，但是性能更好。

- 汇编器允许数字标签被重复定义，并使用 `f` 和 `b` 后缀分别表示下一个（forward）和上一个（backward）标签。

->p140

## 伪指令

- `li rd,imm` load immediate 加载立即数
- `la rd,label` load address 加载地址
- 跳转相关
    - `j label` 跳转到标签
    - `jr rs1` = `jalr x0, 0(rs1)` 跳转到寄存器地址
    - `ret` = `jalr x0, 0(ra)` 返回
    - `call label` 使用 `auipc` 和 `jalr` 调用函数
- 条件跳转（补全了各种情况）：

    ```riscv
    bgt ble bgtu bleu
    ```

- 与 0 有关的条件跳转：

    ```riscv
    beqz rs1, label
    beqz benz blez bgez bltz bgtz
    ```

- 与 0 比较：

    ```riscv
    seqz rd, rs
    seqz snez sltz sgtz
    ```
