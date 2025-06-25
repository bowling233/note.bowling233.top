# Python 程序设计基础（第四版）

Start Out with Python (Forth Edition)

| 出版社         | 作者        | 年份 |
| -------------- | ----------- | ---- |
| 机械工业出版社 | Tony Gaddis | 2019 |

!!! abstract "简评"

    在我看来，这是对新手十分友好的一本书。书中对概念的讲解基础且详细（但对于对编程有一定经验的同学来说会有些繁琐），每节后会有**检查点**模块问你一些基础的概念，确保你掌握了重要的知识。穿插在文中的**聚光灯**模块对于每块知识点，给出一个小程序从目标到设计到实现的完整过程。这让我们可以很好地观察代码细节，不断巩固知识。（但这可能也是让人比较烦的一点：它的例程几乎每一行都有注释！Python 语言的代码本身可读性就非常高，并不需要太多注释。）

    本书对基础知识的编排比《[Python 编程：从入门到实践](PCC.md)》要好一些，但提供的项目实践不足。在阅读完本书后，建议移步到《Python 编程：从入门到实践》的项目部分开始阅读。

    本书叙述很详细，因此章末习题可以不做。必须要做的是每个小节后的**检查点**，这是对本节知识的一个检验，也是对下一节知识的一个铺垫。聚光灯的例程都要看过一遍（不用很细），其中有疑问的地方要自己动手写代码试一试。**实践非常重要**。

## Ch1. 计算机与编程

- 解释器与编译器是如何工作的？
- Python 解释器的两种工作模式：交互模式和脚本模式
- IDLE 的简单使用：使用命令行、编写程序、自动缩进、运行等。

## Ch2. 输入、处理与输出

### 设计一个程序

!!! tip "设计程序"

    任何一个职业的程序员都会告诉你：在正式编写代码前，要对程序进行精心的设计。在启动一个新项目时，程序员绝不会马上陷入细节，把开始编写代码作为第一步。他们会按部就班地从程序设计开始。

- 重点讲解了程序设计的两个步骤：理解任务、决定步骤
- 伪代码和流程图
- 简单流程图中的三类元素：椭圆（终止符）、平行四边形（输入输出）、矩形（处理）

### 输出

- `print()`

格式化输出的一些技术：

- `print('one', end='')` 默认情况下打印会以换行符结尾，`end` 参数可以指定末尾。
- `sep=` 指定输出各项之间的分隔符，==默认情况下是空格==。

嵌套使用 `format()` 函数可以实现更多的格式化，传递给它的参数称为格式限定符。

- `print('Your pay is$', format(1123.23, '12,.2f'), sep='')`
- 最前面的数字是域宽（右对齐），`.2` 精度，`f` 浮点数类型，`d` 整数，`e` 科学计数法，`,` 逗号分隔数字，`%` 输出为百分比形式。

### 字符串

- 可以用 `'` 或 `"` 括起
- 字符串中含有单引号或撇号时，用双引号括起；含有双引号时，用单引号括起
- 三引号 `"""` 或 `'` 可以括起上述两种文本，并可以括起跨行的字符串（换行符被包括进来）
- `+` 字符串拼接操作
- Python 中的字符串同样有一些转义序列

### 变量

- “引用”概念
- 变量命名规则
- ==**下划线分隔**==的命名法在 Python 中比较常见
- Python 关键字

### 类型

- `int` 和 `float` 类型
- ==使用函数 `type()` 判断变量的类型==
- `str` 类型
- Python 中的变量**可以引用任何类型的数据**，也就是说，可以用其他类型的数据项重新赋值

### 读取输入

- `input(prompt)` 返回字符串形式
- `int()`、`float()` 与 `input()` 嵌套调用
- 异常：如果异常没有得到正确的处理，这个意外将导致程序终止。

### 计算

- `//` 整数除法：依据结果有不同的舍入方式
    - 正数：截断
    - 负数：四舍五入
- `**` 指数运算
- 优先级

!!! tip "例外"

    指数运算共享操作数时，应当理解为从右向左，即：`2**3**4` = `2**(3**4)`。

