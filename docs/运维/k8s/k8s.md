
### kubeadm.org

阅读完该系列后，读者应当掌握以下技能：

- 使用 kubeadm 和 cilium 搭建一个简单的 K8S 集群
- 部署一个 Pod
- 创建 Persistent Volume 和 PVC 并挂载到 Pod
- 使用 NodePort 类型的 Service 暴露服务到所有节点的端口
- 使用 Deployment 部署 Nginx
- 使用 ClusterIP 类型的 Service 提供服务
- 使用 ConfigMap 挂载配置文件
- 使用 StatefulSet 部署 MariaDB
- 使用 Secret 保存数据库密码
- 使用 Provisioner 和 PVC 挂载容器存储
    - 了解 PV 的 Node Affinity
- 使用 taint 控制节点上的 Pod 调度
- 了解数据库的 replica 机制：数据库集群一般不使用共享文件系统，而是均使用 local storage，通过数据库自身 replica 机制进行同步，速度快很多，鲁棒性也更强一些
- 使用 Mayastor 部署 OpenEBS Replicated Storage
    - 创建 OpenEBS DiskPool 资源
    - 创建 Replica 的 StorageClass
    - 创建 PVC 并挂载到 Pod
- 使用 DaemonSet 在所有节点上部署 Pod
- 对节点进行 Drain 和 Cordon 操作
- 理解 PV 的四种 Access Modes


## 基础知识

!!! quote

    - 《Kubernetes 权威指南（第 5 版）》

在执行具体的操作前，我们先了解一些基本概念和工具。

### K8S 基本概念

Kubernetes 是一个完备的分布式系统支撑平台。Kubernetes 具有完备的集群管理能力，包括多层次的安全防护和准入机制、多租户应用支撑能力、透明的服务注册和服务发现机制、内建的智能负载均衡器、强大的故障发现和自我修复能力、服务滚动升级和在线扩容能力、可扩展的资源自动调度机制，以及多粒度的资源配额管理能力。同时，Kubernetes 提供了完善的管理工具，这些工具涵盖了包括开发、部署测试、运维监控在内的各个环节。因此，Kubernetes 是一个全新的基于容器技术的分布式架构解决方案，并且是一个一站式的完备的分布式系统开发和支撑平台。

#### Service

在 Kubernetes 中，Service 是分布式集群架构的核心。一个 Service 对象拥有如下关键特征。

- 拥有唯一指定的名称（比如 mysql-server）。
- 拥有一个虚拟 IP 地址（ClusterIP 地址）和端口号。
- 能够提供某种远程服务能力。
- 能够将客户端对服务的访问请求转发到一组容器应用上。

Service 的服务进程通常基于 Socket 通信方式对外提供服务，比如 Redis、Memcached、MySQL、Web Server，或者是实现了某个具体业务的特定 TCP Server 进程。虽然一个 Service 通常由多个相关的服务进程提供服务，每个服务进程都有一个独立的 Endpoint（IP+Port）访问点，但 Kubernetes 能够让我们通过 Service（ClusterIP+Service Port）连接指定的服务。有了 Kubernetes 内建的透明负载均衡和故障恢复机制，不管后端有多少个具体的服务进程，也不管某个服务进程是否由于发生故障而被重新部署到其他机器，都不会影响对服务的正常调用。更重要的是，这个 Service 本身一旦创建就不再变化，这意味着我们再也不用为 Kubernetes 集群中应用服务进程 IP 地址变来变去的问题头疼了。

#### Pod

容器提供了强大的隔离功能，所以我们有必要把为 Service 提供服务的这组进程放入容器中进行隔离。为此，Kubernetes 设计了 Pod 对象，将每个服务进程都包装到相应的 Pod 中，使其成为在 Pod 中运行的一个容器（Container）。为了建立 Service 和 Pod 间的关联关系，Kubernetes 首先给每个 Pod 都贴上一个标签（Label），比如给运行 MySQL 的 Pod 贴上 name=mysql 标签，给运行 PHP 的 Pod 贴上 name=php 标签，然后给相应的 Service 定义标签选择器（Label Selector），例如，MySQL Service 的标签选择器的选择条件为 name=mysql，意为该 Service 要作用于所有包含 name=mysql 标签的 Pod。这样一来，就巧妙解决了 Service 与 Pod 的关联问题。

