---
tags:
    - 需要整理
---

# Python

## 版本管理

Python 语言和软件包更新频繁，不同的项目会使用不同版本的语言和依赖，因此需要做版本控制。

从 Python 3.3 开始，一般使用 Python 自带的模块 `venv` 来创建虚拟环境：

```bash
python3 -m venv <DIR>
source <DIR>/bin/activate
```

创建虚拟环境并加载环境变量后，就能使用 `pip` 命令来安装依赖包了。

需要注意的是 `venv` 配置文件使用绝对路径，因此环境不可移植（跨系统、跨设备）。最好的方式是使用 `requirements.txt` 文件来记录依赖包，这样可以保证不同系统下的依赖包一致。

## Python 书籍目录

我的 Python 学习之路从经典的 Python 三剑客丛书开始。

| 中文名 | 英文名 |
| - | - |
| Python 程序设计基础（原书第 4 版） | Starting Out with Python, 4th edition |
| Python 编程：从入门到实践 | Python Crash Course: A Hands-On, Project-Based Introduction to Programming |
| Python 编程快速上手：让繁琐工作自动化 | Automate the Boring Stuff with Python |
| Python 数据科学手册 | Python Data Science Handbook |

一些书评：

- 《Python 编程快速上手》：目的性很强，快速上手 Python 在实际应用中最常见的用法，只需要写用完就扔掉的代码。因此它并不会向你介绍很多编程方面的细节，甚至根本不提面向对象等知识。在我看来，这本书适合希望快速将 Python 应用到学习工作中的读者。如果选择 SOP 和 PCC 等书，那么大部分的时间都将花在编程和语言的学习上，并不能立刻将它应用到工作上。

## 基础语法

关键字和内置函数

- 定义：`def`、`class` 等
- 布尔等表达式运算：`False`、`True`、`None`、`and`、`or`、`not` 等
- 控制结构：`while`、`for`、`if`、`else`、`elif` 等
- 其他

### `if` 语句

- 比较方法：

    - `in` 检查特定值是否在列表中
    - 使用 `.lower()` 检查字符串是否为小写（注意个这个函数并不能直接检测，怎么用呢？）

- `if-elif-else` 结构
- 一些布尔表达式：
    - Python 中支持连续的比较，如 `age >= 18 and age <= 65` 可以简写为 `18 <= age <= 65`。

### 循环

- 使用 `break` 和 `continue`
- 设置标志
- 用循环处理列表和字典
    - 删除指定值
    - 移动元素
    - 学习下面这种用法，以列表作为循环条件：

        ```python
        users = ['user1', 'user2', 'user3']
        while users:
            user = users.pop()
            print(user)
        ```



### 函数

```python
def function_name(parameters):
    """docstring"""
    function body
```

#### 参数

- 关键字实参：`function_name(parameter=value)`'
- 默认值：`def function_name(parameter=value)`
    - 在函数定义中，应当将没有默认值的参数放在前面，有默认值的参数放在后面，以便 Python 依然能够正确解读位置实参。
- 禁止函数修改列表：`function_name(list_name[:])` （创建副本）
- 任意数量实参：`def function_name(*parameters)`，`*` 会创建一个空元组，将所有值都封装到这个元组中。
- 任意数量的关键字实参：`def function_name(**parameters)`，`**` 会创建一个空字典，将所有值都封装到这个字典中。

!!! note "参数的顺序"

    -   在定义和调用中，都应当按普通形参、带默认值的形参、任意数量形参的顺序排列。

#### 将函数存储在模块中

- 导入整个模块：`import module_name`
- 导入特定函数：`from module_name import function_name`
- 函数别名：`from module_name import function_name as fn`
- 模块别名：`import module_name as mn`
- 导入所有函数：`from module_name import *`

!!! tip "函数代码规范"

    - 形参指定默认值时等号两边不要有空格
    - 函数调用时，关键字实参等号两边不要有空格
    - 对齐参数列表行

    ```python
    def function_name(
            parameter_0, parameter_1, parameter_2,
            parameter_3, parameter_4, parameter_5):
        function body...
    ```

### 类

#### 编写类的基础

- 与类有关的每个方法都需要 `self` 参数，且必须位于参数列表的第一个位置。
- 类的每个属性都必须有初始值。

#### 继承

