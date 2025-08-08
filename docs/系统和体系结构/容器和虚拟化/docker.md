# Docker

- 镜像构建
    - Docker 镜像构建为前后端架构：Buildx 为客户端，BuildKit 为服务端。
- 存储

    !!! quote

        - [A Deep dive into Docker Storage - Medium](https://medium.com/geekculture/docker-storage-1dd3db3ded4c)

    - 默认情况
        - 位于 `/var/lib/docker`，卸载时不会自动清除。
        - 层次化结构，由 Image 层（只读）和 Container 层（运行时数据，可写）组成。如果需要修改 Image 层数据，采用 copy-on-write 方式。
        - 与容器强绑定，容器删除后数据也消失。
    - 持久化存储
        - Volume：
            - 在 `/var/lib/docker/volumes` 下创建，不随容器消失。
            - 可以放置在远端存储。
            - 使用 `docker volume` 命令操作，
        - Bind mount：
            - 挂载宿主机的文件系统。
            - 允许挂载 NFS（使用 Docker 自己的 NFS 驱动，即不需要在宿主机上挂载）。
- 网络
    - 网络对容器透明，它只能看到一个具有 IP 地址的网络接口、网关、路由表和 DNS 服务。
    - 在 Linux 上，Docker 使用 iptables 实现网络。目前有迁移到 nftables 的计划。
    - Docker 自动为 `FORWARD` 链添加 `DROP` 策略，阻止宿主机作为路由器。如果需要，应当手动添加规则。
    - 支持的网络驱动有：`bridge`（默认）、`host`、`none`、`overlay`、`ipvlan`、`macvlan`。
    - `bridge`：部署时建议使用用户定义的 `bridge` 网络。
        - 默认网桥 `bridge`：
            - 自动创建，容器默认连接到该网桥。
            - 容器间只能通过 IP 地址通信。需要使用 `--link` 参数在双方创建连接才能使用自动 DNS。
            - 必须停止容器才能移出默认网桥。
        - 用户定义 `bridge`：
            - 自动 DNS 功能：容器可以使用 IP 地址或**容器名**相互沟通。
            - 热插拔：不需要关闭容器即可移动到其他网络。

    !!! note "Compose 中的网络"

        Compose 会自动为应用创建一个网络，容器可以通过服务名相互访问。

    - `host`：
        - 端口映射不生效。
    - 端口映射：
        - Docker 使用 iptables 进行相关 NAT、PNAT 和 masquerade 操作。
        - 默认情况下所有外部主机都能访问公开端口，如果需要限制，需要自己写 iptables 规则。
        - IPv6 下可能配置为直连路由。
        - `-p host:docker`
- 容器
    - 重启策略：`no`、`always`、`unless-stopped`、`on-failure`。

## 版本

Docker 官方和 Debian APT 的打包策略不同。

在 Debian APT 中，相关软件包说明如下：

- `docker.io` 守护进程
- `docker-cli` 命令行工具
- `docker-compose` 服务编排工具（独立命令 `docker-compose`）

在官方仓库，相关软件包如下：

- `docker-ce` 守护进程
- `docker-ce-cli` 命令行工具
- `docker-compose-plugin` 作为 Docker CLI 的插件（子命令 `docker compose`）

目前，Debian 源中的 `docker-compose` 仍然是 v1 版本（Python 编写），而官方仓库中的 `docker-compose-plugin` 是 v2 版本（Go 编写）。Debian 团队正在努力将 v2 版本打包到源中。最新进展可见 [Debian Bug Report](https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=1040417)。截止 2024 年 7 月，v2 版本的 Go 依赖处理仍在进行中。

OpenWRT 上的 docker-compose 也是 v1 版本。

## 命令

```bash
# 镜像
docker image save image > image.tar
docker image load < image.tar
```

!!! note "Docker Compose 运行自定义命令"

    - [Docker-Compose Entrypoint/Command - Stack Overflow](https://stackoverflow.com/questions/54160315/docker-compose-entrypoint-command)

    Docker 容器只会执行一条命令，该命令退出后容器也会退出。执行的命令由如下方式决定：

    - 如果容器未定义 entrypoint，那么执行 command。
    - 如果容器定义了 entrypoint，那么执行 entrypoint，将 command 作为 entrypoint 的参数。

    为容器提供 entrypoint 和 command 的方式如下：

    | 运行方式 | 定义 entrypoint | 定义 command |
    | --- | --- | --- |
    | `docker run` | `--entrypoint <entrypoint>` 选项 | 镜像名后的参数 |
    | Dockerfile | `ENTRYPOINT` 指令 | `CMD` 指令 |
    | Compose 文件 | `entrypoint:` | `command:` |

    !!! example

        使用下面的 Dockerfile 构建容器：

        ```dockerfile
        ENTRYPOINT ["echo"]
        ```

        使用下面的 compose.yml 文件运行容器：

        ```yaml
        services:
          test:
            build: .
        ```

        使用下面的命令运行容器，command 将被作为参数：

        ```bash
        $ docker compose run test hello world
        hello world
        ```

        而使用下面的命令可以进入 Shell：

        ```bash
        $ docker compose run --entrypoint bash test
        ```

## 工具

- [Dive](https://github.com/wagoodman/dive)

## snippet

一些常用的内容存放在这里：

### 镜像构建

!!! quote

    - [docker buildx build | Docker Docs](https://docs.docker.com/reference/cli/docker/buildx/build/)
    - [Dockerfile reference | Docker Docs](https://docs.docker.com/reference/dockerfile/)

```bash
docker build --add-host=<host>:<ip> --progress=plain \
    --tag <name>:<tag> .
```

- SSH 配置：

    ```dockerfile
    # host key
    RUN ssh-keygen -A
    # ssh key
    RUN mkdir -p /root/.ssh \
        && chmod 700 /root/.ssh \
        && ssh-keygen -t rsa -N "" -f /root/.ssh/id_rsa \
        && cat /root/.ssh/id_rsa.pub >>/root/.ssh/authorized_keys \
        && chmod 600 /root/.ssh/authorized_keys
    # 免校验
    RUN echo "Host *" >>/root/.ssh/config \
        && echo "    StrictHostKeyChecking no" >>/root/.ssh/config \
        && echo "    UserKnownHostsFile=/dev/null" >>/root/.ssh/config
    RUN sed -i 's/^#*Port .*/Port 3333/' /etc/ssh/sshd_config \
        && sed -i 's/^#*PermitRootLogin .*/PermitRootLogin yes/' /etc/ssh/sshd_config \
        && sed -i 's/^#*PasswordAuthentication .*/PasswordAuthentication no/' /etc/ssh/sshd_config \
        && sed -i 's/^#*PubkeyAuthentication .*/PubkeyAuthentication yes/' /etc/ssh/sshd_config
    ```

### Compose

!!! quote

    - [Compose file reference | Docker Docs](https://docs.docker.com/reference/compose-file/)
    - [docker compose | Docker Docs](https://docs.docker.com/reference/cli/docker/compose/)

```bash
docker compose down --remove-orphans --volumes
```

- 特权：

    ```yaml
    services:
      test:
        privileged: true
        cap_add:
          - ALL
    ```

### Daemon 配置

```json
{
    "registry-mirrors": [
    ],
    "insecure-registries": [ 
    ],
    "data-root": "",
}
