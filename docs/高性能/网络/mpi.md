<style type="text/css">
th { white-space: nowrap; }
</style>

# MPI

OpenMPI 和 MPICH 是最主要的 MPI 实现，后者衍生版本众多。下表对比了常见的 MPI 实现。

| 对比项 | OpenMPI | MVAPICH | Intel MPI | MPICH | HPC-X | Platform MPI | Spectrum MPI |
| - | - | - | - | - | - | - | - |
| 历史 | [History of Open MPI](https://docs.open-mpi.org/en/main/history.html)<br>2003<br>由 OSU、LANL、UTK 的 MPI 实现合并而来 | [overview of the mvapich project](https://mug.mvapich.cse.ohio-state.edu/static/media/mug/presentations/2015/mug15-overview_of_the_mvapich_project-dk_panda.pdf)<br>2002<br>OSU 开发，衍生自 MPICH | （来自 Wikipedia）衍生自 MPICH | [MPICH Overview](https://www.mpich.org/about/overview/)<br>2001<br>由 ANL 和 MSU 开发 | []<br><br>打包自 OpenMPI | IBM 闭源 | IBM 闭源 |
| 文档 | [Open MPI main documentation](https://docs.open-mpi.org/en/main/) | [MVAPICH :: UserGuide](http://mvapich.cse.ohio-state.edu/userguide/) | [Intel® MPI Library Documentation](https://www.intel.com/content/www/us/en/developer/tools/oneapi/mpi-library-documentation.html) | [Guides \| MPICH](https://www.mpich.org/documentation/guides/) | | | |
| mpirun 指向 | `prun`(v5.x)<br>`orterun`(v4.x) | `mpiexec.hydra`<br>`mpiexec.mpirun_rsh`（推荐） | `mpiexec.hydra` | hydra（默认）<br>gforker（编译选项） || | |
| Host 选项 | <pre>-H/--host node1:1,node1:1<br>--hostfile hf</pre> | <pre>-np 2 node1 node2<br>-hostfile hf</pre> | <pre>-hosts node1:1,node2:1<br>-f/-hostfile/-machine/-machinefile hf</pre> | `-f hf`|| | |
| Hostfile 格式 | `node1 slots=n` | `node1:n:hca1` | `node1:n` | `node1:n`|| | |
| 例程/测试 | examples/ | OSU Benchmark | Intel MPI Benchmark | exmaples/<br>`make testing` | OSU Benchmark<br>IMB<br>examples | mpitool | |
| 信息 | `ompi_info --all` | `mpiname -a` | | `mpiexec -info` | | | |

## OpenMPI

!!! quote

    - [EasyBuild Tech Talks · easybuilders/easybuild Wiki](https://github.com/easybuilders/easybuild/wiki/EasyBuild-Tech-Talks-I:-Open-MPI)：2020 年的讲座，对 OpenMPI Z
    - [Open MPI head of development — Open MPI main documentation](https://docs.open-mpi.org/en/main/index.html)
    - [open-mpi/ompi: Open MPI main development repository](https://github.com/open-mpi/ompi)

### 基础概念

我们从 2020 年的 The ABCs of Open MPI 系列讲座开始。

OpenMPI 是一个大型项目，采用模块化组织，称为 MCA（Modular Component Architecture），从上至下分为 Project、Framework、Component：

![ompi/mca](mpi.assets/ompi/mca.webp)

默认情况下，所有 component 被编译为动态链接库（Dynamic Shared Objects, DSO），可以按需加载：

![ompi/dso](mpi.assets/ompi/dso.webp)

到 OpenMPI v5.0，共有如下 FrameWork

- MPI：
    - `coll`：MPI Collecitves，实现 `MPI_BCAST`、`MPI_BARRIER`、`MPI_REDUCE` 等集合通信。

        在 v4.1.0 后，增加了可选的组件，并且可以对默认的通信算法选择进行调优。

    - `op`：MPI Reduction Operations
    - `osc`：MPI One-sided Communications
    - `pml`：MPI Point-to-Point Communications，实现 `MPI_SEND`、`MPI_RECV` 等点对点通信。可选：

        - `ob1`：多设备多链路引擎。它可以选择多个能够到达目标的 BTL 组件，并实现负载均衡。

            ![ompi/ob1](mpi.assets/ompi/ob1.webp)

        - `cm`：用于驱动支持硬件消息标签匹配的网络接口（matching network），比如 iWARP、OminiPath

            ![ompi/cm](mpi.assets/ompi/cm.webp)

        - `ucx`：使用 UCX 通信库，用于 InfiniBand 或 RoCE 设备

    - `topo`：MPI Topologies

- 底层传输：

    - `btl`：Byte Transport Layer

        可选组件：

        - `ofi`：Libfabric
        - `self`：loopback
        - `sm`：共享内存
        - `tcp`

        其余不常见的略过。

    - `bml`：BTL Multipliexing Layer
    - `mtl`：Matching Transport Layer，不常用，略

- 文件：

    - `io`：MPI IO，实现 `MPI_FILE_OPEN`、`MPI_FILE_READ`、`MPI_FILE_WRITE` 等文件操作。可选：

        - `ompio`：默认
        - `romio`：来自 MPICH

    - `fbtl`：MPI File Byte Transfer Layer
    - `fcoll`：MPI File Collectives
    - `fs`：MPI File Management
    - `sharedfp`：MPI shared file pointer operations

- 其他：

    - `hook`：Generic Hooks
    - `vprotocol`：Virtual Protocol API Interposition

#### 通信库



通信库主要通过 PML 选择。一般会采用如下组合：

```text
--mca pml ucx
--mca pml_ucx_verbose 100
--mca osc_ucx_verbose 100
-x UCX_NET_DEVICES=
-x UCX_LOG_LEVEL=trace
```

```text
--mca pml ob1 \
--mca btl ofi
```

IB 和 RoCE 设备支持：

- `openib` 在 OpenMPI 5.1.x 后弃用。与之相关的变量有：

    ```text
    OMPI_MCA_btl_openib_if_include
    ```

- `ucx`：IB 和 RoCE 设备的首选模块，相关变量有：

    ```text
    UCX_NET_DEVICES
    ```

#### PMIx

#### ORTE 与 PRRTE

ORTE 就是

`orterun`、`mpirun`、`mpiexec` 是同一个文件，见 [Ubuntu Manpage: orterun, mpirun, mpiexec - Execute serial and parallel jobs in Open MPI.](https://manpages.ubuntu.com/manpages/trusty/man1/orterun.1.html)

如果要阅读 ORTE 源码，可以在 v4.x 下找到 `orte` 文件夹，其中 `orte/tools/orterun/main.c` 就是 orterun 的入口。

在 OpenMPI v5.0 中，PRRTE 取代了 ORTE。现在，`prun` 的入口在 `prrte/src/tools/prun/main.c`。

### 构建和运行

构建：

```bash
./configure CC= CXX= FC= --with-FOO --without-FOO
```

OpenMPI 内置了某些依赖（如 hwloc 和 libevent），但是也可以用 `--with-hwloc` 等选项替换为外部版本。

要构建支持 CUDA 的 OpenMPI，需要：

- 构建支持 GDR 的 UCX
- 然后构建支持 CUDA 和 UCX 的 OpenMPI

查看当前构建的信息：

```bash
ompi_info --parsable
```

运行：

```bash
export OMPI_MCA_foo=bar
mpirun \
    --mca pml ob1/cm/ucx \
    --mca btl a,b,c \
```

!!! example

    在 OpenMPI v4.x 中：

    ```bash
    $ ompi_info --all | grep btl_openib_if_include
    MCA btl openib: parameter "btl_openib_if_include" (current value: "", data source: default, level: 9 dev/all, type: string)
        Comma-delimited list of devices/ports to be used (e.g. "mthca0,mthca1:2"; empty value means to use all ports found).  Mutually exclusive with btl_openib_if_exclude.
    ```

    可以通过下面的方式：

    ```bash
    export OMPI_MCA_btl_openib_if_include=mlx5_0:1
    mpirun \
        --mca btl_openib_if_include mlx5_0:1
    ```

调试选项：

```text
--mca mpi_show_mca_params all \
--mca pml_base_verbose 100
```

### 源码阅读

#### 构建系统

从 Makefile 开始，构建时我们使用 `all` 这个目标，它进入到所有子文件夹并执行构建，见 [Where is the target all-recursive in makefile? - Stack Overflow](https://stackoverflow.com/questions/17172659/where-is-the-target-all-recursive-in-makefile)。

#### 运行时

#### MCA

#### BTL

##### openib

## MPICH

MPICH 是 MVAPICH、Intel MPI 等众多 MPI 实现的基础。MPICH 维护四个组件：

- mpich
- hydra：启动器，衍生版本大多也支持 hydra 启动
- libpmi
- mpich-testsuite

简单梳理：

- Process Manager（PM）：默认 hydra。编译时可选 gforker，仅在单节点上通过 fork 和 exec 创建进程。
- Communication Device：MPICH 中的通信分为 device 和 module 层。
    - `ch3` 意思是 3rd version of Channel interface。通过编译时 `--with-device=ch3:channel:module` 可选组件。`nemesis` 是默认 channel，单节点用 shared-memory，跨节点用 socket。
    - 默认：`ch4`，支持的 module 有 ofi、UCX、POSIX 共享内存。

## MVAPICH

这可能是目前最先进的 MPI 实现，受 NVIDIA 喜爱，并且也是神威 - 太湖之光所使用的 MPI 实现。

MVAPICH 提供非常多种细分的版本，可根据需求选择。一般选用 MVAPICH2（源码分发）或 MVAPICH2-X（打包分发），提供最全面的 MPI 和 IB/RoCE 支持。

用户手册见 MVAPICH 首页 Support->UserGuide。

以 MVAPICH2 为例梳理：

- 通信：基于 MPICH 的 CH3 改造，通过 OpenFabric 支持了多种 RDMA 通信，例如 OFA-IB-CH3、OFA-RoCE-CH3 等。

### 构建和运行

```bash
./configure --enable-g=all --enable-error-messages=all \
    --enable-debuginfo
```

```bash
# RoCE
MV2USERoCE=1 MV2USERDMACM=1
# IB
MV2_IBA=HCA=mlx4_0:mlx4_1
```

## 其他 MPI 实现

### Platform MPI

这是由 IBM 维护的一个古老的 MPI 实现。

它使用一个同样古老的通信库 [DATL（Direct Access Transport Libraries）](https://www.openfabrics.org/downloads/dapl/README.html)，已于 2016 年停止支持。