!!! note "数据类型转换"

    在混合类型表达式中发生隐式类型转换

    - 整型升级为浮点型

    可以使用前面介绍的函数实现强制转换。

    - `int()` 截断

### 其他

- `#` 注释
- ==`\` 行连接符==
- 圆括号内的语句片段，无需行连接符

### 有名常量

> 幻数（magic number）：程序代码中出现的无法解释的数值。**使用有名常量替代幻数**，增强程序的自解释性（self-explanatory）。

### 机器龟图形库

!!! tip "ModuleNotFoundError: No module named 'tkinter'"

    在 WSL 上 `import turtle` 时可能出现如上报错。需要额外安装 `tkinter` 库：

    ```bash
    apt-get install python3-tk
    ```

- `turtle.showturtle()`
- `forward()`, `right()`, `left()`, `setheading()`, `heading()`
- `penup()`, `pendown()`
- `dot()`, `circle()` 值得注意的是龟龟位于圆的切线方向，而不是圆心。
- `pensize()`, `pencolor()`, `bgcolor()`
- 重设屏幕：
    - `reset()` 重置画笔颜色、归位、擦除所有图形。不改变背景。
    - `clear()` 擦除所有图形。
    - `clearscreen()` 重置、归为、重置背景。

本节的剩余部分详细介绍了龟龟画图的各种功能，此处略过。

## Ch3. 选择结构与布尔逻辑

### `if` 语句

基本形式如下：

```python
if condition:
    block of statement
elif condition:
    block of statement
else:
    block of statement
```

!!! note "缩进"

    Python 解释器通过缩进来识别语句块的开始和结束。

!!! note "True 和 False"

    True 和 False 是 Python 中的关键字。

### 布尔表达式

- `bool` 数据类型。
- **关系运算符**与 C 中相同
- **字符串可以直接比较**，比较原理与 C 中的 `strcmp` 类似。

!!! tip "ASCII 编码"

    再回忆一下重要的 ASCII 码

    | 字符 | 码位 |
    | - | - |
    | A~Z | 65~90 |
    | a~z | 97~122 |
    | 0~9 | 48~57 |
    | space | 32 |

- Python 中的**逻辑运算符**是：`and`、`or`、`not`。它们同样支持短路定值（Short-Circuit Evaluation）。

### 机器龟游戏

3.7 节介绍了一个简单的机器龟投球游戏。此处略过。

## Ch4. 循环结构

### `while` 循环

### `for` 循环

- Python 中的 `for` 循环用于针对一组数据迭代处理。

```python
for variable in [value1, value2, etc.]:
    statment
```

### 迭代器 `range()`

该函数创建一个称为 `iterable` 的对象类型。

```python
range(start, end, step)
range(end) # 从 0 开始
```

- 创建的迭代列表是左闭右开的。

!!! question "考考你"

    使用 `range()` 函数实现以下功能：

    - 用户控制的循环次数
    - 降序迭代

### 循环功能

本章的剩余部分讲述了一些循环编程模式：

- 编写累加器
- 带参数的赋值运算符（augmented assignment operator）
- 使用标记（Sentinel，一个标记数据序列结尾的特殊值）来控制循环结束
- 验证输入的循环
    - 启动读（priming read）
- 嵌套循环
- 使用循环的机器龟绘图设计（略）

## Ch5. 函数

本章先介绍 `void` 函数，再介绍有返回值的函数。

函数命名与变量命名规则相同。

### 函数的定义和调用

```python
def function_name():
    statement