这里先简单介绍 Pod 的概念。首先，Pod 运行在一个被称为节点（Node）的环境中，这个节点既可以是物理机，也可以是私有云或者公有云中的一个虚拟机，在一个节点上能够运行多个 Pod；其次，在每个 Pod 中都运行着一个特殊的被称为 Pause 的容器，其他容器则为业务容器，这些业务容器共享 Pause 容器的网络栈和 Volume 挂载卷，因此它们之间的通信和数据交换更为高效，在设计时我们可以充分利用这一特性将一组密切相关的服务进程放入同一个 Pod 中；最后，需要注意的是，并不是每个 Pod 和它里面运行的容器都能被映射到一个 Service 上，只有提供服务（无论是对内还是对外）的那组 Pod 才会被映射为一个服务。

#### Node

在集群管理方面，Kubernetes 将集群中的机器划分为一个 Master 和一些 Node。在 Master 上运行着集群管理相关的一些进程：kube-apiserver、kube-controller-manager 和 kube-scheduler，这些进程实现了整个集群的资源管理、Pod 调度、弹性伸缩、安全控制、系统监控和纠错等管理功能，并且都是自动完成的。Node 作为集群中的工作节点，其上运行着真正的应用程序。在 Node 上，Kubernetes 管理的最小运行单元是 Pod。在 Node 上运行着 Kubernetes 的 kubelet、kube-proxy 服务进程，这些服务进程负责 Pod 的创建、启动、监控、重启、销毁，以及实现软件模式的负载均衡器。

#### Deployment

这里讲一讲传统的 IT 系统中服务扩容和服务升级这两个难题，以及 Kubernetes 所提供的全新解决思路。服务的扩容涉及资源分配（选择哪个节点进行扩容）、实例部署和启动等环节。在一个复杂的业务系统中，这两个难题基本上要靠人工一步步操作才能得以解决，费时费力又难以保证实施质量。

在 Kubernetes 集群中，只需为需要扩容的 Service 关联的 Pod 创建一个 Deployment 对象，服务扩容以至服务升级等令人头疼的问题就都迎刃而解了。在一个 Deployment 定义文件中包括以下 3 个关键信息。

- 目标 Pod 的定义。
- 目标 Pod 需要运行的副本数量（Replicas）。
- 要监控的目标 Pod 的标签。

在创建好 Deployment 之后，Kubernetes 会根据这一定义创建符合要求的 Pod，并且通过在 Deployment 中定义的 Label 筛选出对应的 Pod 实例并实时监控其状态和数量。如果实例数量少于定义的副本数量，则会根据在 Deployment 对象中定义的 Pod 模板创建一个新的 Pod，然后将此 Pod 调度到合适的 Node 上启动运行，直到 Pod 实例的数量达到预定目标。这个过程完全是自动化的，无须人工干预。有了 Deployment，服务扩容就变成一个纯粹的简单数字游戏了，只需修改 Deployment 中的副本数量即可。后续的服务升级也将通过修改 Deployment 来自动完成。

### 两个节点服务

#### kubelet

```ini title="/usr/lib/systemd/system/kubelet.service.d/10-kubeadm.conf"
[Service]
Environment="KUBELET_KUBECONFIG_ARGS=--bootstrap-kubeconfig=/etc/kubernetes/bootstrap-kubelet.conf --kubeconfig=/etc/kubernetes/kubelet.conf"
Environment="KUBELET_CONFIG_ARGS=--config=/var/lib/kubelet/config.yaml"
EnvironmentFile=-/var/lib/kubelet/kubeadm-flags.env
EnvironmentFile=-/etc/default/kubelet
ExecStart=
ExecStart=/usr/bin/kubelet $KUBELET_KUBECONFIG_ARGS $KUBELET_CONFIG_ARGS $KUBELET_KUBEADM_ARGS $KUBELET_EXTRA_ARGS
```

展开：

```text
/usr/bin/kubelet \
    --bootstrap-kubeconfig=/etc/kubernetes/bootstrap-kubelet.conf \
    --kubeconfig=/etc/kubernetes/kubelet.conf \
    --config=/var/lib/kubelet/config.yaml
```

#### kube-proxy

为了支持集群的水平扩展和高可用性，Kubernetes 抽象出了 Service 的概念。Service 是对一组 Pod 的抽象，它会根据访问策略（如负载均衡策略）来访问这组 Pod。

Kubernetes 在创建服务时会为服务分配一个虚拟 IP 地址，客户端通过访问这个虚拟 IP 地址来访问服务，服务则负责将请求转发到后端的 Pod 上。这其实就是一个反向代理，但与普通的反向代理有一些不同：它的 IP 地址是虚拟，若想从外面访问，则还需要一些技巧；它的部署和启停是由 Kubernetes 统一自动管理的。

