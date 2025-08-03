# GNU Compiler Collection

- `cc1`：源码 -> 汇编

## GCC

- [Invoking GCC (Using the GNU Compiler Collection (GCC))](https://gcc.gnu.org/onlinedocs/gcc/Invoking-GCC.html)

    - 具有开启/关闭的选项：`-f` 或 `-W` 开头，格式 `-foo`/`-fno-foo`。

### [spec](https://gcc.gnu.org/onlinedocs/gcc/Spec-Files.html)

!!! quote

    - [linking errors for stitch project (recompile with -fPIC)](https://groups.google.com/g/hugin-ptx/c/cbPR2c-6Ocw)

除了命令行选项，还有一个隐蔽的东西会影响 GCC 及其组件的选项，那就是 spec。使用 `gcc -v` 时可以看到：

```bash
$ gcc -v
Using built-in specs.
Reading specs from /usr/lib/rpm/redhat/redhat-hardened-cc1
```

GCC 定制者（比如发行版）可以通过该文件强制 GCC 使用某些选项。比如，redhat 就强制要求 PIE：

```text title="redhat-hardened-cc1"
*cc1_options:
+ %{!r:%{!fpie:%{!fPIE:%{!fpic:%{!fPIC:%{!fno-pic:-fPIE}}}}}}
```

该 spec 可能在红帽系和

### PIE/PIC

| 选项 | 主要目的 | 主要区别与特性 | 宏定义 | 使用场景/注意 |
|------|----------|----------------|--------|---------------|
| `-fpic` | 生成适用于**共享库（.so）**的位置无关代码 (PIC) | 1) 存在特定架构的 GOT (全局偏移表) 大小限制 (如 SPARC 8k，AArch64 28k，m68k/RS/6000 32k，x86 无限制)<br>2) 目标架构支持有限<br>3) 运行时依赖动态链接器解析 GOT | `__pic__ = 1`<br>`__PIC__ = 1` | 当目标二进制文件 GOT 大小小于目标平台限制时可用。否则考虑 `-fPIC` |
| `-fPIC` | 生成适用于**共享库（.so）**的位置无关代码 (PIC) | 1) 无特定架构的 GOT 大小限制<br>2) 目标架构支持有限<br>3) 运行时依赖动态链接器解析 GOT<br>4) 比 `-fpic` 更通用、更安全，尤其是在 GOT 可能较大的情况 | `__pic__ = 2`<br>`__PIC__ = 2` | 构建动态链接库的标准选择。比 `-fpic` 优先使用以规避 GOT 限制风险 |
| `-fpie` | 生成仅能链接到可执行文件（而非库）的 PIC 代码 | 1) 行为类似 `-fpic`（可能有 GOT 限制）<br>2) 代码只能用于构建可执行文件（PIE），不能放入共享库<br>3) 运行时依赖动态链接器解析 GOT | `__pie__ = 1`<br>`__PIE__ = 1` | 编译打算链接成 `-pie` 或 `-static-pie` 可执行文件的源码。需与对应的链接器选项 (`-pie` 或 `-static-pie`) 同时使用 |
| `-fPIE` | 生成仅能链接到可执行文件（而非库）的 PIC 代码 | 1) 行为类似 `-fPIC`（无 GOT 限制）<br>2) 代码只能用于构建可执行文件（PIE），不能放入共享库<br>3) 运行时依赖动态链接器解析 GOT<br>4) 通常比 `-fpie` 更优 | `__pie__ = 2`<br>`__PIE__ = 2` | 同 `-fpie`，但当 GOT 可能较大时更安全（首选）。需与对应的链接器选项 (`-pie` 或 `-static-pie`) 同时使用 |
| `-pie` | 链接器选项：生成动态链接的位置无关可执行文件 (PIE) | 1) 输出的可执行文件可以在内存任意地址加载执行<br>2) 依赖动态链接器加载共享库和完成运行时链接<br>3) 要求编译对象使用 `-fpie` 或 `-fPIE` 选项编译 | 无 | 创建更安全的、兼容 ASLR（地址空间布局随机化）的动态链接可执行程序 |
| `-no-pie`| 链接器选项：不生成 PIE 可执行文件 | 1) 生成传统的、固定 (或基址可重定位) 加载地址的可执行文件<br>2) 不强制要求代码是 PIC，但编译选项仍需一致以防问题 | 无 | 默认行为或不兼容 PIE 的旧系统。禁用 PIE 生成 |
| `-static-pie` | 链接器选项：生成静态链接的位置无关可执行文件 | 1) 输出的可执行文件可以在内存任意地址加载执行<br>2) 完全不依赖动态链接器，所有库静态链接进文件<br>3) 要求编译对象使用 `-fpie` 或 `-fPIE` 选项编译 | 无 | 创建更安全的、兼容 ASLR、不依赖系统动态库、可独立分发的可执行文件（比纯 `-static` 增加了位置无关性） |

- Code Generation:
    - **`-fpic`/`-fPIC` 用于共享库**: 目标是 `.so` 文件。`-fPIC` 更通用，避免小 GOT 限制的风险。
    - **`-fpie`/`-fPIE` 用于可执行文件中的代码**：目标是最终被链接成 `-pie` 或 `-static-pie` 生成的可执行文件（PIE 或 static PIE），其代码不能放入共享库。
- Link:
    - **`-pie` 用于生成动态链接的 PIE 可执行文件**: 运行时需要动态链接器和目标系统的共享库。
    - **`-static-pie` 用于生成静态链接的 PIE 可执行文件**: 运行时完全独立，不需要动态链接器或目标系统的共享库，自身即可在任何地址运行。这是 `-static`（静态链接无位置无关）和 `-pie`（位置无关但需动态库）特性的结合。
- **宏定义**：`__pic__`/`__PIC__` 用于 PIC；`__pie__`/`__PIE__` 用于 PIE。
- **编译与链接选项配合**：使用 `-pie` 或 `-static-pie` 链接时，必须使用对应的 `-fpie` 或 `-fPIE`（或等效选项）编译所有源码对象。`-fpic`/`-fPIC` 编译的代码只能用于链接到共享库，不能用于 `-pie` 或 `-static-pie` 生成的可执行文件。

这一坨选项呢，彼此之有千丝万缕的联系。如果此时遇到上面针对单个选项的 spec，就会遇到问题。基于 RedHat 上的 GCC 12，我们做下面的实验：:

| gcc 参数 | gcc collect | cc1 collect |
| - | - | - |
| `-fno-PIE -fno-pic` | `-fno-pic` | `-fno-pic` |
| `-fno-PIE -fno-pic -fno-PIC -fno-PIE` | `-fno-PIE` | `-fno-PIE -fPIE` |
| `-fno-PIE -fno-pic -fno-PIC -fno-PIE` | `-fno-PIC` | `-fno-PIC -fPIE` |
| `-fno-PIE -fno-pic -fno-PIE` | `-fno-PIE` | `-fno-PIE -fPIE` |
| `-fno-PIE -fno-pic -fno-pic` | `-fno-pic` | `-fno-pic` |