function_name()
```

- 程序启动时会调用 `main` 函数。
- 解释器读取 `def` 语句时，在内存中创建一个函数，在调用语句处执行该函数。

!!! tip "Python 中的缩进"

    Python 程序员习惯使用四个空格缩进语句块中的代码行。

    只要语句块中所有行都缩进相同的数量，你可以使用任意数量的空格。

    Tab 键也可以，但不要与空格混用。

!!! tip

    语句块中的空白行会被忽略。

- 函数调用的参数**按值传递**。

### 使用函数设计程序

一般遵从自顶向下的设计方法。

- 流程图：函数调用图示 `|| funciton ||`。
- 层次图：描述函数间的关系。

!!! tip

    多使用上面的两种图来思考自己的程序设计。

### 变量和作用域

- 与 C 类似，局部变量在函数内创建，不能被其他函数访问。
    - 显然，不同函数可以有同名的局部变量。
- Python 允许指定实参传递给对应的形参，称为**关键字参数**：

```python
parameter_name=value
```

- 混合使用关键字参数（keyword argument）和位置参数（position argument）时，位置参数必须先出现。

### 全局变量

!!! tip "不应该使用全局变量"

    大多数程序员都认为应该限制使用全局变量

- 在函数内部**为全局变量赋值**，应当用 `global` 关键字声明这个变量是全局变量。
    - 使用时则不需要。

!!! note "赋值语句的思考"

    从这点可以看出，Python 的赋值语句同时具备了创建变量的功能。

```python
number = 0

def main():
    global number
    number = int(input())
```

- Python 中没有全局常量一说，但可以用全局变量模拟。在函数中不用 `global` 声明，就不能更改该全局变量的值。

### `import` 和标准库函数

#### 使用标准库函数

一些库函数内置在 Python 解释器中，可以直接使用，如 `print`、`input`、`range` 等。许多库函数存储在文件中，称为**模块（module）**，在安装 Python 时拷贝到计算机上。

使用模块中的函数，需要用 `import` 语句导入模块，并用 `module.function()` 调用函数。

```python
import random

random.randint()
```

#### 随机数库函数

`random` 库中有一些函数：

- `randint(start, end)` 产生闭区间内的随机整数
- `randrange(start, end, step)` 和 `range()` 的参数一致，返回列表中的随机整数
- `random()` 产生 0.0 到 1.0 （不包括）的随机浮点数
- `uniform(start, end)` 范围内随机浮点数

`random` 库生成的也是伪随机数。导入 `random` 库时，它获取系统时钟并将其作为种子值。

!!! warning "安全上的提示"

    系统时钟的精确度一般为 **0.01 秒**。

使用 `seed()` 可以设置种子值，这样就能制造相同的随机数序列。

### 编写有返回值的函数

只需要用 `return` 语句将值返回到调用者即可。

本节还介绍了使用 **IPO 图**来描述函数的输入、处理和输出。

在 Python 中，函数可以返回多个值。在赋值语句中调用时，需要在等号右边放置对应数量的变量。

```python
def multi_ret(val1, val2):
    return val1, val2

a, b = multiret(c, d)
```

### `math` 库

除了和 C 库类似的一些函数，还有：

- `ceil()`
- `degrees()`, `radians()`
- `exp()`
- `floor()`
- `hypot(x, y)`
- `log10()`

常量：

- `math.pi`
- `math.e`

### 模块化编程

可以编写自己的模块。Python 解释器会尝试在当前文件夹和预定义的路径查找模块。

### 机器龟来咯

本节使用模块化方法写了一些机器龟程序，略过。

## Ch6. 文件和异常

### 文件简介

Python 允许处理**文本文件**和**二进制文件**，本书只讲解文本文件。Python 同样提供**顺序存取**和**随机存取**（直接存取）的文件访问方法，本书只讲解顺序存取。

- 文件对象的概念

文件对象是与特定文件相关联的一个对象，并为程序提供使用该文件的方法。Python 变量可以引用一个文件对象。

- `open(filename, mode)` 函数

该函数返回一个文件对象。

本书只介绍三种简单的模式：`r`, `w`, `a`，这些模式与 C 类似。

如果以字符串形式指定文件路径，特别是 Windows 系统上，应当在字符串前加上 `r`：

```python
test_file = open(r'C:\User\test.txt', 'w')
```

!!! tip "字符串前缀"

    字符串前缀 `r` 指定的字符串是一个原始字符串，解释器将不会解释其中的转义序列。

- 写入文件

文件对象有 `write()` 方法，只接受字符串作为参数：

```python
test_file.write(string)
```

- 读取文件

`read()` 返回文件全部内容的字符串，`readLine()` 返回包括 `\n` 的一行。读取到文件结尾时，这些函数返回空串，这可以用于循环控制。

- 关闭文件 `close()`。
- `rstrip(str)` 从右侧去除特定字符，可以用于消除换行符。
- `str()` 将数据转化为字符串

### 文件循环控制

Python 中可以为文件编写优雅的 `for` 循环：

```python
for variable in file_object:
    statement
