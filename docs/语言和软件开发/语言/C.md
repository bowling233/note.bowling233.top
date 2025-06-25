# C

!!! quote

    - [C reference - cppreference.com](https://cppreference.com/w/c.html)
    - [ISO/IEC JTC1/SC22/WG14 - C](https://www.open-std.org/JTC1/SC22/WG14/)：公开的 C 语言标准草案。正式标准需要收费，但草案也够看了。

## 结构体

[Bit-fields - cppreference.com](https://cppreference.com/w/c/language/bit_field.html)

位域（bit field）：作为 struct 或 union 成员，声明符（declarator）形式如下

```text
identifier(optional): width
b1 : 5, : 11, b2 : 6, b3 : 2;
```

- 相邻的位域被合并（packed），无标识符用于填充（padding）
- pack 的顺序由实现决定，比如 left-to-right 或 right-to-left
- 只能是 unsigned int、signed int、int、_Bool 这几种类型，其他类型（包括原子类型）由编译器支持
- `:0` 结束上一段 padding，从下一个分配单元开始
- 无法使用指针、sizeof()

## 属性

[Attributes (Using the GNU Compiler Collection (GCC))](https://gcc.gnu.org/onlinedocs/gcc/Attributes.html)

[Attribute specifier sequence(since C23) - cppreference.com](https://en.cppreference.com/w/c/language/attributes.html)

可以为函数、变量、类型、语句声明额外的属性。提供两种方式：

- GNU 传统语法：`__attribute__` 关键字
- 新 C23/C++11 标准：`[[…]]`，其中 GNU 特定的包含在 gnu:: 命名空间中

下面是 GNU 语法：

__attribute__((attribute))
__attribute__((format(printf, 1, 2))) d1 (const char *, ...);
void (__attribute__((noreturn)) ****f) (void);
struct __attribute__ ((aligned (8))) S { short f[3]; };
typedef int more_aligned_int __attribute__ ((aligned (8)));

- GNU 建议，将属性直接放在 struct 关键字后，而不是右花括号后。因为逻辑上，在右花括号结束时，整个类型应该被完整地定义。

下面是常用的类型属性：

- packed：将所有成员紧凑排列，使所需内存最小。这会取消编译器的对齐优化。
- aligned, aligned(alignment)：指定该类型/变量的最小对齐
    - 必须是 2 的倍数
    - 不指定则为目标架构的最大对齐（8 或 16 字节）

## 对齐优化

[Common Type Attributes (Using the GNU Compiler Collection (GCC))](https://gcc.gnu.org/onlinedocs/gcc/Common-Type-Attributes.html#index-aligned-type-attribute)

ISO C 要求：struct 和 union 的 alignment 至少是所有成员 alignemnt 的最小公倍数