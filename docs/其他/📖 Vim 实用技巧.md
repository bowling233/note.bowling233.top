---
tags:
  - 读书笔记
  - 暂停
---

# 📖 Vim 实用技巧

!!! abstract "书籍信息"

    - **书名**：Vim 实用技巧/Practical Vim: Edit Text at the Speed of Thought
    - **出版社**：人民邮电出版社


## Make Your Vim More Efficient

### vim-plug

> GitHub Wiki: [vim-plug](https://github.com/junegunn/vim-plug/wiki/tutorial)

`vim-plug` is a Vim plugin manager.

### onedark

> GitHub Page: [onedark.vim](https://github.com/joshdick/onedark.vim)

`onedark` is a Vim color scheme.

### vim-airline

> GitHub Page: [vim-airline](https://github.com/vim-airline/vim-airline)

## 进阶操作

### 模式

Vim 中，不同模式下各种按键产生不同的动作。接下来学习每种模式的工作方式。

#### Normal 普通模式

Vim 的语法是**操作符+动作=操作**。额外的规则是：当操作符重复执行时，它将作用于当前行。这时有一些简化，比如 `gUgU` 简化为 `gUU`。

??? note "操作符"

    `:h operator` 可以查阅完整的操作符列表。这里也记录一些常用的操作符：

    | Operator | Usage |
    | - | - |
    | c | change |
    | d | delete |
    | y | copy to register |
    | g~ | reverse case |
    | gu | to lower |
    | gU | to upper |
    | > | increase indent |
    | < | decrease indent |
    | = | automatic indent |
    | ! | 不知道怎么用，使用外部程序过滤{motion}跨越的行 |


??? note "动作命令"

    `:h motion` 可以查阅完整的动作列表。这里记录一些常用的动作：

    | Motion | Usage |
    | - | - |
    | l | letter |
    | w | next word |
    | e | end of next word |
    | b | begin of next word |
    | aw | a word |
    | ap | a paragraph |

!!! tip "普通模式的哲学"

    普通模式是 Vim 的默认模式，你可能会觉得这很奇怪。Vim 这样做的原因是：程序员应该**只花一小部分时间编写代码，绝大多数时间用来思考、阅读**，以及在代码中穿梭浏览。

* 控制撤销的粒度

`u` 撤销最新的修改。**一次修改**可以是改变文档内文本的任意操作，从进入插入模式开始，直到返回普通模式为止，在此期间的所有操作作为**一次修改**。因此，控制对 ++esc++ 键的使用是很关键的。让每个**可撤销块**对应一次思考过程，你可以在每次停顿输入时按下 ++esc++，把每次换行替换成 ++esc++ + `o`。

但是，如果在插入模式期间使用**光标键**移动光标位置，将产生新的撤销块，这很符合逻辑。这同样对 ++period++ 操作产生影响。

* 构造可重复的修改

如果需要在多个地方执行操作，需要仔细考虑一下。

!!! note "考考你"

    你的光标在行尾，你如何删除最后一个词？

有三种方式：`db,x`, `b,dw`, `daw`。想一想，这三种操作按下 ++period++ 后会发生什么？应该选择谁？

* 对数字作简单运算

`<C-a>` `<C-x>` 分别对数字执行加减。如果**不带数字**那么它们会递增递减，如果**提供数字前缀**则可以加减任意整数。

如果光标在数字上，这个数字会被加减。比如在 5 上执行 `10<C-a>` 会变成 15。

如果不在数字上，会在当前行正向查找一个数字，跳到那里，对数字进行运算。

!!! info "数字的格式"

    如 `007` 这样的数字会被解释为八进制，对其加减也会按对应的进制进行。你可以在 `vimrc` 中设置 `set nrformats=` 将所有数字都作为特定进制处理。

* 次数和重复

删除两个连续单词有 3 种方法：`d2w`, `2dw`, `dw.`。你可以说出它们的区别吗？

!!! tip "用重复代替次数计算"

    计算次数真是一件挺麻烦的事，对吧？你真的确信自己能每次都算对次数吗？或许，我们宁肯用多几次 ++period++ 来代替错误计算次数带来的撤销麻烦。这也能让你有更好的**细粒度**控制。

在必要的时候使用次数。比如，你要替换 3 个单词为另外 2 个单词。此时，你可以使用 `c3w` 删除并进入插入模式。此后按一次 `u` 就可以撤销整个更改。这又是一种对细粒度的把控。

#### 插入模式

* 更正错误

`<C-w>` 删除前一个单词

`<C-u>` 删除至行首

* 粘贴

`<C-r>{register}`

!!! tip "插入寄存器"

    Vim 插入寄存器的方式是：如同这些文本从键盘上输入。因此，如果激活了自动缩进等选项，插入寄存器文本时也会受到影响。用 `<C-r><C-p>{register}` 可以按原义插入寄存器文本。当然，在普通模式下有更好的办法。

`<C-r>=` 使用表达式寄存器，`=` 代表表达式寄存器，它可以做一些简单的运算，并把结果插入光标处。