```

该循环在文件的每行迭代一次。

### 处理记录

第 6.3 节给出了几个**重要的处理记录的例程**，应当仔细研读，写一遍进行操作。

在本节应当掌握：

- 记录和字段的概念
- 添加和显示记录
- 搜索记录
- 修改记录
- 删除记录

!!! tip "随机存取方法"

    在本书中，我们仅采用顺序存取的方法，因此修改和删除都需要操作两个文件。在后续的课程中，我们会学习使用随机读写方法存取文件。

在修改文件的例程中出现了系统调用：`os.remove()` 删除文件和 `os.rename()` 重命名文件。

### 异常

**异常**是导致程序突然停止的一个错误，可以使用 `try` 和 `except` 处理异常。

如果你在程序中用某个数除以 0，你会看见这样的消息，称为 **traceback**。

```
Traceback (most recent call last):
    File "C:\Python\division.py," line 13, in <module>
        main()
ZeroDivisionError: integer division or modulo by zero
```

traceback 的最后一行显示引发的异常名称和简要说明。

有一些异常总是不能避免，比如你希望用户 `input` 一个整数，将其与其他数相乘。如果用户输入字符串，将其作为 `int()` 的参数就会导致 `ValueError` 异常。

#### 异常处理句柄

这些代码用于异常抛出时的响应。下面是一般格式：

```python
try:
    statement
except ExceptionName:
    statement
else:
    statement
finally:
    statement
