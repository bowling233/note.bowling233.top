# OpenStack

OpenStack 能够统一管理裸金属服务器、虚拟机、容器等计算资源，以及存储和网络资源，提供弹性伸缩的云计算服务。

OpenStack 整个项目全部使用 Python 开发。

OpenStack 自身由一系列相互协作的组件组成，每一个组件各自独立又相互关联。考虑 OpenStack 的这种架构，使用容器部署是更加方便的选择。OpenStack 自身有两个项目支持容器部署：

- Kolla：提供 OpenStack 各组件的容器镜像和部署工具
- OpenStack-Helm：基于 Helm Charts 的 OpenStack 容器编排

最终，我们选择使用 OpenStack-Helm 来部署 OpenStack。这要求先搭建好 Kubernetes。

## 基本设置

需要确定两个密码：

- 数据库密码：用于连接外部数据库（如 MariaDB）的密码
- 管理员密码：OpenStack 的 `admin` 账户的密码

需要配好的东西：

- 网络
- DNS，需要解析节点名
- NTP
- 数据库，推荐使用 [MariaDB](https://mariadb.org/)
- 消息队列，推荐使用 [RabbitMQ](https://www.rabbitmq.com/)

### etckeeper

!!! quote

    - [etckeeper - Ubuntu Server documentation](https://documentation.ubuntu.com/server/how-to/backups/install-etckeeper/)

OpenStack 的配置文件直接放在宿主机的 `/etc` 目录下。`etckeeper` 工具能够对 `/etc` 目录进行版本管理，方便我们追踪配置文件的变更。

安装：

```shell
apt install etckeeper
```

`etckeeper` 默认行为如下：

- 使用 Git 管理
- 每日 Commit 一次
- 软件包安装开始前/结束后 Commit

`etckeeper vcs` 相当于 `git` 命令，例如：

```shell
etckeeper vcs log
etckeeper vcs diff
```

### MariaDB

### RabbitMQ

Erlang Cookie？

### Memcached

### etcd

## 身份：keystone

安装：

```shell
apt install keystone
```

配置：`/etc/keystone/keystone.conf`，设置好数据库连接和 token provider。

```text
[database]
# connection = dialect+driver://username:password@host:port/database_name
connnection = mysql+pymysql://**:**@**/keystone
[token]
provider = fernet
```

初始化：

```shell
keystone-manage db_sync
keystone-manage fernet_setup --keystone-user keystone --keystone-group keystone
keystone-manage credential_setup --keystone-user keystone --keystone-group keystone
```

启动：

```shell
keystone-manage bootstrap --bootstrap-password ${PASS} \
  --bootstrap-admin-url http://${HOSTNAME}:5000/v3/ \
  --bootstrap-internal-url http://${HOSTNAME}:5000/v3/ \
  --bootstrap-public-url http://${HOSTNAME}:5000/v3/ \
  --bootstrap-region-id RegionOne
```

验证：

```shell
# 配置好 OpenStack 客户端环境变量
openstack
(openstack) domain list
```

## 看板：horizon

horizon 唯一的依赖是 keystone，所以安装完 keystone 后我们马上安装 horizon，以便使用 Web UI 进行后续组件的管理。

安装：

```shell
apt install openstack-dashboard
```

## 网络：neutron

## 镜像：glance

## 调度：placement

## 计算

### 虚拟机：[nova](https://docs.openstack.org/nova/)

nova 统筹管理 Openstack 的所有计算资源。它的主要职责是管理虚拟机，而容器和裸金属节点则通过其他组件集成进 nova。



### 裸金属：[ironic](https://docs.openstack.org/ironic/)

ironic 通过 PXE、IPMI 等方式管理裸金属机器。它包含下列组件：

- `ironic-api`：处理请求。
- `ironic-conductor`：与裸金属节点交互，执行节点部署、管理等任务。
- `ironic-python-agent`：节点启动初期运行在 ramdisk 中的代理程序，提供节点硬件信息采集等功能，辅助 conductor 完成任务。
- `ironic-novncproxy`：将主机的图形终端连接到 NoVNC。

部署裸金属节点的前置条件：

- ironic 的各组件正常运行
- 配置 nova 使用 ironic 作为裸金属计算资源的后端
- glance 中放置一些镜像
- 配置好裸金属节点的 API，比如 IPMI

### 容器：magnum

magnum 并没有再造容器编排工具的轮子，而是将现有工具（如 K8S）集成到 OpenStack 中。



## 存储

### 块存储：cinder

