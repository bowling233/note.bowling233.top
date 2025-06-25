---
tags:
  - 读书笔记
  - 正在做
---

# 📖 Head First HTML

> 大部分内容来自《Head First HTML》

- [CSS 验证器](https://jigsaw.w3.org/css-validator)

## 概念性知识

### 与 HTML 相关的操作

内联 CSS 样式：

```html
<head>
    <style>
        body {
            font-family: Arial, sans-serif;
        }
        h1 {
            color: blue;
        }
    </style>
</head>
```

链接外部 CSS 文件：

```html
<head>
    <!-- 基本链接 -->
    <link rel="stylesheet" href="styles.css">
    
    <!-- 针对不同设备的样式表 -->
    <link rel="stylesheet" href="mobile.css" media="screen and (max-device-width: 480px)">
    <link rel="stylesheet" href="print.css" media="print">
    <link rel="stylesheet" href="tablet.css" media="screen and (min-device-width: 481px) and (max-device-width: 1024px)">
    <link rel="stylesheet" href="desktop.css" media="screen and (min-device-width: 1025px)">
    
    <!-- 横屏和竖屏样式 -->
    <link rel="stylesheet" href="landscape.css" media="screen and (orientation: landscape)">
    <link rel="stylesheet" href="portrait.css" media="screen and (orientation: portrait)">
</head>
```

### 内置规则

- `@import` 允许导入其他 CSS 文件

- `@media` 创建特定于某些媒体类型的 CSS（如桌面/手机），查询方式和 `<link>` 标签相同。

    ```css
    @media screen and (min-device-width: 481px) {
        #guarantee {
            /* rules */
        }
    }
    ```

### 代码

- 注释：`/*` 和 `*/` 之间。

你想为一些 HTML 元素增加样式，应该怎么做？

1. 创建新的类
2. 设置规则
3. 为元素加上类

### 选择器

```css
h1,
h2 {
}
h1.thisClass {
}
.thisClass {
}
#thisID {
}
parent child {
}
a:link {
}
```

> 为什么会使用 `p#id` ？
>
> 在一组复杂的页面中，id 可能分配给不同元素。

上面的语句展示了选择器的用法

- `,` 分隔选择器
- `.` 表示 class
- `#` 表示 id
- ` ` 表示子孙选择器。父元素可以是元素/id
    - 这将选择所有指定的子孙元素，不管嵌套得多深
    - `>` 选择直接孩子
    - 更复杂的条件 `#elixirs blockquote h2`
- `:` 表示伪类（元素状态）（请保证冒号前后没有空格）
    - `<a>` : `link` `hover` `visited` `focus` `active`
    - 通用伪类（Pseudo-class）：`first-child` `last-child`

### 继承

- 不是所有样式都能继承
- 用树状结构来理解继承
- 覆盖继承：更特定的规则

### 总结：如何知道你的元素应用了什么规则？

1. 有没有某个选择器 **直接选择** 了这个元素？
2. 查看 **父元素** ，向上追溯整个继承体系
3. 默认值

在多个选择器中，更特定的胜出：元素 < 类 < 元素 + 类

如果出现同级，最后的规则胜出。

### 层叠

一些浏览器允许用户创建自己的样式：在属性声明的最后加上 `!important` 就能覆盖样式

浏览器如何找到一个元素的属性？

1. 收集所有的样式表

2. 找到所有匹配的声明（匹配元素且包含属性的规则）

3. 对匹配规则排序（作者、读者、浏览器）

4. 在三个角色各自的范围内按特定性对声明排序

    > 特定性的计算：`000` 三位数，这些分别给对应的位加 1，值越大的越特定
    >
    > - id
    > - 元素名
    > - 类/伪类

5. 冲突规则：按在各自样式表中出现顺序排序

## 网页设计知识

### 字体

- **无衬线** 字体在在计算机上更容易阅读：`sans-serif`

字体格式：

- TrueType: `.ttf`
- OpenType：`.otf`
- Web 开放字体：`.woff`

在 CSS 中增加 `@font-face` 规则，置于 `body` 规则之上

```css
@font-face {
    font-family: "xxx"; /*为该字体指定名称*/
    src: url("http://xx.woff"), url ("http://xx.ttf"); /*浏览器可能支持不同种的字体*/
}
```

### 颜色

Web 颜色由 **红、蓝、绿** 构成。

- 浏览器一定具有下面列出的 16 种颜色名。
- 百分比： `rgb(80%, 40%, 0%)`
- 数值： `rgb(205, 102, 0)` 从 0~255
- 十六进制分量：`#CC6600`
    - 每组分量两位数字相同可以简写，如 `#cb0` = `#ccbb00`
- HTML [颜色表](http://en.wikipedia.org/wiki/Web_colors)

### 大小

先看一些 `font-size` 的大小指标：

- `px` 像素
- `%` 相对值，通常是相对父元素的 `font-size` 来说
- `em` 比例因子，如 `1.2em`
- `pt` 磅数
- 关键字：通常逐级乘 $120\%$ 的比例增长
    - `xx-small`, `x-small`, `small` （一般为 12px）, `medium`, `large`, `x-large`, `xx-large`
    - 建议：`body` 使用 `small`

## 元素定位

零碎的：

- 内联元素
    - 你只会看到左边和右边增加空间。上下边距不会影响其他内联元素的边距，所以会与其他内联元素重叠，看不出效果。
    - **图像** 的宽度、内边距、外边距都表现得像是块元素。

### 流与元素定位

- 流是浏览器摆放 HTML 的方法：沿着元素流逐个显示所遇到的各个元素。
- **块** 元素添加换行：**这个换行很重要**，表明两个块元素不会并排出现。

    - **上下** 摆放的块元素，外边距合并，保留最大的那个。
    - 只要 **两个垂直外边距** 碰到一起就会折叠。
        > 这会引起很奇怪的情况：在嵌套元素中，如果外元素没有边框，那么两个元素的外边距会发生碰撞，进而折叠。这很可能让你莫名其妙。

- **内联** 元素水平摆放
    - **并排** 放置的内联元素，外边距叠加。

`float` 属性如何工作？

- `float` 的元素从流中消失，所以可以和其他块元素并排了。
    - 但其他块内的内联元素会围绕着浮动的元素。
- `clear` 属性，使得这个元素流入页面时，某方向不允许有浮动内容。
    - `right`, `left`,

冻结布局

- 可以避免一些因为窗口扩展带来的问题。
- 冻结布局就是为页面主题 **固定宽度**，并通过 `auto` 的左右边距使主体居中。

绝对定位

- `positon: absolute`，默认是 `static`。
    - `fixed` 将相对 **浏览器窗口** 而不是页面。
    - `relative` **正常流入页面**，但进行偏移。
- `top`, `right`, `bottom`, `left` 这些属性表明相对页面某个边距的位置。
- `z-index` 值越大，浮动元素越在顶部。
- 元素将放置在指定的位置
- 内联元素不再围绕

CSS 表格

- 只放置块元素，`<img>` 应当被包围在 `<div>` 中
- `display: table;` 为整个表格指定
- `display: table-row` 为表格内指定行或列
- `display: table-cell` 指定表格内的元素
- 表格元素的 `border-spacing` 被每个单元格继承
    - 这一边距与 `margin` 不同，**不会被折叠**
- `vertical-align: top;` 确保单元格中内容相对单元格顶部对齐。还可以是 `middle` `bottom`

固定定位

- `position: fixed`

小结

- 我们学习了几种布局：流体布局、浮动元素、冻结布局、凝胶布局
- 4 种定位：静态、绝对、固定、相对

## 样式速查

### 杂的

- `top`
- `left`
- `text-align`
    - 对所有 **内联内容** 对齐，只能在块元素上设置（直接在内联元素上不起作用）
    - 嵌套的元素也对齐了，这是因为 **继承**
    - `center`

### 字体

- `font-weight`
    - `lighter`, `normal`, `bold`, `bolder` （比较级的两个是相对值）
- `font-family`
    - 每个 `font-family` 包含 5 个字体系列：
    - 无衬线 sans-serif: Verdana, Arial Black, Trebuchet MS, Arial, Geneva
    - 衬线 serif: Times, Times New Roman, Georgia
    - 定宽 monospace: Courier, Courier New, Andale Mono
    - 手写 cursive: Comic Sans, Apple Chancery
    - 装饰 fantasy: LAST NINJA, Impact
    - 工作方式：候选字体列表 + 字体系列名。注意：字体名区分大小写，带空格的需要加引号。
- `font-size`
- `font-style`
    - `italic` 和 `oblique` 都是斜体，有时看起来不同。
- `font-variant`
- `line-height` 可以按字体高度的比例指定
    - 特别的点：可以直接指定一个数，表示是 **自己字体大小的几倍**。比如在 `<div>` 设为 1，那么里面的元素行高是各自的 1 倍，而不是 `<div>` 的行高的 1 倍。

> 它们可以简写为 `font`
>
> 必须指定大小，大小（行高放在字体大小后面的斜杠后）和其他元素的相对位置固定：
>
> `font-style` `font-variant` `font-weight` `font-size`/`line-height` `font-family`

### 文本

- `color`

    - `Aqua`, `Black`, `Blue`, `Fuchsia`, `Gray`, `Green`, `Lime`, `Maroon`, `Navy`, `Olive`, `Purple`, `Red`, `Silver`, `Teal`, `White`, `Yellow`
    - 实际上控制着一个元素的 **前景色**，所以会控制文本和边框颜色

- `text-decoration`

    - `underline`, `overline`, `line-through`, `none`

    > 规则的合并：
    >
    > - 如果你分别指定 `text-decoration=underline;` 和 `text-decoration=overline;`，那么不会得到上下线
    > - 应当指定： `text-decoration=underline overline;`
    >
    > 标记删除文本：HTML 具有这样的标签
    >
    > - `<del>` 标记需要删除的内容
    > - `<ins>` 标记插入的内容
    >
    > 注意，使用 HTML 能让内容的含义更加清晰

- `letter-spacing` 字符间距

### 列表

- `list-style-none`
    - `none` 删除项目符号
- （列表元素）`display`
    - `block`, `inline`

### 盒模型

??? note "盒模型由哪几个部分构成？"
_MARGIN
_ BORDER
_PADDING
_ CONTENT

与边框有关的属性：

- `border-color`
- `border-width`
    - `thin`, `medium`, `thick`
- `border-style`
    - `solid`, `double`, `groove`, `outset`, `dotted`, `dashed`, `inset`, `ridge`
- `border-radius`
- 指定某一边框
    - `border-top-color`
    - `border-right-style`
    - `border-bottom-width`
    - `border-top-left-radius`

内衬：

- `padding`
- `padding-left` 等

外边距：

- `margin`

> 有一些属性支持简写：
>
> - `padding` `margin`
>     - 上、右、下、左
>     - 全部
>     - 上和下、左和右
> - `border` 直接写 `width` `style` `color` 属性，顺序任意
> - `background` 直接写 `color` `image` `repeat`

背景：

- `background-image`
    - `url`
- `background-repeat`
    - 默认平铺
    - `no-repeat`, `repeat-x`, `repeat-y`, `inherit`
- `background-position`
    - `top`, `left`

盒模型的宽度

- `width` 内容区宽度
    - `auto` 延伸占满可以用的空间。
    - 百分比：所在容器为标准。
- `height`
    - 高度可能导致内容区溢出，一般不指定。

### 表格

`<table>`

- `caption-side`
    - `bottom`
- 表格元素没有外边距，取而代之的是针对整个表格定义的 `border-spacing` 属性。首先指定垂直间距，再指定水平间距。
- `border-collapse` 折叠/合并边框 `collapse`

> 为表格隔行增加颜色：`nth-child` 伪类
>
> - 该状态是一个元素相对于它的兄弟元素的数字顺序。
>
> - 以下选择器分别选择了偶数、奇数段落；甚至可以使用 n 指定简单表达式
>
>     ```css
>     p:nth-child(even) {
>     }
>     p:nth-child(odd) {
>     }
>     p:nth-child(2n) {
>     }
>     p:nth-child(2n + 1) {
>     }
>     ```

### 列表

`<li>`

- `list-style-type`
    - `disc`
    - `circle`
    - `square`
    - `none`
    - `decimal`
    - `upper-alpha`
    - `lower-alpha`
    - `upper-roman`
    - `lower-roman`
- `list-stype-image`
- `list-style-position`
    - `inside`
    - `outside`

## 更多选择器

- 伪元素
    - `:first-letter` 选择一个块元素中文本的第一个字母
- 属性选择器
    - `img[width="300"]` 选择宽度为 300 的图片元素
- 按兄弟选择
    - `h1+p` 前面有一个 `h1` 的 `p` 元素
- 结合选择器
    - 先定义上下文，再给出元素，最后指定伪类或伪元素
    - `div:greentea > blockquote p:first-line`

## 开发商特定 CSS 属性

名称一般为：`-开发商标识符-属性` 如：`-moz-transform`