```

- `try` 语句块是有可能引发异常的一个或多个语句
- 当 `try` 中的一个语句抛出异常后：
    - 若是 `except` 语句指定的异常，则立刻执行 `except` 语句，然后执行 `try/except` 块后的语句。
    - 若不是 `except` 语句指定的异常，则输出 `traceback` 并停止
- 若没有抛出异常，则 `except` 块被忽略。
- 可以有不指定异常名称的 `except` 语句。
- `else` 语句块只在没有异常的情况下执行。
- `finally` 语句块在 `try` 和所有异常句柄执行后执行（不论是否有异常），目的是执行清理操作，如关闭文件等。

#### 异常对象

抛出异常时，内存中将创建一个**异常对象**，包含关于该错误的默认错误消息。

```python
except ValueError as err:
```

上面这个做法将异常对象赋给 `err` 变量，可以传递给 `print()` 等函数，显示 Python 为该类错误提供的默认错误消息。

## Ch7. 列表和元组

**序列**是保存多个数据项的对象，它们一个接一个地存储。两种**基本序列类型**是：列表和元组，它们都是可以容纳各种类型数据的序列。列表是可变的，元组是不可变的。

### 列表

```python
even_numbers = [2, 4, 6, 8, 10]
info = ['Alice', 27, 10.45]
```

操作：

- `list()` 将特定类型的对象转换为列表。

    ```python
    numbers = list(range(5))
    ```

- `*` 重复操作符：当它的左边是一个序列，右边是一个整数时，它将复制列表的多个副本并连接

    ```python
    numbers = [0] * 5
    ```

- `for` 循环可以在列表上迭代
- `list[1]` 下标访问称为**索引**。
    - 负索引将从列表尾部开始，负索引与列表长度相加得到索引。最后一个索引是 `[-1]`。
    - 无效索引引发 `IndexError` 异常。
- `len()` 返回列表长度。
- 切片：`days[2:5]` 将返回从索引 2 到索引 5 的元素列表（左闭右开）。
    - 从头开始：`days[:5]`
    - 直到尾部：`days[2:]`
    - 整个副本：`days[:]`
    - 设置步长：`days[1:8:2]`

!!! note "无效的切片索引"

    无效的切片索引不会引发异常：

    - 超过尾部会自动替换为列表长度
    - 超过头部会自动替换为开始位置
    - `start` 大于 `end` 将返回空列表

- 查找：`item in list`，如果列表中能找到元素，则表达式为真。
- `list.append(item)`
- `list.index(item)`，如果找不到，抛出 `ValueError` 异常
- `list.insert(index, item)`，指定无效索引不会引发异常，自动替代。
- `list.sort()`，升序排列
- `list.remove(item)`，移除出现的第一个元素，如果找不到，抛出 `ValueError` 异常。
- `list.reverse()`
- 删除：`del list[2]`
- 最值：`max()` 和 `min()`

!!! note "拷贝列表"

    回忆：在 Python 中，将一个变量赋给另一个变量，只是使这两个变量引用内存中的同一个对象。

    你可以：

    - 使用循环和 `.append()` 复制列表
    - 使用连接操作符：`list2 = [] + list1`

### 处理列表

- 创建列表并遍历：

    ```python
    hours = [0] * NUM
    for index in range(NUM)
        statement
    ```

变量可以选用下标，也可以选用列表中的元素。

- 列表可以作为参数，也可以作为返回值。

#### 列表与文件

文件对象具有：

- `file.writelines()` 方法，将整个列表写入文件中一行，每个元素之间不会自动添加空格
- `file.readlines()` 方法，将整个文件返回为字符串列表

#### 二维列表（嵌套列表）

列表的元素可以是任何东西，包括列表。

### 元组

元组（tuple）是不可变的序列。

```python
tuple = (1, 2, 3, 4)
```

元组也支持与列表相同的一些操作：

- 下标索引
- 各种方法如 `index()`
- 内置函数如 `len()`、`min()` 和 `max()`
- 切片
- `in`
- `+` 和 `*`

不支持 `append`、`remove` 等操作。

如果创建只有一个元素的元组，应当加逗号 `tuple = (1,)`，否则只是赋值。

- `list()` 将元组转换成列表
- `tuple()` 将列表转换为元组

## 专题：`matplotlib` 包

`matplotlib` 不是标准 Python 库的一部分，需要安装：

```bash
sudo pip3 install matplotlib
```

??? note "附录 F：`pip` 工具"

    许多独立程序员编写的 Python 模块提供标准 Python 库不具备的功能，称为第三方模块，在 [PyPI](https://pypi.python.org) 网站上提供了大量的第三方模块。

    PyPI 提供的模块组织为**包**，从 Python 3.4 开始，`pip` 工具进入标准 Python。使用 `pip` 工具安装软件包，并验证软件包是否安装：

    ```bash
    sudo pip3 install package_name
    python3
    >>> import package_name
    ```

### 导入模块

`matplotlib` 包中含有的 `pyplot` 模块提供绘图功能。

```python
import matplotlib.pyplot as plt
plt.plot(arguments...)
```

上面的语句导入并为模块创建了别名。回忆：调用模块中的函数，必须加上前缀。

??? note "附录 E：`import` 语句详解"

    - 从模块中导入特定的函数或类：`from math import sqrt`
        - 这将允许你直接调用函数而不用加上模块名称。
        - 可以指定多个元素的名称：`from math import sqrt, radians`
        - 支持通配导入：`from math import *`
    - 别名：`as` 关键字为模块分配别名。
        - 可以与上面的 `from` 结合起来使用：`from math import sqrt as sqaure_root, tan as tangent`

### 折线图

创建折线图，需要自变量和因变量的列表

```python
x_coords = [0, 1, 2]
y_coords = [0, 3, 1]
plt.plot(x_coords, y_coords, marker='o')
plt.title('title')
plt.xlabel('x axis')
plt.ylabel('y axis')
plt.xlim(xmin = 1, xmax = 100)
plt.xticks([0, 1, 2], ['Baseball', 'Basketball', 'Football'])
plt.grid(True)
plt.show()
```

`marker` 标记符号：

- `s` ■ 方形 square
- `o` ⚪ 圆点 dot
- `*` ⭐ 星形
- `D` 💎 钻石形

如果将标记字符作为位置参数而不是关键字参数传入，则不会用线连接起来：`plt.plot(x, y, 'o')`。

### 条形图

条形图的两个列表分别代表：每个条形**左边缘**的 X 坐标，以及每个条形的高度。

```python
left_edges = [0, 10, 20, 30, 40]
heights = [100, 200, 300, 400, 500]
plt.bar(left_edges, heights, color=('r', 'g', 'b', 'w', 'k'))
plt.show()
```

- `width` 关键字参数可以自定义条形图的宽度。
- 支持与折线图类似的其他功能。
- `plt.barh()` 绘制水平条形图。

### 饼图

饼图使用 `pie()` 函数绘制，需要一个列表，其中包含每个扇形的大小。`pie()` 会按照每个元素相对整体的大小计算每个扇形的角度。

```python
slices = [10, 20, 30, 40]
plt.pie(slices)
plt.show()
```

- `labels` 关键字参数可以指定每个扇形的标签。
- `colors` 关键字参数可以指定每个扇形的颜色。

## Ch8. 深入字符串

!!! abstract

    - 基本操作
        - 逐字符访问
        - 连接
        - 切片
        - 查找子串
        - 重复
    - 对象方法
        - 测试
        - 查找与替换
        - 分割

字符串也是一种序列，适用前一章学过的部分内容。

- 使用 `for` 循环可以逐一访问字符串中的每个字符**副本**。也就是说，不能改变字符串中的值。
- 使用索引可以取得字符串中单个字符的**副本**。索引可以是负数，从右向左计数。如果超出范围，会引起 `IndexError` 异常。

一些工具：

- `len()` 返回字符串长度
- `+` 连接字符串，`+=` 连接并赋值，注意左侧必须是变量。
- 字符串也可以切片。

!!! note "字符串是不可变的"

    Python 中的字符串是不可变的，这意味着你不能改变字符串中的字符。如果你想改变字符串中的字符，你必须创建一个新的字符串。

- `in` 检查子串是否存在于字符串中。
- `not in` 检查子串是否不存在于字符串中。

### 对象方法

不列出全部，可自己类推。

测试方法：

- `isdigit()`
- `isalnum()`
- `islower()`

和 C 挺像的。

修改方法：

- `lower()`
- `lstrip()` 从左侧删除空白字符
- `lstrip(char)` 从左侧删除字符
- `rstrip()`
- `strip()` 从左右删除空白

查找与替换：

- `endswith(substring)` 是否以子串结尾
- `find(substring)`
- `replace(old, new)`
- `startswith(substring)`

重复操作符 `*`：

```python
for count in range(8, 0, -1):
    print('z' * count)
