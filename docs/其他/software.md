# 软件使用经验和技巧

## 好用的软件

| 类别 | macOS | Linux | Windows | Android |
| --- | --- | --- | --- | --- |
| 压缩工具 | [Keka](https://www.keka.io/en/) | | Bandizip | |
| 播放器 | [IINA](https://iina.io/) | VLC | PotPlayer | VLC |
| 窗口管理 | [Rectangle](https://rectangleapp.com/) | | | |
| 终端 | Termius | Termius | Termius | Termius |
| 远程摄像头 | | | | Iriun Webcam |
| 投屏 | | | | [scrcpy](https://github.com/Genymobile/scrcpy) |
| 网络工具 | | | | [CellularZ](http://www.cellularz.fun/)<br/>网络万用表 |
| 文件管理 | | [Midnight Commander](https://midnight-commander.org/) | | |
| 久坐提醒 | [Time Out](https://dejal.com/timeout/) | [Safe Eyes](https://slgobinath.github.io/SafeEyes/) | [Big Stretch Reminder](https://monkeymatt.com/bigstretch/) | |
| 按键展示 | [KeyCastr](https://github.com/keycastr/keycastr) | | [YetAnotherKeyDisplayer](https://github.com/Jagailo/YetAnotherKeyDisplayer)/[Kling](https://github.com/KaustubhPatange/Kling) | |
| 连点器 | [macos-auto-clicker](https://github.com/othyn/macos-auto-clicker) | | [OP Auto Clicker](https://www.opautoclicker.com/) | |

## 编辑器

- 使用 [EditorConfig](https://editorconfig.org/) 统一编辑器配置

### VSCode

- 烦人的自动补全

    在写程序代码时，自动补全能极大地提高效率，但在撰写文档过程中常常相反。比如在 Markdown 中开启代码建议，则常常会跳出前面出现过的句子等，按下 ++enter++ 键就直接键入了，非常智障。但关闭代码建议后，自定义的用户代码片段也不会自动跳出。最好的解决方法是：指定 ++tab++ 键为接受代码建议的按键。更多代码建议的信息，请参考 [How to disable IntelliSense in VS Code for Markdown? - Stack Overflow](https://stackoverflow.com/questions/38832753/how-to-disable-intellisense-in-vs-code-for-markdown)。

- 标题栏

    在 Linux 系统上安装 VSCode 可能会出现很厚的标题栏。在 `settings.json` 中设置

    ```json
    "window.titleBarStyle": "custom"
    ```

    即可隐藏系统自带标题栏。你还可以进一步隐藏菜单栏，设置 `"window.menuBarVisibility"` 即可，参考 [vs code 界面去除菜单栏_window.menubarvisibility": "default",-CSDN 博客](https://blog.csdn.net/qq_28120673/article/details/81544136)。

- 用户代码片段

    可以在 VSCode 中配置用户代码片段，为常用的代码片段提供便捷的智能输入。[Snippet Generator](https://snippet-generator.app/) 是一个用户代码片段生成器。

- `tasks.json`：用于项目生成任务，参考官方文档 [Tasks](https://code.visualstudio.com/docs/editor/tasks)。

    Tasks 的目的是将各种自动化项目构建系统（如 Make 等），通过命令行聚合到 VSCode 中。

    在 VSCode 中配置的一般顺序如下：首先在 `tasks.json` 中编写自己的 task，然后在命令面板中选择 **Configure Default Build Task** 设置某个 task 为默认选项，此后便可以使用 ++ctrl+shift+b++ 执行任务。

    !!! info "tasks 示例"

        ```json
        {
        "version": "2.0.0",
        "tasks": [
            {
                "type": "cppbuild",
                "label": "C/C++: gcc.exe 生成活动文件",
                "command": "C:\\mingw64\\bin\\gcc.exe",
                "args": [
                    "-fdiagnostics-color=always",
                    "-g",
                    "${file}",
                    "-o",
                    "${fileDirname}\\${fileBasenameNoExtension}.exe"
                ],
                "options": {
                    "cwd": "${fileDirname}"
                },
                "problemMatcher": [
                    "$gcc"
                ],
                "group": "build",
                "detail": "调试器生成的任务。"
            }
        ]
        }
        ```

        这是 VSCode 默认生成于 Windows 中的执行文件。还是比较容易读懂它的作用是什么的。为了可移植性，我将 `command` 更改为 `gcc`，经过测试在 Windows 和 Linux 上均可用。其他东西基本不需要动。

- `launch.json`：用于调试（执行编译好的文件），参考官方文档 [Debug](https://code.visualstudio.com/docs/editor/debugging)。

    VSCode 利用语言对应的扩展来支持调试。不同的调试器支持的配置也不同，以下以 `gdb` 调试器为例。

    !!! info "launch 实例"

        ```json
        {
            "version": "0.2.0",
            "configurations": [
                {//Windows 和 Linux 通用，gdb 调试
                    "name": "gdb",
                    "type": "cppdbg",
                    "request": "launch",
                    "program": "${fileDirname}\\${fileBasenameNoExtension}",
                    "args": [],
                    "stopAtEntry": false,
                    "cwd": "${fileDirname}",
                    "environment": [],
                    "externalConsole": false,
                    "MIMode": "gdb",
                    "miDebuggerPath": "gdb",
                    "setupCommands": [
                        {
                            "description": "将反汇编风格设置为 Intel",
                            "text": "-gdb-set disassembly-flavor intel",
                            "ignoreFailures": true
                        }
                    ],
                    "preLaunchTask": "build",
                }
            ]
        }
        ```

## 终端

!!! quote

    - [命令行界面 (CLI)、终端 (Terminal)、Shell、TTY，傻傻分不清楚？](https://prinsss.github.io/the-difference-between-cli-terminal-shell-tty/)
    - [控制台、终端和 Shell 的关系](https://www.eet-china.com/mp/a46011.html)。
    - [Unix 终端系统（TTY）是如何工作的？](https://waynerv.com/posts/how-tty-system-works/)
    - [TTY: under the hood](https://www.yabage.me/2016/07/08/tty-under-the-hood/)

### Tmux

!!! quote

    - [Getting Started · tmux/tmux Wiki](https://github.com/tmux/tmux/wiki/Getting-Started)

- 会话、窗口和窗格（sessions, windows and panes）
- 使用 `/tmp` 中的 socket 文件与服务端通信。服务端在后台运行，管理运行在 tmux 中的所有程序并跟踪它们的输出。

常用命令：

- 创建会话：`tmux new`
    - `-s` 指定会话名称
    - `-n` 指定窗口名称
- 附着：`tmux attach`
    - `-t` 指定会话的名称
    - `-d` 使其他附着到该会话的客户端脱离
- 列出会话：`tmux ls`

快捷键：`C-b` 开头

- `:` 打开内置命令行。
- `q` 显示窗格编号。
- `?` 查看按键列表。

常用内置命令：

- 窗格
    - `select-layout` 选择布局
        - `even-vertical` 垂直等分
        - `even-horizontal` 水平等分
    - `break-pane -t <pane_id>` 将当前窗格分离为一个新窗口
    - `join-pane -s <source_pane> -t <target_window>` 将窗格合并到目标窗口
- `kill-server` 关闭服务端，结束所有会话。
- 窗口
    - `split-window` 分割窗格
        - `-h`, `-v` 水平、垂直分割
        - `-d` 不改变活动窗格
        - `-f` 让新窗格占据整个横、纵向位置
        - `-b` 将新窗格置于左、上方
    - `neww` 新的窗口
        - `-d` 创建但不设为当前窗口
        - `-n` 设置名字
        - `-t` 设定序号
        - `command` 最后跟执行的命令

## 杂项

### Vivado 自定义编辑器 VSCode 无法打开

报错内容：vivado unable to launch external text editor

原因：VSCode 默认的执行路径不是 `.exe` 文件，而是为了方便在 WSL 中使用而包装的脚本文件。查看 Windows Path，可以看到设置在 `AppData\Local\Programs\Microsoft VS Code\bin` 下，而该目录下只有几个脚本文件：

![vivado_texteditor_error](./software.assets/vivado_texteditor_error.png)

大致内容如下：

```cmd
@echo off
setlocal
set VSCODE_DEV=
set ELECTRON_RUN_AS_NODE=1
"%~dp0..\Code.exe" "%~dp0..\resources\app\out\cli.js" %*
endlocal
```

在 PowerShell 中执行 `(Get-Command code).Path` 也显示为 `AppData\Local\Programs\Microsoft VS Code\bin\code.cmd`，因此 Vivado 对脚本的调用不成功（推测是 Java 调用外部程序，没有 Shell 环境）。

解决方法：真正的 `code.exe` 在上层目录，将 `Path` 末尾的 `bin` 去掉即可。
