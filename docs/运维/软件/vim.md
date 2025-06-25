# Vim

## 文本操作

### 普通模式

- 跳转

    ```text
    % 匹配括号间跳转
    f
    F
    ; 重复最新 f/t 命令
    ```

- 粘贴

    ```text
    p 光标后粘贴
    P 光标前粘贴
    gp/gP 粘贴后光标停在文本末尾
    ```

- 替换

    ```text
    %s/old/new/g 全文
    %s/old/new/gc 全文，交互式确认
    ```

    ```text
    R 普通模式进入替换模式
    gR 虚拟替换模式，按屏幕上实际显示的宽度来替换字符，避免制表符的显示问题
    ```

- 撤销

    ```text
    <C-r> 还原撤销
    ```

- 搜索

    ```text
    ? 向前搜索
    <C-o> 回到上一个光标位置
    ```

### 插入模式

```text
<INS> 插入模式/替换模式切换
```

## 窗口操作

- 切换窗口：++ctrl+w++

## 文件命令

```text
:r FILENAME 读取文件到当前光标处
:r !cmd 读取命令输出到当前光标处
:w FILENAME 写入文件
```

## 配置

### 配置文件

```text
set number
set relativenumber
```

### 配置命令

```text
:set option? 查询选项状态
:set option! 切换选项状态
:set option=value 设置选项值
```
