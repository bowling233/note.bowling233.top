# 构建

## 获取源码

- Git

    ```bash
    git clone git@github.com:torvalds/linux.git
    cd linux
    ```

- Debian：[Chapter 4. Common kernel-related tasks](https://www.debian.org/doc/manuals/debian-kernel-handbook/ch-common-tasks.html)

    ```bash
    apt-get build-dep linux
    apt-get install linux-source build-essential fakeroot devscripts rsync git linux-headers-amd64
    tar xaf /usr/src/linux-source-*.tar.xz
    cd linux-source-*
    ```

- RedHat：[Building a Custom Kernel :: Fedora Docs](https://docs.fedoraproject.org/en-US/quick-docs/kernel-build-custom/)

    ```bash
    dnf builddep kernel
    dnf download --source kernel
    rpm2cpio kernel*.rpm | cpio -idmv 'linux-*.tar.xz'
    tar xf linux-*
    cd linux-*
    ```

    若要在 `/usr/src` 下放置源码：

    ```bash
    dnf install kernel-devel
    cd /usr/src/kernels/*
    ```

## 配置

```bash
yes "" | make oldconfig
make -j
```

## 内核模块

如果只需要特定的内核模块，参考 [Building External Modules¶](https://docs.kernel.org/kbuild/modules.html)

```shell
cd drivers/net/bonding
make -C /lib/modules/`uname -r`/build M=$PWD
make -C /lib/modules/`uname -r`/build M=$PWD modules_install
```

此时模块会被安装到 `/lib/modules/$(uname -r)/update/*.ko`。直接用其覆盖目标模块即可。
