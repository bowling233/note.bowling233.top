# 调试

## 基础知识

### 符号文件

### 断点原理

## GDB

- 输出超长字符串：

    ```text
    set print elements 0
    ```

---

如果你要调试系统级软件包

## ddeb

<https://wiki.debian.org/AutomaticDebugPackages>
<https://www.debian.org/doc/manuals/debmake-doc/ch10.en.html#dbgsym>

<https://docs.fedoraproject.org/en-US/packaging-guidelines/Debuginfo/>

## coredump

!!! quote

    - [core(5) - Linux manual page](https://man7.org/linux/man-pages/man5/core.5.html)

Core dump (核心转储) 是一个文件，它包含了进程在终止时的内存快照，可用于调试。

- **生成条件**：
    - 某些信号（如 `SIGSEGV`、`SIGABRT`）会默认导致进程终止并生成 core dump 文件。
    - 需要有写入 core dump 文件的权限和足够的文件系统空间。
    - 进程的资源限制 (`RLIMIT_CORE`) 不能为零。
    - 被执行的二进制文件需有读权限。
    - 对于设置了 SUID/SGID 或文件能力的程序，默认情况下不会生成 core dump（除非进行了特定配置）。
- **命名和路径**
    - 默认文件名是 `core` 或 `core.PID`。
    - `/proc/sys/kernel/core_pattern` 文件可以自定义 core dump 文件的命名模板，支持多种变量（如 `%p` 代表 PID）。
    - 模板可以包含目录，用来指定 core dump 的保存路径。
- **其他控制选项**
    - **管道模式**: `/proc/sys/kernel/core_pattern` 如果以 `|` 开头，core dump 数据会被作为标准输入传给一个用户态程序进行处理。
    - **内存过滤**: `/proc/pid/coredump_filter` 文件可以控制哪些类型的内存映射（如匿名映射、文件支持映射）被写入 core dump。
    - **systemd**: 在使用 systemd 的系统上，core dumps 可能会被管道传输给 `systemd-coredump` 服务，并保存在 `/var/lib/systemd/coredump/` 中，可以使用 `coredumpctl` 工具进行管理。
    - `gdb` 的 `gcore` 命令也可以为运行中的进程生成 core dump。

### systemd-coredump 与 coredumpctl

!!! quote

    - [](https://documentation.suse.com/en-us/sles/15-SP7/html/SLES-all/cha-tuning-systemd-coredump.html)

```bash
$ cat /proc/sys/kernel/core_pattern
|/usr/lib/systemd/systemd-coredump %P %u %g %s %t %c %h %e
```

在 journal 中可以看到：

```text
kernel: Thread (pooled)[2787986]: segfault at 0 ip 0000000000000000 sp 00007f6fffffbe30 error 14 in installer[558adeb46000+163000]
kernel: Code: Bad RIP value.
systemd[1]: Started Process Core Dump (PID 2787988/UID 0).
systemd-coredump[2787990]: Resource limits disable core dumping for process 2787979 (installer).
systemd-coredump[2787990]: Process 2787979 (installer) of user 0 dumped core.
systemd[1]: systemd-coredump@25-2787988-0.service: Succeeded.
```

```text
kernel: Thread (pooled)[59207]: segfault at 0 ip 0000000000000000 sp 00007f901574ae30 error 14 in installer[55ab2d090000+163000]
kernel: Code: Bad RIP value.
systemd[1]: Started Process Core Dump (PID 59209/UID 0).
systemd-coredump[59211]: Process 59205 (installer) of user 0 dumped core.
                           
                           Stack trace of thread 59207:
                           #0  0x0000000000000000 n/a (n/a)
                           #1  0x00007f901746b7e6 vforkfd (libQt6Core.so.6)
                           #2  0x00007f9008104188 n/a (n/a)
systemd[1]: systemd-coredump@2-59209-0.service: Succeeded.
```

```text
[root@SE11 intel-mpi-2021.16.0.443_offline]# bash -x ./install.sh --silent --cli --eula accept
++ getScriptPath ./install.sh
++ script=./install.sh
++ '[' -L ./install.sh ']'
+++ command dirname -- ./install.sh
+++ dirname -- ./install.sh
++ scriptDir=.
+++ cd .
+++ command pwd -P
+++ pwd -P
++ scriptDir=/data/bowlingzhu/intel-mpi-2021.16.0.443_offline
++ printf %s /data/bowlingzhu/intel-mpi-2021.16.0.443_offline
+ scriptLocation=/data/bowlingzhu/intel-mpi-2021.16.0.443_offline
+ /data/bowlingzhu/intel-mpi-2021.16.0.443_offline/bootstrapper --silent --cli --eula accept
Checking system requirements...
Done.
Wait while the installer is preparing...
Done.
Launching the installer...
./install.sh: 行 34: 540258 段错误               (核心已转储)"$scriptLocation"/bootstrapper "$@"
# dmesg
[20293.676358] Thread (pooled)[540288]: segfault at 0 ip 0000000000000000 sp 00007f13b2802e30 error 14 in installer[55872db79000+163000]
[20293.676362] Code: Bad RIP value.
```