```python
class ChildClass(ParentClass):
    """docstring"""
    def __init__(self, parameters):
        super().__init__(parameters)
        self.attribute = value
```

- 子类的方法 `__init__()` 需要父类的方法 `__init__()` 来初始化父类的属性。
- `super()` 是一个特殊函数，帮助 Python 将父类和子类关联起来。
- `super().__init__(parameters)`：调用父类的方法 `__init__()`。
- 重写父类的方法
- 将实例用作属性（其实就是类的嵌套）：一些类的细节越来越多，应当拆分成多个协同工作的小类。


### 异常

- `try-except-else` 代码块
- `pass` 语句：什么都不做，只是占位符，使代码结构正确。


### 代码风格

- 类名使用驼峰命名法
- 实例名和模块名使用小写字母和下划线
- 每个类定义后都应当包含一个文档字符串
- 每个模块都应包含一个文档字符串

## 字符串与 I/O

字符串的写法：

- `r` 前缀：原始字符串，不转义。常用于路径、正则表达式。
- **_三重引号_**：其间的所有引号、制表符或换行都被认为是字符串的一部分。**_Python 的代码缩进规则不适用于三重引号_**，你的缩进也会被打印出来。三重引号推荐这样写，保证输出与代码看起来一致。其中第一行的 `\` 防止文段开头多一个换行符。

```python
print('''\
Dear Alice,
    Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.
Sincerely,
Bob''')
```

- 多行字符串还经常用作多行注释。

各类字符串方法：

- `isX()` 系列：`isalpha()`、`isalnum()`、`isdecimal()`、`isspace()`、`istitle()`。
- 转换：`upper()`、`lower()`、`title()`、`capitalize()`、`swapcase()`。
- 查找：`startswith()`、`endswith()`。
- 多字符串：`join()`、`split()`
- 裁剪：`strip()`、`lstrip()`、`rstrip()`。
- 对齐：`ljust()`、`rjust()`、`center()`。
- 将其他数据转换为字符串 `str(value)`

- 修改大小写 `.title()`、`.upper()`、`.lower()`
- 拼接
- 删除空白 `.rstrip()`、`.lstrip()`、`.strip()`

    !!! tip "空白"

        空白指任何非打印字符。

### 文件

- `with` 语句：在不再需要访问文件后将其关闭。

```python
with open(filename) as file_object:
    contents = file_object.read() # 读取整个文件
    for line in file_object: # 逐行读取
        print(line.rstrip())