在很多情况下，Service 只是一个概念，而真正将 Service 的作用落实的是它背后的 kube-proxy 服务进程。只有理解了 kube-proxy 的原理和机制，我们才能真正理解 Service 的实现逻辑。

- **第一代 Proxy**：数据面代理

    我们知道，在 Kubernetes 集群的每个 Node 上都会运行一个 kube-proxy 服务进程，我们可以把这个进程看作 Service 的透明代理兼负载均衡器，其核心功能是将到某个 Service 的访问请求转发到后端的多个 Pod 实例上。

    起初，kube-proxy 进程是一个真实的 TCP/UDP 代理，类似 HA Proxy，负责转发从 Service 到 Pod 的访问流量，这被称为 userspace（用户空间代理）模式。当某个客户端 Pod 以 ClusterIP 地址访问某个 Service 时，这个流量就被 Pod 所在 Node 的 iptables 转发给 kube-proxy 进程，然后由 kube-proxy 建立起到后端 Pod 的 TCP/UDP 连接，再将请求转发到某个后端 Pod 上，并在这个过程中实现负载均衡功能。

    此外，Service 的 ClusterIP 与 NodePort 等概念是 kube-proxy 服务通过 iptables 的 NAT 转换实现的，kube-proxy 在运行过程中动态创建与 Service 相关的 iptables 规则，这些规则实现了将访问服务（ClusterIP 或 NodePort）的请求负载分发到后端 Pod 的功能。由于 iptables 机制针对的是本地的 kube-proxy 端口，所以在每个 Node 上都要运行 kube-proxy 组件，这样一来，在 Kubernetes 集群内部，我们可以在任意 Node 上发起对 Service 的访问请求。综上所述，由于 kube-proxy 的作用，客户端在 Service 调用过程中无须关心后端有几个 Pod，中间过程的通信、负载均衡及故障恢复都是透明的。

- **第二代 Proxy**：趋向控制面

    从 1.2 版本开始，Kubernetes 将 iptables 作为 kube-proxy 的默认模式。iptables 模式下的第二代 kube-proxy 进程不再起到数据层面的 Proxy 的作用，Client 向 Service 的请求流量通过 iptables 的 NAT 机制直接发送到目标 Pod，不经过 kube-proxy 进程的转发，kube-proxy 进程只承担了控制层面的功能，即通过 API Server 的 Watch 接口实时跟踪 Service 与 Endpoint 的变更信息，并更新 Node 节点上相应的 iptables 规则。

    根据 Kubernetes 的网络模型，一个 Node 上的 Pod 与其他 Node 上的 Pod 应该能够直接建立双向的 TCP/IP 通信通道，所以如果直接修改 iptables 规则，则也可以实现 kube-proxy 的功能，只不过后者更加高端，因为是全自动模式的。与第一代的 userspace 模式相比，iptables 模式完全工作在内核态，不用再经过用户态的 kube-proxy 中转，因而性能更强。

- **第三代 Proxy**：IPVS

    第二代的 iptables 模式实现起来虽然简单，性能也提升很多，但存在固有缺陷：在集群中的 Service 和 Pod 大量增加以后，每个 Node 节点上 iptables 中的规则会急速膨胀，导致网络性能显著下降，在某些极端情况下甚至会出现规则丢失的情况，并且这种故障难以重现与排查。于是 Kubernetes 从 1.8 版本开始引入第三代的 IPVS（IP Virtual Server）模式。

    iptables 与 IPVS 虽然都是基于 Netfilter 实现的，但因为定位不同，二者有着本质的差别：iptables 是为防火墙设计的；IPVS 专门用于高性能负载均衡，并使用更高效的数据结构（哈希表），允许几乎无限的规模扩张，因此被 kube-proxy 采纳为第三代模式。

    与 iptables 相比，IPVS 拥有以下明显优势：

    - 为大型集群提供了更好的可扩展性和性能；
    - 支持比 iptables 更复杂的复制均衡算法（最小负载、最少连接、加权等）；
    - 支持服务器健康检查和连接重试等功能；
    - 可以动态修改 ipset 的集合，即使 iptables 的规则正在使用这个集合。

    由于 IPVS 无法提供包过滤、airpin-masquerade tricks（地址伪装）、SNAT 等功能，因此在某些场景（如 NodePort 的实现）下还要与 iptables 搭配使用。在 IPVS 模式下，kube-proxy 又做了重要的升级，即使用 iptables 的扩展 ipset，而不是直接调用 iptables 来生成规则链。

    iptables 规则链是一个线性数据结构，ipset 则引入了带索引的数据结构，因此当规则很多时，也可以高效地查找和匹配。我们可以将 ipset 简单理解为一个 IP（段）的集合，这个集合的内容可以是 IP 地址、IP 网段、端口等，iptables 可以直接添加规则对这个“可变的集合”进行操作，这样做的好处在于大大减少了 iptables 规则的数量，从而减少了性能损耗。假设要禁止上万个 IP 访问我们的服务器，则用 iptables 的话，就需要一条一条地添加规则，会在 iptables 中生成大量的规则；但是用 ipset 的话，只需将相关的 IP 地址（网段）加入 ipset 集合中即可，这样只需设置少量的 iptables 规则即可实现目标。

