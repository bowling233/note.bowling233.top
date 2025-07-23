# Apptainer/Singularity

!!! quote

    - [Apptainer - Portable, Reproducible Containers](https://apptainer.org)
    - [apptainer/apptainer: Apptainer: Application containers for Linux](https://github.com/apptainer/apptainer)

2021 年，[Singularity 的主要开发者团队将项目捐赠给了 Linux 基金会](https://apptainer.org/news/community-announcement-20211130)，并更名为 Apptainer。本文将基于 Apptainer 1.4.1 文档进行介绍。

## 安装

Apptainer 默认不以 suid 的方式执行，而是**以运行命令的用户身份创建命名空间**。如需使用 suid 方式，需额外安装 apptainer-suid 包。

```text
# 源码
apptainer-1.4.1.tar.gz
apptainer-1.4.1-1.src.rpm
# 非 SUID 版
apptainer-1.4.1-1.x86_64.rpm
apptainer_1.4.1_amd64.deb
apptainer-dbgsym_1.4.1_amd64.deb
apptainer-debuginfo-1.4.1-1.x86_64.rpm
# SUID 版
apptainer-suid-1.4.1-1.x86_64.rpm
apptainer-suid_1.4.1_amd64.deb
apptainer-suid-dbgsym_1.4.1_amd64.deb
apptainer-suid-debuginfo-1.4.1-1.x86_64.rpm
```

!!! note "版本兼容性提醒"

    在一些过于老旧的发行版上，只能使用特定的 Apptainer 版本。

    - Ubuntu 18：无 libfuse3-3，只有 libfuse2，只能使用 Apptainer v1.2.0 及以前。

## 镜像构建

!!! quote

    - [Build a Container — Apptainer User Guide 1.4 documentation](https://apptainer.org/docs/user/latest/build_a_container.html)
    - [Definition Files — Apptainer User Guide 1.4 documentation](https://apptainer.org/docs/user/latest/definition_files.html)

    使用 `spack containerize` 生成定义文件，见 [Spack]()。

`.def` 文件指示如何构建镜像，分为以下两部分。这两部分可以重复多次，每次重复为一个 stage。

- Header：描述基础系统

    ```text
    Bootstrap: docker
    From: debian:10

    Bootstrap: dnf
    OSVersion: 9
    MirrorURL: 
    ```

- Sections：具体的构建步骤

    ```text
    %setup 构建前执行，比如设置挂载点 mkdir
    %files 拷贝文件
        <src> [dst]
    %post 主要内容
        运行时才能确定的环境变量可以写到 $APPTAINER_ENVIRONMENT 中，例：
            ENVIRONMENT >> $APPTAINER_ENVIRONMENT
        且优先级比 %environment 高
    %environment 容器运行时的环境变量，语法与 bashrc 相似
        export FOO=${FOO:-'default'}
    %runscript
        容器运行时执行的命令（相当于 Docker Entrypoint）
    ```

    - 命令默认以 `/bin/sh` 执行，这和 Bash 语法有些区别，比如 Python venv 会遇到问题。

可以构建下面几种格式：

- SIF 镜像：默认是 immutable 的，无法执行安装软件等操作。
- sandbox 文件夹：方便先在文件夹测试修改，然后打包为 SIF 镜像。

    ```bash
    apptainer build --sandbox <DIR> <IMAGE>
    apptainer <exec|shell|run> --writeable <DIR>
    apptainer build <SIF> <DIR>
    ```

    此外可以使用 `--update` 起到类似 Docker build cache 的作用。将跳过 Header，并覆盖现有文件。

## 运行

```bash
apptainer pull docker://nvcr.io/nvidia/hpc-benchmarks:24.03
apptainer shell --nv ./hpc-benchmarks_24.03.sif
apptainer --debug run docker://alpine
apptainer inspect \
    --lables \
    --deffile
```

非交互式：

```bash
# 参数说明
apptainer <run|exec> \
    -it --rm \
    --nv \
    --pwd <DIR> \
    --bind src[:dest[:opts]] \
    --no-mount bind-paths \
    # 环境：默认继承宿主的环境变量（除了 PATH 和 LD 等），会被容器内覆盖
    --cleanenv \
    --env "NAME=VALUE,..." \
    --env-file <file> \
    # 网络
    --dns <DNS_SERVER> \
    --hostname <HOSTNAME> \
    YOUR_APPLICATION \
        YOUR_APPLICATION_OPTIONS

# 示例
apptainer run \
    --nv \
    --bind /src:/dest:ro \
    ./hpc-benchmarks_24.03.sif \
    nsys profile \
        --trace=cuda,openmp,nvtx,cublas \
        --sample=system-wide \
        /workspace/hpcg.sh \
            --dat ~/hpcg/hpcg-nsys.dat
```

需要注意，Apptainer 会[自动挂载](https://apptainer.org/docs/user/1.4/bind_paths_and_mounts.html)下列目录，可以使用 `--no-mount bind-paths` 关闭：

```text
$HOME, $PWD
/dev, /proc, /sys
/etc/hosts, /etc/localtime, /etc/resolv.conf, ...
/tmp, /var/tmp
```

### MPI

!!! quote

    - [Running MPI parallel jobs using Apptainer containers – Reproducible computational environments using containers: Introduction to Apptainer](https://carpentries-incubator.github.io/apptainer-introduction/08-apptainer-mpi/index.html)

!!! note "原理"

    在 Apptainer 容器中运行 MPI 代码时，我们使用宿主系统的 MPI 安装来启动容器，而不是容器内的 MPI 可执行文件（即不使用 `apptainer exec mpirun -np <numprocs> /path/to/my/executable`）。宿主系统的 MPI 为每个 MPI 进程启动一个容器实例。

如果 MPI 实现没有 Apptainer 支持，这会为每个进程启动单独的容器实例，在大量进程时可能产生开销。支持 Apptainer 的 MPI 实现可以减少这种开销。

运行时，容器内的 MPI 代码链接到容器内的 MPI 库，这些库与宿主系统的 MPI 守护进程通信。对于 MPICH，只要容器内的 MPI 版本与宿主系统版本保持 ABI 兼容性，作业就能成功运行。

```bash
mpirun \
    -np 2 \
    OTHER_MPI_OPTIONS \
    apptainer run \
        --sharens \
        YOUR_APPLICATION
```

有两种运行方式：

- Hybrid 混合模式：同时使用宿主和容器中的 MPI。**需要这两种 MPI 实现 ABI 兼容**。经过测试，Intel MPI 2021 和 MVAPICH2 可以兼容。

- Bind 绑定模式：只使用宿主机的 MPI，容器内无 MPI。**需要 Bind mount 宿主的 MPI 环境**。