```

- `open()`：默认以只读模式打开文件。

!!! question "考考你"

    以下问题如何解决？

    -  读取文件时，空行会出现两次换行
    -  将文件内容全部合并成一行
    -  从文件中读取数字等其他类型的值

文件对象操作：

- `.read()`：读取整个文件
- `.write()`：写入文件。注意不会在末尾添加换行符。

### 剪贴板

使用 `pyperclip` 包（需要安装）可以与系统剪贴板交互。

- `pyperclip.copy()`：将字符串复制到剪贴板。
- `pyperclip.paste()`：将剪贴板中的内容粘贴到字符串。

!!! example "练习"

    从剪贴板上获取多行字符串，将它们转换成 Markdown 格式的列表。

    ```python
    import pyperclip
    text = pyperclip.paste()
    lines = text.split('\n')
    for i in range(len(lines)):
        lines[i] = '* ' + lines[i]
    text = '\n'.join(lines)
    pyperclip.copy(text)
    ```

    关注代码中如何处理换行符。

### 正则表达式

`re` 模块处理正则表达式：

- `re.compile()`：创建一个正则表达式对象（Regex 模式对象）。
    - 用例：`phoneNumRegex = re.compile(r'\d\d\d-\d\d\d-\d\d\d\d')`
    - 记得用原始字符串。
- Regex 对象有如下方法：
    - `search()`：找第一次匹配
        - 输入：一个字符串。
        - 返回：一个 **Match 对象**，包含匹配文本的**第一个**出现的位置。
        - Match 对象有如下方法：
            - `group()`：返回匹配的字符串。
                - 输入：一个整数（表示分组），0 或留空表示整个匹配。
                - 返回：对应的字符串。
    - `findall()`：找所有匹配
        - 输入：一个字符串。
        - 返回：一个**字符串列表**，包含匹配的所有字符串。
        - 如果有分组，返回的是元组的列表。每个元组是一个**匹配**，每个元组中的项是改匹配中的各个分组。
    - `sub()`：替换操作
        - 输入：两个参数，第一个是用于取代匹配的字符串，第二个是要被替换的字符串。
            - 在第一个参数中，可以使用 `\1`、`\2` 等来引用匹配的分组。
        - 返回：替换后的字符串。

正则表达式语法

- `()` 分组：
    - 分组就是将多个字符当作一个单元。
- `|` 管道：匹配多个表达式中的一个。
- `?` 可选匹配：前面的分组出现 0 次或 1 次。
- `*` 零次或多次匹配：前面的分组出现 0 次或多次。
- `+` 一次或多次匹配：前面的分组出现 1 次或多次。
- `{n}` 匹配 n 次：前面的分组出现 n 次。
    - 可以指定一个范围，比如 `{1,3}`。第一或第二个数字可以不写。
- `?` 非贪心匹配：限制前面的分组匹配尽可能少的文本。
    - 默认情况下，正则表达式是贪心的，即匹配尽可能长的字符串。

字符分类：

- `\d` 任何数字
- `\D` 除数字外的任何字符
- `\w` 任何字母、数字或下划线字符（可以认为是匹配“单词”字符）
- `\W` 除字母、数字和下划线以外的任何字符
- `\s` 空格、制表符或换行符（可以认为是匹配“空白”字符）
- `\S`

使用中括号创建自己的字符分类。

- `^`：匹配必须发生在字符串开始处。
- `$`：匹配必须发生在字符串结尾处。
- `.`：通配符，匹配除换行符外任何字符。
    - 如果要匹配换行符，使用 `re.DOTALL` 作为 `re.compile()` 的第二个参数。
- 大小写无关匹配：使用 `re.IGNORECASE` 或 `re.I` 作为 `re.compile()` 的第二个参数。

管理复杂的正则表达式：

- 使用 `re.VERBOSE` 或 `re.X` 作为 `re.compile()` 的第二个参数，可以在正则表达式中添加注释。

用例：

```python
phoneRegex = re.compile(r'''(
    (\d{3}|\(\d{3}\))?              # area code
    (\s|-|\.)?                      # separator
    \d{3}                           # first 3 digits
    (\s|-|\.)                       # separator
    \d{4}                           # last 4 digits
    (\s*(ext|x|ext.)\s*\d{2,5})?    # extension
)''', re.VERBOSE)
```

加了注释，正则表达式就清晰多啦！

使用管道符号可以将多个 `re.compile()` 的第二个参数结合起来使用，比如 `re.compile(r'pattern', re.IGNORECASE | re.DOTALL)`。

## 数据结构

### 列表

```python
list = [1, 2, 3]
```

基础：

- 添加元素：`.append(value)`、`.insert(index, value)`
- 删除语句：`del list[index]`、`.pop(index)`（尾部删除）、`.remove(value)`
- 排序：`.sort()`、`.sort(reverse=True)`
- 反转：`.reverse()`
- 长度：`len(list)`

更多操作：

- 遍历

!!! warning "循环结束后"

    `for` 循环结束后，迭代使用的变量仍然存在。

- `range(start, end, step)`：生成一个整数序列，不包含 `end`，默认 `start=0`，`step=1`。
- 切片 `list[start:end:step]`：不包含 `end`，默认 `start=0`，`step=1`。
    - 利用切片复制列表 `list[:]`。

!!! note "列表解析"

    这是一种比较高阶但常用的技巧，使用一行代码生成特定的列表：

    ```python
    squares = [value**2 for value in range(1, 11)]
    ```

    -   首先指定列表名
    -   再定义一个表达式 `value**2` 生成你要存储到列表中的值
    -   接下来写一个 `for` 循环给表达式提供值

    请练习生成一个 3 的倍数的列表。

### 字典

```python
dict = {'key': 'value'}
```

基础：

- 访问：`dict['key']`
- 添加：`dict['key'] = 'value'`
- 创建空字典：`dict = {}`
- 修改值：`dict['key'] = 'new value'`
- 删除键值对：`del dict['key']`

较长字典的缩进方法：

```python
dict = {
    'key1': 'value1',
    'key2': 'value2',
    'key3': 'value3',
    }
