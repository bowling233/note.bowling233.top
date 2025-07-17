# K8S

初次部署 K8S 时，请逐篇阅读 [Container Runtimes | Kubernetes](https://kubernetes.io/docs/setup/)，按照流程配置。

下文按照 kubeadm、kubelet 的顺序进行配置。

### 基础设置

开启 IP 转发，关闭 swap：

```bash
cat <<EOF | sudo tee /etc/sysctl.d/k8s.conf
net.ipv4.ip_forward = 1
EOF
sudo sysctl --system
sudo swapoff -a
```

### 容器运行时：containerd

!!! quote

    - [Container Runtimes | Kubernetes](https://kubernetes.io/docs/setup/production-environment/container-runtimes/)
    - [containerd/docs/getting-started.md at main · containerd/containerd](https://github.com/containerd/containerd/blob/main/docs/getting-started.md)

K8S 需要一个容器运行时，默认使用 containerd。如果已安装 Docker，则 containerd 已安装。

```toml
# disabled_plugins = ["cri"]

# K8S
[plugins."io.containerd.grpc.v1.cri"]
  sandbox_image = "registry.k8s.io/pause:3.10"
[plugins."io.containerd.grpc.v1.cri".containerd.runtimes.runc]
  [plugins."io.containerd.grpc.v1.cri".containerd.runtimes.runc.options]
    SystemdCgroup = true
```

- containerd 和 Docker 类似，由 daemon 拉取镜像，因此需要修改 systemd unit 文件配置代理。

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

### kubelet