### 两个重要工具

#### kubeadm

```shell
kubeadm config print init-defaults
kubeadm config print join-defaults
```

#### kubectl

### 容器技术

!!! quote

    - [技术干货｜Docker 和 Containerd 的区别，看这一篇就够了 - 知乎](https://zhuanlan.zhihu.com/p/494054143)
    - [The Containerization Tech Stack | Medium](https://medium.com/@noah_h/the-containerization-tech-stack-3ac4390d47bf)

#### 容器运行时

K8S 中的 kubelet 负责节点上所有 Pod 的全生命周期管理，其中就包括相关容器的创建和销毁这种基本操作。容器的创建和销毁等操作的代码不属于 Kubernetes 的代码范畴，比如目前流行的 Docker 容器引擎就属于 Docker 公司的产品，所以 kubelet 需要通过某种进程间的调用方式如 gRPC 来实现与 Docker 容器引擎之间的调用控制功能。在说明其原理和工作机制之前，我们首先要理解一个重要的概念——Container Runtime（容器运行时）。

<figure>
    ![](https://miro.medium.com/v2/resize:fit:1100/format:webp/1*J-IBJC-6qg5AVJuLOrvekg.png)
    <figcaption>
        容器技术栈<br>
        <small>来源：[The Containerization Tech Stack | Medium](https://medium.com/@noah_h/the-containerization-tech-stack-3ac4390d47bf)</small>
    </figcaption>
</figure>

“容器”这个概念是早于 Docker 出现的，容器技术最早来自 Linux，所以又被称为 Linux Container。

- **LXC**：LXC 项目是一个 Linux 容器的工具集，也是真正意义上的一个 Container Runtime，它的作用就是将用户的进程包装成一个 Linux 容器并启动运行。
- **runc**：Docker 一开始时就使用了 LXC 项目代码作为 Container Runtime 来运行容器，但从 0.9 版本开始被 Docker 公司自研的新一代容器运行时 Libcontainer 所取代，再后来，Libcontainer 的代码被改名为 runc，被 Docker 公司捐赠给了 OCI 组织，成为 OCI 容器运行时规范的第 1 个标准参考实现。

所以，LXC 与 runC 其实都可被看作开源的 Container Runtime，但它们都属于低级别的容器运行时（low-level container runtimes），因为它们不涉及容器运行时所依赖的镜像操作功能，比如拉取镜像，也没有对外提远程供编程接口以方便其他应用集成，所以又有了后来的高级别容器运行时（high-level container runtimes），其中最知名的就是 Docker 公司开源的 containerd。

- **containerd**：containerd 被设计成嵌入一个更大的系统如 Kubernetes 中使用，而不是直接由开发人员或终端用户使用，containerd 底层驱动 runc 来实现底层的容器运行时，对外则提供了镜像拉取及基于 gRPC 接口的容器 CRUD 封装接口。发展至今，containerd 已经从 Docker 里的一个内部组件，变成一个流行的、工业级的开源容器运行时，已经支持容器镜像的获取和存储、容器的执行和管理、存储和网络等相关功能。在 containerd 和 runC 成为标准化容器服务的基石后，上层应用就可以直接建立在 containerd 和 runC 之上了。如果我们只希望用一个纯粹的、稳定性更好、性能更优的容器运行时，就可以直接使用 containerd 而无须再依赖 Docker 了。

除了 containerd，还有类似的其他一些高层容器运行时也都在 runC 的基础上发展而来，目前比较流行的有红帽开源的 CRI-O、openEuler 社区开源的 iSula 等。这些 Container Runtime 还有另外一个共同特点，即都实现了 Kubernetes 提出的 CRI 接口规范（Container Runtime Interface），可以直接接入 Kubernetes 中。CRI 顾名思义，就是容器运行时接口规范，这个规范也是 Kubernetes 顺应容器技术标准化发展潮流的一个重要历史产物，早在 Kubernetes 1.5 版本中就引入了 CRI 接口规范。引入了 CRI 接口规范后，kubelet 就可以通过 CRI 插件来实现容器的全生命周期控制了，不同厂家的 Container Runtime 只需实现对应的 CRI 插件代码即可，Kubernetes 无须重新编译就可以使用更多的容器运行时。

#### CRI 接口

CRI 接口规范主要定义了两个 gRPC 接口服务：ImageService 和 RuntimeService。其中：

- **ImageService** 提供了从仓库拉取镜像、查看和移除镜像的功能
- **RuntimeService** 则负责实现 Pod 和容器的生命周期管理，以及与容器的交互（exec/attach/port-forward）

我们知道，Pod 由一组应用容器组成，其中包含共有的环境和资源约束，这个环境在 CRI 里被称为 **Pod Sandbox**。Container Runtime 可以根据自己的内部实现来解释和实现自己的 Pod Sandbox，比如对于 Hypervisor 这种容器运行时引擎，会把 PodSandbox 具体实现为一个虚拟机。所以，RuntimeService 服务接口除了提供了针对 Container 的相关操作，也提供了针对 Pod Sandbox 的相关操作以供 kubelet 调用。在启动 Pod 之前，kubelet 调用 RuntimeService.RunPodSandbox 来创建 Pod 环境，这一过程也包括为 Pod 设置网络资源（分配 IP 等操作），Pod Sandbox 在被激活之后，就可以独立地创建、启动、停止和删除用户业务相关的 Container 了，当 Pod 销毁时，kubelet 会在停止和删除 Pod Sandbox 之前首先停止和删除其中的 Container。
### 容器网络模型

!!! quote

    - [Container Networking: What You Should Know](https://www.tigera.io/learn/guides/kubernetes-networking/container-networking/)
    - [让容器通信变得简单：深度解析 Containerd 中的 CNI 插件](https://kubesphere.io/zh/blogs/containerd-cni/)
    - [richtman.au • Debugging a Kubernetes CNI plugin with Containerd](https://www.richtman.au/blog/debugging-k8s-cni/)

容器运行时并不管理容器的网络，于是容器网络领域诞生了多种模型和实现方式。

#### CNI

以 Docker 阵营的 CNM 为例，当 Docker 通过 containerd 创建容器后，Docker 接管网络配置的工作，调用 CNM 来为容器创建网络接口、分配 IP 地址等。

K8S 和 containerd 支持 [CNI](https://github.com/containernetworking/cni) 规范的网络插件，Cilium 是其中一个流行的实现。

#### containerd 与 CNI

containerd 与 CNI 有关的配置如下：

```toml title="/etc/containerd/config.toml"
[plugins."io.containerd.grpc.v1.cri".cni]
  bin_dir = "/opt/cni/bin"
  conf_dir = "/etc/cni/net.d"
```

- 需确保 CNI 插件、配置文件放置在对应的位置，这可能随发行版、安装方式不同而不同
- 配置文件规范见 [CNI Spec#Network configuration format](https://www.cni.dev/docs/spec/#section-1-network-configuration-format)

containerd 会运行 CNI，可以通过调整其日志登记拿到 CNI 的返回结果等：

```toml title="/etc/containerd/config.toml"
[plugins."io.containerd.grpc.v1.cri".cni]
    [plugins."io.containerd.grpc.v1.cri".cni.logging]
        level = "debug"
        file = "/var/log/cni.log"
        max_size = "10MiB"
```

#### K8S 与 CNI

!!! info "废弃的选项"

    部分老旧的文档描述 `kubelet` 通过 `--cni-conf-dir` 和 `--cni-bin-dir` 选项配置 CNI，但这些选项已废弃，CNI 不属于 K8S 配置的范畴，由 containerd 负责。见上文 containerd 配置中 CNI 相关部分的 `bin_dir` 和 `conf_dir`。

#### Cilium

<https://docs.siderolabs.com/kubernetes-guides/cni/deploying-cilium#installation-using-helm>

### 四个控制面服务

!!! quote

    - 《Kubernetes 权威指南（第 5 版）》第五章 核心组件的运行机制

#### etcd

#### kube-apiserver

#### kube-scheduler

#### kube-controller-manager

一般来说，智能系统和自动系统通常会通过一个“操作系统”不断修正系统的工作状态。在 Kubernetes 集群中，每个 Controller 都是这样的一个“操作系统”，它们通过 API Server 提供的（List-Watch）接口实时监控集群中特定资源的状态变化，当发生各种故障导致某资源对象的状态变化时，Controller 会尝试将其状态调整为期望的状态。比如当某个 Node 意外宕机时，Node Controller 会及时发现此故障并执行自动化修复流程，确保集群始终处于预期的工作状态下。Controller Manager 是 Kubernetes 中各种操作系统的管理者，是集群内部的管理控制中心，也是 Kubernetes 自动化功能的核心。

Controller Manager 内部包含 Replication Controller、Node Controller、ResourceQuota Controller、Namespace Controller、ServiceAccount Controller、Token Controller、Service Controller、Endpoint Controller、Deployment Controller、Router Controller、Volume Controller 等各种资源对象的控制器，每种 Controller 都负责一种特定资源的控制流程，而 Controller Manager 正是这些 Controller 的核心管理者。

- **副本调度控制器**：在 Kubernetes 中存在两个功能相似的副本控制器：Replication Controller 及 Deployment Controller。为了区分 Controller Manager 中的 Replication Controller（副本控制器）和资源对象 Replication Controller，我们将资源对象 Replication Controller 简写为 RC，而 Replication Controller 是指副本控制器，以便于后续分析。

    - **Replication Controller**：Replication Controller 的核心作用是确保集群中某个 RC 关联的 Pod 副本数量在任何时候都保持预设值。如果发现 Pod 的副本数量超过预设值，则 Replication Controller 会销毁一些 Pod 副本；反之，Replication Controller 会自动创建新的 Pod 副本，直到符合条件的 Pod 副本数量达到预设值。需要注意：只有当 Pod 的重启策略是 Always 时（RestartPolicy=Always），Replication Controller 才会管理该 Pod 的操作（例如创建、销毁、重启等）。在通常情况下，Pod 对象被成功创建后都不会消失，唯一的例外是 Pod 处于 succeeded 或 failed 状态的时间过长（超时参数由系统设定），此时该 Pod 会被系统自动回收，管理该 Pod 的副本控制器将在其他工作节点上重新创建、运行该 Pod 副本。

        RC 中的 Pod 模板就像一个模具，模具制作出来的东西一旦离开模具，它们之间就再也没关系了。同样，一旦 Pod 被创建完毕，无论模板如何变化，甚至换成一个新的模板，也不会影响到已经创建的 Pod 了。此外，Pod 可以通过修改它的标签来脱离 RC 的管控，该方法可以用于将 Pod 从集群中迁移、数据修复等的调试。对于被迁移的 Pod 副本，RC 会自动创建一个新的副本替换被迁移的副本。

    - **Deployment Controller**：随着 Kubernetes 的不断升级，旧的 RC 已不能满足需求，所以有了 Deployment。Deployment 可被视为 RC 的替代者，RC 及对应的 Replication Controller 已不再升级、维护，Deployment 及对应的 Deployment Controller 则不断更新、升级新特性。Deployment Controller 在工作过程中实际上是在控制两类相关的资源对象：Deployment 及 ReplicaSet。在我们创建 Deployment 资源对象之后，Deployment Controller 也默默创建了对应的 ReplicaSet，Deployment 的滚动升级也是 Deployment Controller 通过自动创建新的 ReplicaSet 来支持的。

        下面总结 Deployment Controller 的作用，如下所述。

        1. 确保在当前集群中有且仅有 N 个 Pod 实例，N 是在 RC 中定义的 Pod 副本数量。
        1. 通过调整 spec.replicas 属性的值来实现系统扩容或者缩容。
        1. 通过改变 Pod 模板（主要是镜像版本）来实现系统的滚动升级。

        最后总结 Deployment Controller 的典型使用场景，如下所述。

        1. 重新调度（Rescheduling）。如前面所述，不管想运行 1 个副本还是 1000 个副本，副本控制器都能确保指定数量的副本存在于集群中，即使发生节点故障或 Pod 副本被终止运行等意外状况。
        1. 弹性伸缩（Scaling）。手动或者通过自动扩容代理修改副本控制器 spec.replicas 属性的值，非常容易实现增加或减少副本的数量。
        1. 滚动更新（Rolling Updates）。副本控制器被设计成通过逐个替换 Pod 来辅助服务的滚动更新。

- **节点控制器**：

    - **节点上报**：kubelet 进程在启动时通过 API Server 注册自身节点信息，并定时向 API Server 汇报状态信息，API Server 在接收到这些信息后，会将这些信息更新到 etcd 中。在 etcd 中存储的节点信息包括节点健康状况、节点资源、节点名称、节点地址信息、操作系统版本、Docker 版本、kubelet 版本等。节点健康状况包含就绪（True）、未就绪（False）、未知（Unknown）三种。
    - **Node Controller 的工作流程**：

        1. Controller Manager 在启动时如果设置了 `--cluster-cidr` 参数，那么为每个没有设置 `Spec.PodCIDR` 的 Node 都生成一个 CIDR 地址，并用该 CIDR 地址设置节点的`Spec.PodCIDR`属性，这样做的目的是防止不同节点的 CIDR 地址发生冲突。
        1. 逐个读取 Node 信息，多次尝试修改 nodeStatusMap 中的节点状态信息，将该节点信息和在 Node Controller 的 `nodeStatusMap` 中保存的节点信息做比较。如果判断出没有收到 kubelet 发送的节点信息、第 1 次收到节点 kubelet 发送的节点信息，或在该处理过程中节点状态变成非健康状态，则在 `nodeStatusMap` 中保存该节点的状态信息，并用 Node Controller 所在节点的系统时间作为探测时间和节点状态变化时间。如果判断出在指定时间内收到新的节点信息，且节点状态发生变化，则在 `nodeStatusMap` 中保存该节点的状态信息，并用 Node Controller 所在节点的系统时间作为探测时间和节点状态变化时间。如果判断出在指定时间内收到新的节点信息，但节点状态没发生变化，则在 `nodeStatusMap` 中保存该节点的状态信息，并用 Node Controller 所在节点的系统时间作为探测时间，将上次节点信息中的节点状态变化时间作为该节点的状态变化时间。如果判断出在某段时间（gracePeriod）内没有收到节点状态信息，则设置节点状态为“未知”，并且通过 API Server 保存节点状态。
        1. 逐个读取节点信息，如果节点状态变为非就绪状态，则将节点加入待删除队列，否则将节点从该队列中删除。如果节点状态为非就绪状态，且系统指定了 Cloud Provider，则 Node Controller 调用 Cloud Provider 查看节点，若发现节点故障，则删除 etcd 中的节点信息，并删除与该节点相关的 Pod 等资源的信息。

- **Endpoint 控制器**：

    - **Endpoints**：表示一个 Service 对应的所有 Pod 副本的访问地址，Endpoints Controller 就是负责生成和维护所有 Endpoints 对象的控制器。
    - **Endpoints Controller**：负责监听 Service 和对应的 Pod 副本的变化，如果监测到 Service 被删除，则删除和该 Service 同名的 Endpoints 对象。如果监测到新的 Service 被创建或者修改，则根据该 Service 信息获得相关的 Pod 列表，然后创建或者更新 Service 对应的 Endpoints 对象。如果监测到 Pod 的事件，则更新它所对应的 Service 的 Endpoints 对象（增加、删除或者修改对应的 Endpoint 条目）。

    那么，Endpoints 对象是在哪里被使用的呢？答案是每个 Node 上的 kube-proxy 进程，kube-proxy 进程获取每个 Service 的 Endpoints，实现了 Service 的负载均衡功能。

- **服务控制器**：Service Controller 其实是 Kubernetes 集群与外部的云平台之间的一个接口控制器。Service Controller 监听 Service 的变化，如果该 Service 是一个 LoadBalancer 类型的 Service（externalLoadBalancers=true），则 Service Controller 确保该 Service 对应的 LoadBalancer 实例在外部的云平台上被相应地创建、删除及更新路由转发表（根据 Endpoints 的条目）。

### 调试和错误排查

## 部署集群

## 部署服务

### Persistent Volume



---

待整理

## Tutorials

目标：掌握在 K8S 集群上部署、扩展、更新、调试容器应用的技能。

### Learn Kubernetes Basics

基本概念：

- 每节点一个 kubelet，用于管理节点、与控制面通信
- 控制面暴露 K8S API，供用户和 kubelet 使用

Minikube：

- 不建议在 Windows 上用 Minikube，Hyper-V 支持不好，没配成功代理，服务起不来
- Pod：一组容器，统一管理、网络
- Deployment：负责维护 Pod 的状态、升级等
- Service：负责暴露 Deployment，负载均衡

    ```text
    get pods
    logs <pod>
    top pods

    create deployment
    get deployment
    expose deployment
    delete deployment

    get services
    delete service

    get events
    config view
    ```

kubectl：

- 默认配置 `~/.kube/config`
- 命令格式 `<action> <resource>`：

    重要的 action：

    ```text
    describe
    ```

- 进入 Pod：

    首先拿到 Pod 名：

    ```shell
    export POD_NAME="$(kubectl get pods -o go-template --template '{{range .items}}{{.metadata.name}}{{"\n"}}{{end}}')"
    ```

    Proxy：本地开一个代理能直接访问 Pod 的 API，例：

    ```shell
    curl http://localhost:8001/api/v1/namespaces/default/pods/$POD_NAME:8080/proxy/
    ```

    Exec：直接在 Pod 执行命令，例：

    ```shell
    kubectl exec -ti $POD_NAME -- bash
    ```

Service 相关：

- K8S 集群内的 Pods IP 独立
- Service 在 Pods 上构建一层抽象，选入一组 Pods（通常通过 Label）并定义 policy，从而提供统一的访问入口、负载均衡、服务发现等
- Label 就是附加在 K8S 对象上的键值对

Scale 相关：

- Deployment 创建 ReplicaSet

    ```text
    get rs
    scale deployment/<name> --replicas=<num>
    ```

- `[DEPLOYMENT-NAME]-[RANDOM-STRING]`

## Getting Started

### Installing kubeadm

开启 IP 转发，关闭 swap：

```shell
cat <<EOF | sudo tee /etc/sysctl.d/k8s.conf
net.ipv4.ip_forward = 1
EOF
sudo sysctl --system
sudo swapoff -a
```

容器运行时：containerd

!!! quote

    - [Container Runtimes | Kubernetes](https://kubernetes.io/docs/setup/production-environment/container-runtimes/)
    - [containerd/docs/getting-started.md at main · containerd/containerd](https://github.com/containerd/containerd/blob/main/docs/getting-started.md)

K8S 需要一个容器运行时，默认使用 containerd。如果已安装 Docker，则 containerd 已安装。

```shell
apt install docker.io docker-buildx docker-compose
```

在使用 Systemd 的系统上，cgroup driver 需要配置为 systemd：

```toml title="/etc/containerd/config.toml"
# [plugins."io.containerd.grpc.v1.cri"]
#   sandbox_image = "registry.k8s.io/pause:3.10"
[plugins.'io.containerd.cri.v1.runtime'.containerd.runtimes.runc]
  [plugins.'io.containerd.cri.v1.runtime'.containerd.runtimes.runc.options]
    SystemdCgroup = true
```

### Creating a cluster with kubeadm

#### 镜像

K8S 需要通过容器运行时拉取镜像。对于 containerd，其守护进程支持使用 `HTTPS_PROXY` 环境变量配置代理。因此一般选择修改 systemd unit 文件：

```shell
root@M604:~# cat /etc/systemd/system/containerd.service.d/override.conf
[Service]
Environment="HTTP_PROXY="
Environment="HTTPS_PROXY="
```

尝试拉取镜像：

```shell
kubeadm config images pull
```

#### 网络规划

K8S 划分三个网络区域，要求区域网段不重叠：

- Service 网络
- Pod 网络
- Node 网络

K8S Pod 依赖 CNI 进行通信，需要先安装 CNI，才能启动 CoreDNS 等服务。以 cilium 为例：

```shell
```

`kubeadm init` 做的事情：

- 检查
- 建立 CA，为各组件生成证书
- 在 `/etc/kubernetes/` 下生成文件供 kubelet 使用
- 在 `/etc/kubernetes/manifests/` 下生成静态 Pod 清单文件，kubelet 根据它拉起控制面服务
- 给控制面服务加 label
- 生成添加新节点的 token
- 安装 CoreDNS 和 kube-proxy

---

??? error "sock rpc error"

    执行 `kubeadm init` 时，如果遇到

    ```text
    failed to create new CRI runtime service: validate service connection: validate CRI v1 runtime API for endpoint "unix:///var/run/containerd/containerd.sock": rpc error: code = Unimplemented desc = unknown service runtime.v1.RuntimeService[preflight] If you know what you are doing, you can make a check non-fatal with `--ignore-preflight-errors=...`
    ```

    这类错误，一般是 containerd 没有配置好。按照 [Container Runtimes | Kubernetes](https://kubernetes.io/docs/setup/production-environment/container-runtimes/#containerd-systemd) 配置，直到 `cri` 插件正常：

    ```bash
    $ ctr plugin ls | grep cri
    TYPE                   ID   PLATFORM    STATUS
    io.containerd.grpc.v1  cri  linux/amd64 ok
    ```

## 基础设置

### kubelet