```

- 遍历：
    - `for key, value in dict.items():`
    - `for key in dict.keys():`
    - `for value in dict.values():`
    - 按顺序遍历 `for key in sorted(dict.keys()):`

更多操作：

- 字典的列表
- 字典中嵌套列表
- 字典嵌套字典

## 大型项目

### Modules

!!! quote

    - [6. Modules — Python 3 documentation](https://docs.python.org/3/tutorial/modules.html)

Module：

- Python 文件就是 Module，文件名和模块名一致，模块内部 `__name__` 变量为模块名，例如 `numpy.__name__` = `numpy`
- 导入模块：

    ```python
    from a import b as c
    ```

    解释器会搜索 `sys.builtin_module_names` 和 `sys.path` 的路径，包括：

    - 输入脚本的文件夹
    - `PYTHONPATH`
    - `site` 模块提供的安装路径，如 `site-packages`

- 执行模块：此时 `__name__` 为 `__main__`
- 为了加快载入，Python 会将模块编译为 `__pycache__/module.version.pyc`，这是一种字节码
- `sys` 标准模块内置在每个 Python 解释器中
- `dir()` 可以显示模块定义的所有名字（变量、函数、模块），无参数时显示当前定义的所有名字，内置的名字不自动显示

    ```python
    dir(builtins)
    ```

Package：

- `__init__.py` 让解释器认为该文件夹是一个 Package

    - 可以定义 `__all__`，当 `from a import *` 时将导入其中包含的模块

        ```python
        __all__ = ["foo"]
        ```

        如果不定义，则不会导入子模块

- 在包内可以使用相对（没有斜杠）和绝对路径导入其他模块

    ```python
    from ..a import b
    ```

### Python Packaging User Guide

!!! quote

    - [Python Packaging User Guide](https://packaging.python.org/en/latest/)

| 术语 | 含义 |
| - | - |
| Pure Module | 单个 Python 文件构成的模块 |
| Extension Module | 由其他语言编写的 Python 扩展，通常是动态库文件（如 `.so`）|
| Import Package | 可以包含子模块或其他包的Python 模块 |
| Distribution Package / Project | 可以安装的软件，可以提供多个 Import Package<br>例：Pillow 提供 `PIL` |

- 源码分发
    - 任何包含 Python 文件的目录都被视为一个 Import Package
    - 如果是纯 Python 编写，就可以使用源码分发
    - 称为 sdist，为 `.tar.gz` 格式，包含一个或多个 Package 或 Module
    - sdist 规范要点：命名为 `{name}-{version}`，包含 `pyproject.toml` 和 `PKG-INFO` 文件
- 二进制分发
    - 如果依赖非 Python 库，应当使用二进制分发
    - 使用 Wheel 格式打包二进制，并且 pip 偏好这种格式，即使存在源码包，因为它更快
    - Whell 格式要点：ZIP 格式压缩，扩展名 `{distribution}-{version}(-{build tag})?-{python tag}-{abi tag}-{platform tag}.whl`
        - 安装过程：解压、移动到指定目录、编译所有 `.py` 为 `.pyc`
- 此外还有一种老的 egg 格式，已经被弃用

打包流程：

- 准备源码树和 `pyproject.toml`，它指定了构建工具

    ```toml
    [build-system]
    requires = ["setuptools"]
    build-backend = "setuptools.build_meta"
    ```

    这里称为 backend，是因为由 frontend 来运行它们。frontend 可以是 pip、build 等工具。

- 构建 sdist 和一些 wheel

    ```bash
    python3 -m build --sdits <dir>
    python3 -m build --wheel <dir>
    ```

- 上传到 PyPI 等分发服务

#### setuptools

!!! warning "`setup.py` 是已经弃用的方式，应当改用 `pyproject.toml`。"

setuptools 作为后端，不需要手动下载，用上面的 build 前端可以自动下载调用。


 
### Pybind11

### 测试

`unittest` 模块：用于核实函数的行为是否符合预期。

- 导入 `unittest` 模块和要测试的函数
- 创建一个继承 `unittest.TestCase` 的类

```python
import unittest
from name_function import get_formatted_name

class NamesTestCase(unittest.TestCase):
    """测试name_function.py"""
    def test_first_last_name(self):
        """能够正确地处理像Janis Joplin这样的姓名吗？ """
        formatted_name = get_formatted_name('janis', 'joplin')
        self.assertEqual(formatted_name, 'Janis Joplin')

