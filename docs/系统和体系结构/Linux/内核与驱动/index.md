# 构建

## 辅助工具

- [Linux source code - Bootlin Elixir Cross Referencer](https://elixir.bootlin.com/linux)：在这里对比多版本源码比 GitHub 方便些。

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

## [Kernel Build System](https://docs.kernel.org/kbuild/index.html)

KBuild 的所有规则放置在 `scripts/Makefile.*` 中。

一些小细节：

- 为了支持构建命令变化时重构，Kbuild 令 Target 依赖 `FORCE` 并使用 `$(call if_changed, <command>)`。
- 为了支持不同选项的编译器，Kbuild 使用 `cflags-y += $(call cc-option,<option>)` 来指定编译器选项。如果不支持，就不会添加。

### `obj-` 的用法

内核 `Makefile` 中的各个模块使用下面的语法：

```makefile
obj-$(CONFIG_FOO) += foo.o bar/
foo-y := bar.o
foo-$(CONFIG_BAR) += baz.o
ccflags-y += -Wall
```

其中 `CONFIG_` 的值从 Kconfig 中读取。如果值为 `y`，则会添加到 `obj-y` 列表。

`obj-y` 列表就是内核构建要编译的所有 `.o` 文件，最终会被全部 `ar` 成一个 `.a` 链入内核。

`obj-m` 将作为可动态加载的内核模块构建。

该列表除了接受 `.o` 文件，也接受子文件夹，以此实现递归构建。

除了 `obj-`，内核也识别 `<module>-`、`ccflags-` 和 `ldflags-` 等。

### 模块构建过程

我们知道模块构建需要传递 `M=` 参数，内核 Makefile 会这样处理：

```makefile title="Makefile"
# 赋值给 KBUILD_EXTMOD，接下来用它控制分支
ifeq ("$(origin M)", "command line")
  KBUILD_EXTMOD := $(M)
endif

# 设置构建目标为 modules
PHONY := __all
__all:
PHONY += all
ifeq ($(KBUILD_EXTMOD),)
__all: all
else
__all: modules
endif

# 将 build-dir 设置为模块目录
ifeq ($(KBUILD_EXTMOD),)
build-dir	:= .
    # 内核构建...
else # KBUILD_EXTMOD
KBUILD_BUILTIN :=
KBUILD_MODULES := 1
build-dir := $(KBUILD_EXTMOD)
    # 模块构建...
endif # KBUILD_EXTMOD

# 定义 modules 目标，依赖链：__all -> modules -> modpost 
# -> modules_check -> MODORDER -> build-dir -> 递归构建
PHONY += modules modules_install modules_sign modules_prepare
export MODORDER := $(extmod_prefix)modules.order
ifdef CONFIG_MODULES # 内核开启了 CONFIG_MODULES
$(MODORDER): $(build-dir)
	@:
modules: modpost
ifneq ($(KBUILD_MODPOST_NOFINAL),1)
	$(Q)$(MAKE) -f $(srctree)/scripts/Makefile.modfinal
endif
PHONY += modules_check
modules_check: $(MODORDER)
	$(Q)$(CONFIG_SHELL) $(srctree)/scripts/modules-check.sh $<
else # 如果内核未开启 CONFIG_MODULES，则不构建 modules 目标
modules:
	@:
KBUILD_MODULES :=
endif # CONFIG_MODULES

# 定义结束目标
PHONY += modpost
modpost: $(if $(single-build),, $(if $(KBUILD_BUILTIN), vmlinux.o)) \
	 $(if $(KBUILD_MODULES), modules_check)
	$(Q)$(MAKE) -f $(srctree)/scripts/Makefile.modpost

# 从 build-dir 开始启动递归构建
PHONY += $(build-dir)
$(build-dir): prepare
	$(Q)$(MAKE) $(build)=$@ need-builtin=1 need-modorder=1 $(single-goals)
```

经常出现的 `$(build)` 定义如下：

```makefile title="scripts/Kbuild.include"
###
# Shorthand for $(Q)$(MAKE) -f scripts/Makefile.build obj=
# Usage:
# $(Q)$(MAKE) $(build)=dir
build := -f $(srctree)/scripts/Makefile.build obj
```

这个文件被很多 Makefile 使用，提供一些通用函数。

使用 `V=1` 开启详细输出，可以看到 Makefile 的递归过程：

```bash
$ make -C /lib/modules/<kernel>/build M="/root/rpmbuild/BUILD/<module>/obj/default"
make[1]: Entering directory '/usr/src/kernels/<kernel>'
make --no-print-directory -C /usr/src/kernels/<kernel> \
-f /usr/src/kernels/<kernel>/Makefile modules
make -f ./scripts/Makefile.build obj=/root/rpmbuild/BUILD/<module>/obj/default need-builtin=1 need-modorder=1
make -f ./scripts/Makefile.build obj=/root/rpmbuild/BUILD/<module>/obj/default/compat \
need-builtin=1 \
need-modorder=1 \

make -f ./scripts/Makefile.build obj=/root/rpmbuild/BUILD/<module>/obj/default/drivers/infiniband \
need-builtin= \
need-modorder=1 \
```

### 实例：infiniband 驱动构建

记住，Kernel 构建的目标是 `obj-y` 等列表。当 Make 递归向下时，传递给 `Makefile.build` 的 `obj` 是它构目标列表的基础。比如，它会检测 `obj` 下的配置文件，决定构建顺序：

```makefile title="scripts/Makefile.build"
ifdef need-builtin
targets-for-builtin += $(obj)/built-in.a
endif
ifdef need-modorder
targets-for-modules += $(obj)/modules.order
endif
# 如果 obj 是子文件夹，用 subdir
$(obj)/: $(if $(KBUILD_BUILTIN), $(targets-for-builtin)) \
	 $(if $(KBUILD_MODULES), $(targets-for-modules)) \
	 $(subdir-ym) $(always-y)
	@:
# Built-in and composite module parts
$(obj)/%.o: $(src)/%.c $(recordmcount_source) FORCE
	$(call if_changed_rule,cc_o_c)
	$(call cmd,force_checksrc)
```

接下来列出部分 Makefile 中的列表：

```makefile
# obj/default/Makefile
obj-y := compat$(CONFIG_COMPAT_VERSION)/
obj-$(CONFIG_INFINIBAND)        += drivers/infiniband/
# obj/default/drivers/infiniband/Makefile
obj-$(CONFIG_INFINIBAND)		+= core/
obj-$(CONFIG_INFINIBAND)		+= hw/
# obj/default/drivers/infiniband/core/Makefile
obj-$(CONFIG_INFINIBAND) +=		ib_core.o ib_cm.o iw_cm.o \
					$(infiniband-y)
```

进入叶子 Makefile 时，仅有 `$(obj)/%.o` 目标，将经过下面的处理：

```makefile title="scripts/Kbuild.include
# Usage: $(call if_changed_rule,foo)
# Will check if $(cmd_foo) or any of the prerequisites changed,
# and if so will execute $(rule_foo).
if_changed_rule = $(if $(if-changed-cond),$(rule_$(1)),@:)
# C (.c) files
# The C file is compiled and updated dependency information is generated.
# (See cmd_cc_o_c + relevant part of rule_cc_o_c)

is-single-obj-m = $(and $(part-of-module),$(filter $@, $(obj-m)),y)

# When a module consists of a single object, there is no reason to keep LLVM IR.
# Make $(LD) covert LLVM IR to ELF here.
ifdef CONFIG_LTO_CLANG
cmd_ld_single_m = $(if $(is-single-obj-m), ; $(LD) $(ld_flags) -r -o $(tmp-target) $@; mv $(tmp-target) $@)
endif
```

```makefile title="scripts/Makefile.lib"
cmd_objtool = $(if $(objtool-enabled), ; $(objtool) $(objtool-args) $@)
```

```makefile title="Makefile.build"
quiet_cmd_cc_o_c = CC $(quiet_modtag)  $@
      cmd_cc_o_c = $(CC) $(c_flags) -c -o $@ $< \
		$(cmd_ld_single_m) \
		$(cmd_objtool)
```

最终的命令例：

```bash
gcc \
    -Wp,-MMD,/root/rpmbuild/BUILD/<module>/obj/default/drivers/infiniband/sw/rxe/.rdma_rxe_dummy.o.d -nostdinc -D__OFED_BUILD__ -D__KERNEL__ -O2 -flto=auto -ffat-lto-objects -fexceptions -g -grecord-gcc-switches -pipe -Wall -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2 -Wp,-D_GLIBCXX_ASSERTIONS -specs=/usr/lib/rpm/TencentOS/TencentOS-hardened-cc1 -fstack-protector-strong  -m64 -march=x86-64-v2 -mtune=generic -fasynchronous-unwind-tables -fstack-clash-protection -fcf-protection -DCOMPAT_BASE="\"mlnx-ofa_kernel-compat-20230220-1058-4ff99ef\"" -DCOMPAT_BASE_TREE="\"mlnx_ofed/mlnx-ofa_kernel-4.0.git\"" -DCOMPAT_BASE_TREE_VERSION="\"4ff99ef\"" -DCOMPAT_PROJECT="\"Compat-mlnx-ofed\"" -DCOMPAT_VERSION="\"4ff99ef\""  -include /lib/modules/<kernel>/build/include/generated/autoconf.h -include /lib/modules/<kernel>/build/include/linux/kconfig.h -include /root/rpmbuild/BUILD/<module>/obj/default/include/linux/compat-2.6.h    -I/root/rpmbuild/BUILD/<module>/obj/default/include -I/root/rpmbuild/BUILD/<module>/obj/default/include/uapi -I/root/rpmbuild/BUILD/<module>/obj/default/drivers/infiniband/debug   -I./arch/x86/include -Iarch/x86/include/generated -Iinclude -I./arch/x86/include/uapi -Iarch/x86/include/generated/uapi -I./include -I./include/uapi -Iinclude/generated/uapi  -I./arch/x86/include -Iarch/x86/include/generated  -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -std=gnu11 -fshort-wchar -funsigned-char -fno-common -fno-PIE -fno-strict-aliasing -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m64 -falign-jumps=1 -falign-loops=1 -mno-80387 -mno-fp-ret-in-387 -mpreferred-stack-boundary=3 -mskip-rax-setup -mtune=generic -mno-red-zone -mcmodel=kernel -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -mindirect-branch-cs-prefix -mfunction-return=thunk-extern -fno-jump-tables -fpatchable-function-entry=16,16 -fno-delete-null-pointer-checks -O2 -fno-allow-store-data-races -fstack-protector-strong -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -fno-inline-functions-called-once -falign-functions=16 -fno-strict-overflow -fno-stack-check -fconserve-stack -fno-builtin-wcslen -Wall -Wundef -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Werror=strict-prototypes -Wno-format-security -Wno-trigraphs -Wno-frame-address -Wno-address-of-packed-member -Wframe-larger-than=2048 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -Wno-dangling-pointer -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-array-bounds -Wno-alloc-size-larger-than -Wimplicit-fallthrough=5 -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wenum-conversion -Wno-unused-but-set-variable -Wno-unused-const-variable -Wno-restrict -Wno-packed-not-aligned -Wno-format-overflow -Wno-format-truncation -Wno-stringop-overflow -Wno-stringop-truncation -Wno-missing-field-initializers -Wno-type-limits -Wno-shift-negative-value -Wno-maybe-uninitialized -Wno-sign-compare -g -Wno-date-time  -DMODULE  -DKBUILD_BASENAME='"rdma_rxe_dummy"' -DKBUILD_MODNAME='"rdma_rxe"' -D__KBUILD_MODNAME=kmod_rdma_rxe \
    -c -o \
    /root/rpmbuild/BUILD/<module>/obj/default/drivers/infiniband/sw/rxe/rdma_rxe_dummy.o \
    /root/rpmbuild/BUILD/<module>/obj/default/drivers/infiniband/sw/rxe/rdma_rxe_dummy.c \
      ; \
    ./tools/objtool/objtool \
        --hacks=jump_label --hacks=noinstr --hacks=skylake --orc --retpoline --rethunk --static-call --uaccess --prefix=16   --module \
        /root/rpmbuild/BUILD/<module>/obj/default/drivers/infiniband/sw/rxe/rdma_rxe_dummy.o
```

### 编译选项

接下来探究 `c_flags`，它定义为：

```makefile title="scripts/Makefile.lib"
c_flags        = -Wp,-MMD,$(depfile) $(NOSTDINC_FLAGS) $(LINUXINCLUDE)     \
		 -include $(srctree)/include/linux/compiler_types.h       \
		 $(_c_flags) $(modkern_cflags)                           \
		 $(basename_flags) $(modname_flags)
```

逐个展开：

```makefile
# $(depfile)
depfile = $(subst $(comma),_,$(dot-target).d)
# $(NOSTDINC_FLAGS)
-nostdinc
# $(LINUXINCLUDE)
LINUXINCLUDE    := \
  -I$(srctree)/arch/$(SRCARCH)/include \
  -I$(objtree)/arch/$(SRCARCH)/include/generated \
  $(if $(building_out_of_srctree),-I$(srctree)/include) \
  -I$(objtree)/include \
  $(USERINCLUDE)
USERINCLUDE    := \
  -I$(srctree)/arch/$(SRCARCH)/include/uapi \
  -I$(objtree)/arch/$(SRCARCH)/include/generated/uapi \
  -I$(srctree)/include/uapi \
  -I$(objtree)/include/generated/uapi \
                -include $(srctree)/include/linux/compiler-version.h \
                -include $(srctree)/include/linux/kconfig.h
# -include $(srctree)/include/linux/compiler_types.h

# $(_c_flags)
见下文
# $(modkern_cflags)
modkern_cflags =                                          \
 $(if $(part-of-module),                           \
  $(KBUILD_CFLAGS_MODULE) $(CFLAGS_MODULE), \
  $(KBUILD_CFLAGS_KERNEL) $(CFLAGS_KERNEL) $(modfile_flags))
# $(basename_flags)
basename_flags = -DKBUILD_BASENAME=$(call name-fix,$(basetarget))
```

其中，`_c_flags` 的展开有几个步骤：

- 汇集 `KBUILD_CPPFLAGS`、`KBUILD_CFLAGS`、`ccflags-y` 和 `CFLAGS_$(target-stem).o`，然后基于 `ccflags-remove-y` 和 `CFLAGS_REMOVE_$(target-stem).o` 进行过滤

    ```makefile
    _c_flags       = $(filter-out $(CFLAGS_REMOVE_$(target-stem).o), \
                     $(filter-out $(ccflags-remove-y), \
                         $(KBUILD_CPPFLAGS) $(KBUILD_CFLAGS) $(ccflags-y)) \
                     $(CFLAGS_$(target-stem).o))
    ```

    其中 `KBUILD_*FLAGS` 在各个 Makefile 中定义，比如不同的体系结构需要不同的 FLAG。**并且，用户可以通过环境变量导入更多的 Flag：**

    ```makefile
    # Add user supplied CPPFLAGS, AFLAGS, CFLAGS and RUSTFLAGS as the last assignments
    KBUILD_CPPFLAGS += $(KCPPFLAGS)
    KBUILD_AFLAGS   += $(KAFLAGS)
    KBUILD_CFLAGS   += $(KCFLAGS)
    KBUILD_RUSTFLAGS += $(KRUSTFLAGS)
    ```

- 添加 KConfig 控制的选项：

    ```makefile
    ifeq ($(CONFIG_KASAN),y)
    ifneq ($(CONFIG_KASAN_HW_TAGS),y)
    _c_flags += $(if $(patsubst n%,, \
            $(KASAN_SANITIZE_$(basetarget).o)$(KASAN_SANITIZE)y), \
            $(CFLAGS_KASAN), $(CFLAGS_KASAN_NOSANITIZE))
    endif
    endif
    ```

- out-of-tree 添加结尾：

    ```makefile
    # $(srctree)/$(src) for including checkin headers from generated source files
    # $(objtree)/$(obj) for including generated headers from checkin source files
    ifeq ($(KBUILD_EXTMOD),)
    ifdef building_out_of_srctree
    _c_flags   += -I $(srctree)/$(src) -I $(objtree)/$(obj)
    _a_flags   += -I $(srctree)/$(src) -I $(objtree)/$(obj)
    _cpp_flags += -I $(srctree)/$(src) -I $(objtree)/$(obj)
    endif
    endif
    ```

### modpost 和 modules_check

回顾：

```makefile title="Makefile"
modpost: $(if $(single-build),, $(if $(KBUILD_BUILTIN), vmlinux.o)) \
	 $(if $(KBUILD_MODULES), modules_check)
	$(Q)$(MAKE) -f $(srctree)/scripts/Makefile.modpost
```

`Makefile.modpost` 负责的功能包括：模块的版本信息、符号依赖关系以及模块的 CRC 校验等。

1. **模块版本信息生成**：
   - 通过 `modpost` 工具生成 `<module>.mod.c` 文件，其中包含模块的版本信息（如 `MODULE_VERSION`、`MODULE_ALIAS`、`MODULE_LICENSE` 等）。
   - 这些信息会被嵌入到模块的 ELF 段中，供内核加载时使用。

2. **符号依赖与版本控制**：
   - 生成 `Module.symvers` 文件，记录所有导出符号的 CRC 值，用于模块间的版本控制。
   - 如果启用了 `CONFIG_MODVERSIONS`，还会处理符号版本信息，确保模块间的兼容性。

3. **依赖关系处理**：
   - 读取 `modules.order` 文件，获取所有模块的构建顺序和依赖关系。
   - 如果某些输入文件缺失（如 `vmlinux.o` 或 `Module.symvers`），会发出警告，但不会中断构建（除非显式设置 `KBUILD_MODPOST_WARN=1`）。

4. **特殊配置支持**：
   - 支持 `CONFIG_TRIM_UNUSED_KSYMS`，通过白名单过滤未使用的内核符号。
   - 支持外部模块构建（`KBUILD_EXTMOD`），允许从外部目录加载额外的符号信息（`KBUILD_EXTRA_SYMBOLS`）。

5. **错误处理与警告**：
   - 如果某些输入文件缺失，会输出警告信息，但默认情况下不会终止构建。
   - 可以通过设置 `KBUILD_MODPOST_WARN=1` 将错误降级为警告。

6. **性能优化**：
   - 通过 `-T` 参数传递 `modules.order` 文件，避免因参数过长导致的性能问题。

7. **模块构建的阶段性**：
   - 该文件是模块构建的第二阶段，第一阶段生成 `.o` 文件和 `.mod` 文件，第二阶段通过 `modpost` 处理这些文件。


