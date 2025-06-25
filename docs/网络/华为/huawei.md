# 华为路由器/交换机笔记

## AR101GW-Lc-S

### [基础配置](https://support.huawei.com/enterprise/zh/doc/EDOC1100033933)

- 默认帐密：`admin`/`Admin@huawei`
- 用户界面

    ```text
    display user-interface [ ui-type ui-number1 | ui-number ] [ summary ]
    # 用户界面视图下
    idle-timeout <minutes> [ seconds ]
    screen-length <screen-length> [ temporary ]
    histrory-command max-size <size-value>
    authentication-mode <authentication-mode>
    ```

    - Console

        ```text
        user-interface console 0
        speed <speed-value>
        ```

    - VTY

        ```text
        user-interface maximum-vty <number>
        user-interface vty <first-ui-number> [ last-ui-number ]
        shell
        protocol inbound { all | ssh }
        ```

- 用户和认证

    ```text
    display user [ all ]
    display local-user
    display access-user
    ```

    - AAA

        ```text
        # 系统视图下
        aaa
        local-user <user-name> password irreversible-cipher <password>
        local-user <user-name> service-type <service-type>
        local-user <user-name> privilege level <level>
        ```

    - Password

        ```text
        # 用户界面视图下
        set authentication password cipher
        ```

- STelnet

    ```text
    # 用户
    display ssh user-information
    ssh user <user-name> authentication-type { password | rsa | password-rsa | ecc | password-ecc | all }
    # 服务器
    display ssh server status
    display ssh server session
    stelnet server enable
    ssh server permit interface { interface-type interface-number }
    ```

- 时间

    ```text
    display clock
    ```

    - 手动

        ```text
        clock timezone <time-zone-name> { add | minus } <offset>
        # clock timezone BJ add 8
        clock datetime HH:MM:SS YYYY-MM-DD
        # clock datetime 10:00:00 2021-01-01
        ```

    - NTP

        ```text
        ntp-service unicast-server <server-ip> [prefer] [source <interface-type interface-number>]
        # ntp-service unicast-server
        ```

- Web 网管

    ```text
    http server load <file-name>
    set insecure-protocol enable # 允许 HTTP 和 Telnet
    http server enable
    http secure-server enable
    http server permit interface { interface-type interface-number }
    set web login-style { professional | simple }
    ```

- 文件管理

    ```text
    # 目录
    pwd
    cd
    dir [ /all ] [ filename | directory ]
    mkdir <directory-name>
    rmdir <directory-name>
    # 文件
    more [ /binary ] <filename> [ offset ] [ all ]
    copy <source-filename> <destination-filename>
    move <source-filename> <destination-filename>
    rename <old-filename> <new-filename>
    zip <source-filename> <destination-filename>
    unzip <source-filename> <destination-filename>
    delete [ /unreserved ] [ /force ] { filename | devicename }
    undelete { filename | devicename }
    reset recycle-bin [ filename | devicename ]
    execute <batch-filename>
    # 存储器
    format <drive>
    remove <drive>
    ```

    - TFTP

        ```text
        # 服务器
        set default tftp-server <directory>
        tftp server enable
        # 客户端
        tftp [ -a source-ip-address | -i interface-type interface-number ] <tftp-server> [ public-net | vpn-instance vpn-instance-name ] { get | put } <source-filename> [ destination-filename ]
        # tftp 10.0.0.1 get test.txt
        ```

- 杂项

    ```text
    sysname <host-name>
    save
    autosave interval on
    autosave interval { time | configuration time }
    ```

- 检查

    ```text
    display current-configuration
    ```