unittest.main()
```

- `NamesTestCase` 中所有以 `test_` 开头的方法都将自动运行。
- 常用的断言方法：
    - `assertEqual(a, b)`：核实 a == b
    - `assertNotEqual(a, b)`：核实 a != b
    - `assertTrue(x)`：核实 x 为 True
    - `assertFalse(x)`：核实 x 为 False
    - `assertIn(item, list)`：核实 item 在 list 中
    - `assertNotIn(item, list)`：核实 item 不在 list 中
- `setUp()` 方法：只需创建一次对象，然后在每个测试方法中使用它。
- 测试结果：
    - `.`：测试通过
    - `E`：测试引发错误
    - `F`：测试断言失败

## 标准库

- `collections`：包含很多有用的类
    - `OrderedDict` 类：记录键值对的添加顺序
- `argparse`

    ```python
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", ...)
    args = parser.parse_args()
    print(f"{args.input}")
    ```

    - 以 `-` 开头的识别为**可选参数**，否则为**位置参数**
    - 参数最终存放的变量名由 `add_argument()` 的 `dest` 参数决定，默认是对**第一个长选项名进行 strip**：移除开头的 `-`，将中间的 `-` 转换为 `_`

        !!! example

            ```python
            add_argument('-f', '--foo-bar', '--foo')
            ```

            结果为 `foo_bar`
    
## 常用第三方库

### 

---

待整理

## Projects

### Project 2 数据可视化

#### json 模块

在第 10 章我们学习了 json 模块，现在简单回顾复习文件操作和模块使用：

- `json.dump(data, file_object)`：将 Python 数据结构转换为 JSON 格式并写入文件。
- `json.load(file_object)`：从文件中读取 JSON 格式的数据并转换为 Python 数据结构。

示例：

```python title="remember_me.py"
import json
filename = 'username.json'
try:
    with open(filename) as file_object:
        username = json.load(file_object)
except FileNotFoundError:
    username = input("What's your name? ")
    with open(filename, 'w') as file_object:
        json.dump(username, file_object)
        print("We'll remember you when you come back, " + username + "!")
else:
    print("Welcome back, " + username + "!")
```

#### matplotlib

!!! tip "查阅手册"

    需要时请翻阅手册查找用法

以下功能应该如何实现？

- 绘制：折线图、散点图
- 设置样式：
    - 文字：标题、坐标轴标题
    - 刻度（`tick_params`）：标记、大小、颜色
    - 线条：粗细、颜色
    - 点：大小、颜色
    - 各部分的字体大小

#### 生成数据

完成以下任务：

- 绘制从 1 到 100 的平方图像（列表解析）。
- 用随机漫步方法绘制图像，可以使用散点图和折线图（`random.choice()`）。
- 为图表使用颜色映射。
- 使用 Pygal 生成矢量图形文件。
- 写一个掷骰子实验，一个和多个骰子。

#### CSV 文件

- `csv` 模块：用于读取和写入 CSV 文件。
- `csv.reader(file_object)`：读取文件并返回一个读取器对象，其中包含以逗号分隔的值。
- `next(reader)`：返回文件中的下一行。

其他函数：

- `enumerate()`：返回一个包含索引和值的元组列表。
- `datetime` 模块：
    - `strptime()`：将字符串转换为日期对象。

## 零散 tips

- `None` 值：这是 `NoneType` 数据类型的唯一值。如果你希望变量中的值不会和其他东西混淆，你可以使用它。
    - 使用没有返回值的函数进行赋值也会得到 `None`，你可以认为 Python 在这些函数末尾都加上了 `return None`。
- 引用到底是怎么回事？
    - 你可以将变量看作一个包含值的盒子
    - 对于**不可变数据类型**的值，比如**字符串、整型或元组**，盒子里装的就是值本身。
    - 对于**可变数据类型**的值，比如**列表、字典**，盒子里装的是值的引用。
    - 技巧：`copy` 模块：
        - `copy.copy()`：复制列表或字典这样的可变值。但是，如果列表的里面有列表呢？
        - `copy.deepcopy()`：同时复制列表、字典或列表中的列表这样的可变值。
- 技巧：`pprint` 模块 漂亮打印：
    - `pprint.pprint()`：将列表、字典等数据类型打印成漂亮的格式。
    - 如果希望得到字符串，使用 `pprint.pformat()`。