```

- `split(char)` 默认以空格分割，返回字符串列表

## Ch9. 字典与集合

!!! abstract

    - 字典
    - 集合
    - 对象序列化

### 字典

字典中的每个元素有两个部分：

- 键：必须是不可变对象，如字符串、整数、元组
- 值：可以是任何类型对象

创建字典：将键值对放在大括号内创建：

```python
book = {'xxx':'555-111', 'www':'666-111'}
```

可以创建空字典。

字典中的元素并不是有序存储的。

字典操作：

- 检索：`dictionary[key]`
    - 找不到时抛出 `KeyError`
- 检查 `in`, `not in`
- 添加或替换元素：`dictionary[key] = value`
- 删除：`del dictionary[key]`
- 数量：`len()`
- `for` 循环可以遍历字典中所有键

对象方法：

- `clear()`
- `get(key, default)` 找不到时会返回默认值而不是抛出异常
- `items()` 以元组形式返回所有键值对，称为字典视图，长这样：`[('xxx', '555-111'), ('www', '666-111')]`
- `keys()` 返回所有键
- `pop(key, default)` 返回对应值并删除键值对
- `popitem()` 以元组形式返回随机键值对并删除
    - 它可以进行**多重赋值**：`key, value = dictionary.popitem()`，多重赋值将返回的键和值**分别**分配给多个单独的变量。
- `values()`

### 集合

这里的集合和数学中的集合类似，包含一组**唯一**值。此外它无序、可以是任何类型对象。

创建集合：

```python
myset = set()
```

可以为 `set()` 传入一个列表、元组或字符串，这些对象中的**可迭代元素**会自动**去重**并转换成集合的元素。注意，不能传递多个参数，如 `set('aa', 'bb')`。

基本操作：

- `len()`
- `for` 循环迭代
- `in` 和 `not in`

对象操作：

- `.add()`
- `.update()` 可以添加可迭代元素
- `.remove()` 或 `.discard()` 删除元素
    - `remove()` 会抛出 `KeyError` 异常，而 `discard()` 不会
- `.clear()`

集合操作：

- `union(set2)`
- `intersection(set2)`
- `difference(set2)`
    - 也可以使用 `-` 运算符
- `symmetric_difference(set2)` 返回两个集合中不重复的元素（对称差集）
    - 也可以使用 `^` 运算符
- `issubset(set2)` 是否是子集
- `issuperset()`

结尾的两个操作也可以用 `<=` 和 `=>` 操作符代替。

### 对象序列化（Pickling）

将对象转换为字节流的过程称为**对象序列化**。标准库中的 `pickle` 模块提供了相关的功能。

```python
import pickle
outputfile = open('mydata.dat', 'wb')
pickle.dump(object, outputfile)
outputfile.close()

