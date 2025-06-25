# 操作系统

本文用列表的方式简单对比三大主流平台的操作系统（Windows、macOS、Linux）常用运维命令及工具，以命令行为主。

## Shell

!!! quote

    - [PowerShell Documentation - PowerShell | Microsoft Learn](https://learn.microsoft.com/en-us/powershell/)
    - [PowerShell/PowerShell: PowerShell for every system!](https://github.com/PowerShell/PowerShell)
    - [Bash Reference Manual](https://www.gnu.org/software/bash/manual/bash.html)
    - [sh](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/sh.html)
    - [Shell Command Language](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html)

一点关于 Shell 的历史：

- 目前 Windows 预装的是 PowerShell 5.1，不开源。开源且跨平台支持的是 PowerShell 7+。
- `sh` 是 POSIX 标准，并非一个真正的 Shell 实现，实际上链接到系统上的某个 Shell。
- `bash` 并不完全符合 POSIX 标准。当以 `sh` 或 `--posix` 模式运行时，`bash` 能够兼容 POSIX 标准。

| | PowerShell | Bash |
|-|-|-|
| 脚本扩展名 | `.ps1` | `.sh` |
| 运行脚本命令 | `pwsh -noexit` | `bash` |
| 退出 Shell | `exit` | 同 |
| 获取帮助 | `Help about_Line_Editing` | `man bash` |
| 逐词移动 | ++ctrl+left++ | 同 |
| 删除到行首 | ++ctrl+home++ | ++ctrl+u++ |
| 删除到词首 | ++ctrl+backspace++ | ++ctrl+w++ |
| 搜索历史命令 | ++ctrl+r++ | 同 |
| 打开文件/目录 | `ii .`/`Invoke-Item .` | `open` |
| 查找命令 | `Get-Command` | `type`/`which` |
| 列出环境变量 | `Get-ChildItem Env:` | `printenv` |
| 用户名 | `[System.Security.Principal.WindowsIdentity]::GetCurrent().Name` | `whoami` |
| 重定向 | `>` | `>` |

PowerShell 的基础知识：

- Execution policy：控制脚本的执行权限，一般是 `Restricted`（不允许运行任何脚本）。使用 `Set-ExecutionPolicy RemoteSigned` 允许运行本地脚本和签名的远程脚本。
- PowerShell 是面向对象的脚本语言：
    - 对象操作：
        - 查看对象成员：`Get-Member [-MemberType { Property | Method } ]`
        - 从集合中选对象：`Where-Object { <condition> }`
            - 属性匹配：`Name -EQ value'
        - 选择对象：`Select-Object -Property <property1>,<property2>,...`
    - 输出格式化：`Format-List`

## 启动

### Linux GRUB

- BIOS 直接进系统了，试试按住 ++shift++ 可以进入 Grub。

- 设置默认启动项：[linux - Set the default kernel in GRUB - Unix & Linux Stack Exchange](https://unix.stackexchange.com/questions/198003/set-the-default-kernel-in-grub)，有几种方式：

    - 用 `grub-set-default` 命令
    - 去 `/boot/grub/grub.cfg` 找到对应启动项的序号，写入 `/etc/default/grub` 的 `GRUB_DEFAULT=` 中

## 远程控制/桌面

| | Windows | macOS | Linux |
|-|-|-|-|
| SSH Server 配置文件 | `C:\ProgramData\ssh` | `/etc/ssh/` | 同 |

Windows RDP：

- RDP 本身容易被攻击，不建议防火墙开放 RDP 端口，而是通过 SSH 端口转发访问。
- 登录：

    - 本地账户：直接使用用户名和密码登录即可。
    - 微软账户：使用微软账户的主要邮箱地址和密码登录。

        如果 Remote Desktop 进入到了账户密码验证界面，但无法使用 Microsoft 账户密码登录，请参考这个解答：[StackExchange: Unable to access a remote computer through Remote Desktop Connection when using a Microsoft Account](https://superuser.com/questions/1222431/unable-to-access-a-remote-computer-through-remote-desktop-connection-when-using)。省流如下：

        > Further to the above, I believe (from later found articles) that if the user signs in with the Microsoft's account password at least once rather than the PIN the issue may also be fixed. (Have confirmed this as the best fix)

        可能当初我们安装系统时，就是直接使用 Microsoft 账户，并随即按照提示设置好了 PIN。上面的回答指出，**我们必须至少使用 Microsoft 账户的密码在本机上登录一次后，远程桌面才会接受我们使用 Microsoft 账户密码进行的登录。**为了使用 Microsoft 账户密码登录本机，可能需要修改设置中的 Require Windows Hello sign-in for Microsoft accounts 等选项，具体可以查阅解答。

!!! note "安装 Windows 系统时，请从本地账户开始"

    综合上面的解答和评论来看，安装 Windows 时，先不登录 Microsoft 账号是最好的。

    - 你可以自定义本地账户的名称，否则微软会截取你账户名称的前几个字符，使用户文件夹名称看起来很丑。
    - 不会出现上面远程登录的这些问题。

## 网络

- Windows：
    - `ipconfig`：网络配置
    - `netstat`：监控网络连接
    - `net`：网络配置
    - `route`：路由表


