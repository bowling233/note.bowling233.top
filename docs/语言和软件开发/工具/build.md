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

## Meson

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

## Ninja

Ninja 设计从高级构建系统接受输入，用于快速构建。Ninja 配置文件难以编写，但却能快速识别增量构建。


## GNU Autotools

### 多版本 gcc 管理

CentOS
https://blog.csdn.net/JasonZhao7/article/details/128159650

与之类似地，Debian 中有 update-alternatives