inputfile = open('mydata.dat', 'rb')
object = pickle.load(file)
inputfile.close()
```

当读取超过文件末尾时，`load()` 抛出 `EOFError` 异常。我们可以利用前面学过的异常处理来识别文件末尾：

```python
while not end_of_file:
    try:
        person = pickle.load(input_file)
    except EOFError:
        end_of_file = True
```

!!! note

    Python 序列化后的数据，并不如 C 那样纯净。比如，在 C 中使用 `fwrite()` 向文件中写入一个字符数组，那么文件中只有字符数组。若将 Python 的字符串对象写入文件，你可以看到解码出的字符串，但其则前后还有一些额外的字节。这可能是用于标记对象的类型。

## Ch10. 类与面向对象编程

!!! abstract

    - 创建类
    - 使用类
    - 方法和实例
    - 如何设计一个类

面向对象思想在 C++ 和计算机科学导论中有介绍，这里不再赘述。你只需要有这些概念：对象、封装、公共方法、私有方法、属性、类、实例。

### 创建类

```python
class Coin:
    def __init__(self):
        self.sideup = 'heads'

    def toss(self):
        if random.randint(0, 1) == 0
            self.sideup = 'heads'
        else:
            self.sideup = 'tails'
```

- 每个类的方法都需要 `self` 参数，它是操作的对象的引用。它不一定要命名为 `self` 但这是惯例。
- 大多数类都具有 `__init__()` 方法用于初始化，它通常是类定义中的第一个方法。
- 上面创建的类中，`sideup` 属性并不是私有的。要隐藏一个属性，**使用两个下划线作为属性的开头**，如 `__sideup`。

### 使用类

```python
def main():
    mycoin = Coin()
```

发生了两件事情：对象的实例在内存中被创建，初始化方法被调用。

### 模块化

把类存储在文件里，使用 `import` 使用它：

```python
import coin
def main():
    mycoin = coin.Coin()
```

记得加上包名。

### 方法

- `__str__()` 把对象传递给 `print()` 函数时自动调用。

方法有如下分类：

- 访问器（Accessor 或 getter）：返回对象的状态信息，但不修改它。
- 修改器（Mutator 或 setter）：修改对象的状态信息。

### 实例

对象的实例可以放置在序列、字典等中，就像我们之前一直做的那样。

!!! note "设计类的技巧"

    书本 10.4 节介绍了类的设计范式。这套规则比较系统，建议完整阅读一遍。如果你想写一个自动从文字描述中生成类的代码的 AI 程序，或许也可以参考这一范式。

## Ch.11 继承

!!! note "本书的用词"

    本书认为：

    - 超类 = 基类
    - 子类 = 派生类

**继承是一种 is a 关系**，即子类具有超类的所有特征，以及使自己特殊化的其他特征。

创建一个继承的类，在类名后跟上超类的类名即可：

```python
class Car(Automobile):
    def __init__(self, ...):
        Automobile.__init__(self, ...)
        self.__xxx = xxx
```

- 子类继承了超类的所有方法和数据属性。
- 一般调用超类的初始化方法初始化对应的属性。

## 后续章节

本书第 12、13 张分别介绍了递归和 GUI 编程，我们跳过。接下来，请移步至：[Python 编程：从入门到实践](PCC.md)
