# 网络

## 名称解析 Name Service Switch

!!! quote

    - [Name Service Switch (The GNU C Library)](https://www.gnu.org/software/libc/manual/html_node/Name-Service-Switch.html)
    - [gethostbyname(3) - Linux manual page](https://man7.org/linux/man-pages/man3/gethostbyname.3.html)

NSS 负责名称解析，可以使用 NIS、DNS、本地文件等提供服务。为了方便扩展，这些服务提供者被设计为一个个 Database 模块，支持对应类型名称的查询，例如 ServiceName（端口名）、HostName（主机名）等。

非常常见的一些函数如下：

```c
#include <netdb.h>
getservbyname()
gethostbyname()
```

我们将首先了解底层数据库的实现，再解读上层 API。

### Module & Database

定义：

```c title="nss/nss_module.h"
/* A NSS service module (potentially unloaded).  Client code should
   use the functions below.  */
struct nss_module
{
  union
  {
    struct nss_module_functions typed;
    nss_module_functions_untyped untyped;
  } functions;
  struct nss_module *next;
  char name[];
};
```

Database 和 Module 是什么关系呢？请看：

```c title="nss/nss_action.h, nss/nss_database.h"
/* A NSS action pairs a service module with the action for each result
   state.  */
struct nss_action {
    struct nss_module *module;
    unsigned int action_bits;
};
/* A list of struct nss_action objects in array terminated by an
   action with a NULL module.  */
typedef struct nss_action *nss_action_list;
/* Internal type.  Exposed only for fork handling purposes.  */
struct nss_database_data {
  struct file_change_detection nsswitch_conf;
  nss_action_list services[NSS_DATABASE_COUNT];
};
```

```c title="nss/nss_database.c"
static const database_name nss_database_name_array[] =
  {
#define DEFINE_DATABASE(name) #name,
#include "databases.def"
#undef DEFINE_DATABASE
  };
```

数据库查询中经常使用两个宏：

- `DB_LOOKUP_FCT`：获得可用的查询函数表

    ```c title="nss/XXX-lookup.c"
    #define DB_LOOKUP_FCT CONCAT3_1 (__nss_, DATABASE_NAME, _lookup2)
    int DB_LOOKUP_FCT (nss_action_list *ni, const char *fct_name, const char *fct2_name,
            void **fctp){
        if (! __nss_database_get (DATABASE_NAME_ID, &DATABASE_NAME_SYMBOL))
            return -1;
        *ni = DATABASE_NAME_SYMBOL;
        /* We want to know about it if we've somehow got a NULL action list;
        in the past, we had bad state if seccomp interfered with setup. */
        assert(*ni != NULL);
        return __nss_lookup (ni, fct_name, fct2_name, fctp);
    }
    ```

- `DL_CALL_FCT`：调用查询函数

    ```c title="include/dlfcn.h"
    # define DL_CALL_FCT(fctp, args) ((fctp) args)
    ```

其中 `__nss_lookup()` 的调用链如下：

```text
__nss_lookup() 对 action_list 循环
__nss_lookup_function (*ni, fct_name);
__nss_module_get_function (ni->module, fct_name);
```

在 `__nss_module_get_function()` 中：

- `__nss_module_load(module)` 检查对应模块是否已载入
- `bsearch(name, nss_function_name_array)` 在函数表中查找函数名字
- `idx = name_entry - nss_function_name_array` 得到函数的编号
- `fptr = module->functions.untyped[idx]` 获得模块的对应函数

接下来我们以 hosts 作为 Database，向下继续挖掘。



### Name Service Cache Daemon

### gethostbyname

用法：

根据 Glibc 源码，`gethostbyname` 的调用路径和工作原理如下：

主体调用路径：

- `gethostbyname` 函数的声明在 `resolv/netdb.h`，其实现和实际功能主要在 `nss/gethstbynm.c` 等 NSS（Name Service Switch）相关文件中。
- 内部会调用 `gethostbyname2` 和 `gethostbyname2_r`，以支持不同的地址族（如 IPv4/IPv6）和线程安全。
- 这些实现最终会调用到 NSS 框架，查找 `/etc/nsswitch.conf` 配置指定的数据源（如 files, dns, etc）。
- 对于 DNS 查询，调用会进入 `resolv/compat-gethnamaddr.c` 的 `res_gethostbyname2_context`，并通过 `__res_context_search` 发起实际的 DNS 查询。

关键源码路径：

- `nss/gethstbynm.c` 和 `nss/gethstbynm2.c`：定义了 `gethostbyname` 及其变体的顶层实现。
- `resolv/compat-gethnamaddr.c`：
    - `res_gethostbyname2_context` 负责真正的查询，失败时设置错误码。
    - `__res_context_search` 发起 DNS 查询，返回负值表示查询失败。
    - 如果查找失败，源码中会多处调用 `__set_h_errno` 设置错误码，并返回 NULL。

关键错误处理代码片段：

```c
if ((n = __res_context_search(ctx, name, C_IN, type, buf.buf->buf, 1024,
      &buf.ptr, NULL, NULL, NULL, NULL)) < 0) {
    if (buf.buf != origbuf)
        free (buf.buf);
    if (errno == ECONNREFUSED)
        return (_gethtbyname2(name, af));
    return (NULL);
}
```

（见：`resolv/compat-gethnamaddr.c`，源码位置）

以及：

```c
no_recovery:
    __set_h_errno (NO_RECOVERY);
    return (NULL);
```

（见：`resolv/compat-gethnamaddr.c:394-426`）

产生“无法获得地址”错误的源码位置：

一般会在如下位置设置错误并返回 NULL，进而导致 `gethostbyname` 失败：

- DNS 查询失败、未找到主机、或 hosts 文件（`/etc/hosts`）查不到时，在 `compat-gethnamaddr.c` 的多处调用 `__set_h_errno`，如 `HOST_NOT_FOUND`、`NO_RECOVERY`、`NETDB_INTERNAL` 等。
- 具体源码位置示例：
    - `resolv/compat-gethnamaddr.c`，如 394-426 行的 `no_recovery` 标签。
    - `resolv/compat-gethnamaddr.c`，如 721-744 行 hosts 文件查找失败时。
    - 以及 `__res_context_search` 调用失败时。

总结 `gethostbyname` 的大致内部流程：

1. 查找 hosts 文件（`/etc/hosts`）。
2. 查找 DNS（如 `/etc/resolv.conf` 配置的服务器）。
3. 若都失败，会设置合适的 `h_errno` 并返回 NULL，导致“无法获得地址”错误。

参考源码文件：

- [`nss/gethstbynm.c`](https://github.com/bminor/glibc/blob/master/nss/gethstbynm.c)
- [`nss/gethstbynm2.c`](https://github.com/bminor/glibc/blob/master/nss/gethstbynm2.c)
- [`resolv/compat-gethnamaddr.c`](https://github.com/bminor/glibc/blob/master/resolv/compat-gethnamaddr.c)

### getXXbyYY

在 Glibc 2.1（1999）中，我们还能找到 `gethostbyname` 的实现：

```c
struct hostent *
gethostbyname(name)
    const char *name;
{
    struct hostent *hp;

    if ((_res.options & RES_INIT) == 0 && __res_ninit(&_res) == -1) {
        __set_h_errno (NETDB_INTERNAL);
        return (NULL);
       }
    if (_res.options & RES_USE_INET6) {
        hp = gethostbyname2(name, AF_INET6);
        if (hp)
            return (hp);
    }
    return (gethostbyname2(name, AF_INET));
}
```

而在 Glibc 2.41，这些函数使用 `nss/getXXbyYY.c` 中的统一宏模板实现。

- 提供一套通用的查找框架，可通过宏定义定制具体的查询函数（如查找主机、服务、协议等）。能通过多次 include 本文件、配合不同宏，实现多个类似的查找函数。
- 文件开头约定：调用前需要定义 `LOOKUP_TYPE`（查询结果类型）、`FUNCTION_NAME`（导出函数名）、`DATABASE_NAME`（数据库名，如 host）、`ADD_PARAMS`/`ADD_VARIABLES`（额外参数）、`BUFLEN`（缓冲区大小）等。
- 统一了缓冲区管理、错误码处理（如 `h_errno`）、查找重试、内存回收等细节，减少代码重复。
- 首次调用时分配缓冲区；查找失败且缓冲区不够时自动扩容并重试。
- 非重入版本统一使用静态缓冲区和全局状态。
- 用 `static buffer` 指针和 `buffer_size` 管理查找用的缓冲区。
- 实现了无锁与线程安全的查找逻辑，既支持非重入（非 _r）版本，也为重入版本做了适配。
- 使用 `__libc_lock_define_initialized` 定义静态锁，保证多线程下缓冲区和状态的安全。

该宏模板的核心是下面的调用：

```c title="nss/getXXbyYY.c"
/* Prototype for reentrant version we use here.  */
extern int INTERNAL (REENTRANT_NAME) (ADD_PARAMS, LOOKUP_TYPE *resbuf,
          char *buffer, size_t buflen,
          LOOKUP_TYPE **result H_ERRNO_PARM)
     attribute_hidden;
while (buffer != NULL
     && (INTERNAL (REENTRANT_NAME) (ADD_VARIABLES, &resbuf, buffer,
                    buffer_size, &result H_ERRNO_VAR)
         == ERANGE)
#ifdef NEED_H_ERRNO
     && h_errno_tmp == NETDB_INTERNAL
#endif
) { /* manage buffer */ }
```

可以看到任务转交给重入版本了。重入版本的核心是下面的循环：

```c title="nss/getXXbyYY_r.c"
#ifndef DB_LOOKUP_FCT
# define DB_LOOKUP_FCT CONCAT3_1 (__nss_, DATABASE_NAME, _lookup2)
#endif
extern int DB_LOOKUP_FCT (nss_action_list *nip, const char *name,
    const char *name2, void **fctp);
no_more = DB_LOOKUP_FCT (&nip, REENTRANT_NAME_STRING,
    REENTRANT2_NAME_STRING, &fct.ptr);
while (no_more == 0) {
    status = DL_CALL_FCT (fct.l, (ADD_VARIABLES, resbuf, buffer, buflen,
        &errno H_ERRNO_VAR EXTRA_VARIABLES));
    if (status == NSS_STATUS_TRYAGAIN) 
        break;

    if (do_merge) {
        if (status == NSS_STATUS_SUCCESS){
        /* The previous loop saved a buffer for merging.
            Perform the merge now.  */
        err = MERGE_FN (&mergegrp, mergebuf, endptr, buflen, resbuf,
            buffer);
        CHECK_MERGE (err,status);
        do_merge = 0;
        } else {
        /* If the result wasn't SUCCESS, copy the saved buffer back
            into the result buffer and set the status back to
            NSS_STATUS_SUCCESS to match the previous pass through the
            loop.
            * If the next action is CONTINUE, it will overwrite the value
                currently in the buffer and return the new value.
            * If the next action is RETURN, we'll return the previously-
                acquired values.
            * If the next action is MERGE, then it will be added to the
                buffer saved from the previous source.  */
        err = DEEPCOPY_FN (mergegrp, buflen, resbuf, buffer, NULL);
        CHECK_MERGE (err, status);
        status = NSS_STATUS_SUCCESS;
        }
    }
}
```

这里调用不同 DATABASE 的查找函数，并合并结果。

以 `gethostbyname` 为例，`DB_LOOKUP_FCT` 最终会展开为 `__nss_hosts_lookup2`。

### 重入（reentrant）

**重入（Reentrant）**，指的是一个函数能够被多个线程（或多次递归）安全地同时调用，而不会导致数据混乱或错误。  
通常，重入函数不会依赖或修改共享的全局状态，也不会使用静态（static）或者全局变量存储中间结果。这样，每次调用都是“自洽”的、互不干扰的。

在多线程编程中，如果两个线程同时调用同一个非重入的函数，可能会因为共享静态数据而产生冲突。例如，C 标准库中许多传统的查找函数（如 `gethostbyname`、`strtok`）都是非重入的，它们会把结果缓存在静态变量中，导致线程间互相覆盖数据。

为了解决这个问题，glibc 等库提供了带 `_r` 后缀的重入版本函数。这些函数通常要求调用者自己提供用于存放返回结果的缓冲区和状态变量，从而避免了全局或静态数据的竞争。

!!! example "`_r` 后缀函数的例子"

    下面举几个典型的 `_r` 函数（reentrant function）例子：

    1. **`gethostbyname` 与 `gethostbyname_r`**
    - `gethostbyname(const char *name)`（非重入）：返回静态分配的结构体指针。
    - `gethostbyname_r(const char *name, struct hostent *ret, char *buf, size_t buflen, struct hostent **result, int *h_errnop)`（重入）：调用者分配结构体和缓冲区，函数把结果放进去。

    2. **`strtok` 与 `strtok_r`**
    - `strtok(char *str, const char *delim)`（非重入）：使用静态变量保存状态，非线程安全。
    - `strtok_r(char *str, const char *delim, char **saveptr)`（重入）：调用者提供状态指针，线程安全。

    3. **`asctime` 与 `asctime_r`**
    - `asctime(const struct tm *tm)`（非重入）：返回静态分配的字符串指针。
    - `asctime_r(const struct tm *tm, char *buf)`（重入）：调用者提供缓冲区。

    4. **`localtime` 与 `localtime_r`**
    - `localtime(const time_t *timep)`（非重入）：使用静态区域返回 `struct tm*`。
    - `localtime_r(const time_t *timep, struct tm *result)`（重入）：调用者提供 `struct tm*` 存放结果。
