# 构建

## 构建系统

### CMake

!!! quote

    - [Modern CMake](https://cliutils.gitlab.io/modern-cmake/README.html)

#### CMake 命令行

使用：

```shell
cmake -S . -B build
cmake --build build -j -v
cmake --install build
```

指定编译器和构建系统：

```shell
cmake --help
CC=clang CXX=clang++ ...
cmake -G"Ninja"
```

选项：

```shell
# -L 显示所有变量，-H 显示帮助
cmake -LH
cmake -D...
# 标准选项
-DCMAKE_BUILD_TYPE=Release|Debug|RelWithDebInfo
-DCMAKE_INSTALL_PREFIX=...
-DBUILD_SHARED_LIBS=ON|OFF
-DBUILD_TESTING=
```

#### CMake 项目结构

```text
project
├── CMakeLists.txt
├── cmake
│   └── FindXXX.cmake
├── include
│   └── project
│       └── lib.hpp
├── src
│   ├── lib.cpp
│   └── CMakeLists.txt
├── apps
│   ├── app.cpp
│   └── CMakeLists.txt
├── docs
│   └── CMakeLists.txt
├── extern
└── tests
    ├── testlib.cpp
    └── CMakeLists.txt
```

- 为了能将 `include` 拷贝至 `/usr/include` 等安装目录，其中不应当含有 `CMakeLists.txt`，且应当创建一层项目的子目录 `project`。
- `CMakeLists.txt` 分散到各个子目录，使用 `add_subdirectory` 引入。
- 使用下面的命令引入 `cmake` 目录下的组件：

```cmake
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
```

#### CMake 基本语法

```cmake
cmake_minimum_required(VERSION 4.0.0)

project(MyProject VERSION 1.0
                  DESCRIPTION "Very nice project"
                  LANGUAGES CXX)
# 语言包括 C、CXX、Fortran、ASM、CUDA

add_executable(one two.cpp three.h)
# 智能识别源文件，头文件一般用于 IDE 提示

add_library(one STATIC two.cpp three.h)
# 不指定则由 BUILD_SHARED_LIBS 决定
# 只有头文件的库：INTERFACE

target_include_directories(one PUBLIC include)
# PUBLIC：链接到当前 target 的
# PRIVATE：仅当前 target
# INTERFACE：依赖当前 target 的

add_library(another STATIC another.cpp another.h)
target_link_libraries(another PUBLIC one)
# 添加依赖 target 或系统中的库，可使用绝对路径、链接器选项

set(MY_VARIABLE "value")
${MY_VARIABLE}
# 本地变量具有作用域，附加 PARENT_SCOPE 可以传递到父作用域
$ENV{MY_ENV_VARIABLE}
# 环境变量
set(MY_CACHE_VARIABLE "VALUE" CACHE STRING "Description")
set(MY_CACHE_VARIABLE "VALUE" CACHE STRING "" FORCE)
# 缓存变量，不覆盖与覆盖
set(MY_LIST "one" "two")
# 列表操作
option(MY_OPTION "This is settable from the command line" OFF)
# 布尔选项

mark_as_advanced(MY_CACHE_VARIABLE)
# 高级选项，在 cmake -L 中隐藏

set_property(TARGET TargetName
             PROPERTY CXX_STANDARD 11)
set_target_properties(TargetName PROPERTIES
                      CXX_STANDARD 11)
get_property(ResultVariable TARGET TargetName PROPERTY CXX_STANDARD)

if(variable)
else()
endif()
```

Target：

- 在 IDE 中 Target 一般显示为文件夹。
- **包含下列内容：**include directories, linked libraries (or linked targets), compile options, compile definitions, compile features, and more.

变量：

- 本地变量
- 缓存变量：会在 `CMakeCache.txt` 中保存，不用每次都重新设置
- 环境变量

属性：与变量类似，但是附加到某些对象上，如 target 或目录。很多属性从对应的 `CMAKE_` 变量初始化而来，比如：`CMAKE_CXX_STANDARD` -> `CXX_STANDARD`。

#### `CMAKE_BUILD_TYPE`

!!! quote

    - [cmake-buildsystem(7)](https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#build-configurations)
    - [cmake - What are CMAKE_BUILD_TYPE: Debug, Release, RelWithDebInfo and MinSizeRel? - Stack Overflow](https://stackoverflow.com/questions/48754619/what-are-cmake-build-type-debug-release-relwithdebinfo-and-minsizerel)

该变量有可能影响程序行为。我多次遇到 `Release` 和 `Debug` 两种模式下程序行为不一致的情况。

CMake 文档并没有对 `CMAKE_BUILD_TYPE` 的行为进行明确说明，因为这是由编译器和构建系统决定的。在 `CMakeCache.txt` 中可以找到对于不同 `CMAKE_BUILD_TYPE` 的编译器选项 `CMAKE_<LANG>_FLAGS_<CONFIG>`。

```shell
grep '_FLAGS' CMakeCache.txt | grep -v -E '/|INTERNAL'
```

### Meson

使用方式：

```shell
meson setup build && cd build
meson configure -Dbuildtype=debug
meson compile 或 ninja
meson install
```

- 不允许在源码目录中编译，总是新建一个目录
- 修改项目后，只需要重新执行 `meson compile` 即可
- 使用 `-D` 修改构建选项

配置文件：`meson.build`

```text
project('tutorial', 'c')
gtkdeps = [dependency('gtk+-3.0'), dependency('gtksourceview-3.0')]
executable('demo', 'main.c', dependencies : gtkdep)
```

- 不需要引入头文件

### Ninja

Ninja 设计从高级构建系统接受输入，用于快速构建。Ninja 配置文件难以编写，但却能快速识别增量构建。

### GNU Autotools

#### GNU Make

简单语法略过，记录一些细节。

- [Flavors (GNU make)](https://www.gnu.org/software/make/manual/html_node/Flavors.html)

    - 简单赋值：使用 `:=` 定义，在第一次定义时展开，**以后不会再变**。
    - 递归赋值：使用 `=` 定义，每次遇到时展开，**可能会变**。
    - 条件赋值：使用 `?=` 定义，如果没有定义过，则展开。

#### GNU Automake

!!! quote

    - [](https://www.gnu.org/software/automake/manual/html_node/index.html)

> Note that most GNU Make extensions are not recognized by Automake. Using such extensions in a Makefile.am will lead to errors or confusing behavior.

例如 `filter-out` 这样的 GNU Make 扩展在 Automake 中无效。

#### 多版本 gcc 管理

[CentOS](https://blog.csdn.net/JasonZhao7/article/details/128159650)

与之类似地，Debian 中有 update-alternatives

## 包管理器

包管理器又是如何利用、包装上述构建系统的？如何在本地修改软件包源码、修改编译选项并重新构建安装？

### APT/DPKG

!!! quote

    - [Chapter 5. Simple packaging](https://www.debian.org/doc/manuals/debmake-doc/ch05.en.html)

#### deb 包格式

打包流程：

```shell
# 进入源码目录
debmake
debuild
```

#### `debian/rules`

`debain/rules` 是一个 Makefile，定义了软件包的构建过程。`debuild` 会执行下面这些操作：

```shell
fakeroot debian/rules clean
fakeroot debian/rules build
fakeroot debian/rules binary
fakeroot debian/rules clean
```

`debian/rules` 默认将所有 target 转交给 `dh`，也可以自行覆盖 target 做自定义操作：

```Makefile
%:
	dh $@
```

#### [dh](https://manpages.debian.org/testing/debhelper/dh.1.en.html)

!!! todo

    研究 dh 如何使用各个构建系统的。

    研究 rdma-core 是如何被构建的。

重新打包：

```shell
# 获取源码，安装相关依赖：
apt-get source <package>
apt-get build-dep <package>
cd <package>-*/
# 修改源码
# 添加版本后缀
dch --local foo
# 构建
debuild -us -uc
```

### YUM/DNF/RPM

!!! quote

    - [How to create a Linux RPM package](https://www.redhat.com/en/blog/create-rpm-package)
    - [Rpmbuild Tutorial](https://rpmbuildtut.wordpress.com/)
    - [Fedora Packaging Guidelines :: Fedora Docs](https://docs.fedoraproject.org/en-US/packaging-guidelines/)

打包流程：

```text
# RPM 包命名格式
<name>-<version>-<release>.<arch>.rpm
bdsync-0.11.1-1.x86_64.rpm
bdsync-0.11.1-1.el8.x86_64.rpm
```

```bash
$ dnf install -y rpmdevtools rpmlint
# RPM 包构建要求家目录下指定结构
$ rpmdev-setuptree
$ tree rpmbuild
rpmbuild
├── BUILD
├── RPMS
├── SOURCES # 源码打成 tar 放在这里
├── SPECS
└── SRPMS
# 放置源码
$ tar czvf rpmbuild/SOURCES/myscript-1.0.tar.gz myscript-1.0/
# 放置 spec 文件
$ rpmdev-newspec rpmbuild/SPECS/myscript.spec
# 构建
$ rpmbuild -bb rpmbuild/SPECS/myscript.spec
```

#### rpm 包操作

```bash
# 解包
$ rpm2cpio *.rpm | cpio -idm
```

#### `.spec`

- 宏 `%{_prefix}`，使用 `rpm --eval '%{_prefix}'` 可以查看。有内置、用户定义、spec 文件专有三种。

    spec 文件专有的宏控制整体构建：

    ```text
    # 构建
    %build
    %configure

    # 安装与卸载
    %pre # before install script run
    %post # after install

    %files # files to be installed

    %preun # before uninstall 
    %postun # after uninstall
    ```

#### rpmbuild

- [传递参数](https://stackoverflow.com/questions/64220041/how-to-use-environment-variables-in-the-header-of-rpm-spec-file-for-version)：

    环境变量无法进入 rpmbuild 的构建过程，应该通过选项传输：

    ```bash
    rpmbuild -ba --define "version_ ${VERSION}" myspec.spec
    Version: %{version_}
    ```

- 构建系统：支持 [CMake](https://docs.fedoraproject.org/en-US/packaging-guidelines/CMake/) 和 [Meson](https://docs.fedoraproject.org/en-US/packaging-guidelines/Meson/)，可以在 spec 文件中使用预制的宏，会自动设置好各类选项：

    ```text
    %__cmake
    %cmake
    %cmake_build
    %cmake_install
    ```

### Nix

!!! quote

    - [Welcome to nix.dev — nix.dev documentation](https://nix.dev/)
    - [NixOS Search - Packages](https://search.nixos.org/packages)

```bash
#!/usr/bin/env nix-shell
#! nix-shell -i <interpreter shell> --pure
#! nix-shell -p <package>
#! nix-shell -I nixpkgs=<package archive>

```