---
tags:
  - 个人笔记
  - 进行中
---

# 📒 内核源码：NFS

## NFS 标准规范

!!! tip

    在阅读 NFS RFC 前，最好对 XDR（RFC 4506）和 RPC（RFC 5531）有所了解。




## 内核源码

### [`mount`](https://git.kernel.org/pub/scm/utils/util-linux/util-linux.git/about/)

`mount` 工具属于 `util-linux`，让我们来看看它是如何挂载文件系统的。

[`mount.c`](https://github.com/util-linux/util-linux/blob/7d13654dd363c8be44044d0e071e99f99599fbda/sys-utils/mount.c#L1047) 中的 `main()` 函数：

```c
struct libmnt_context *cxt;
mnt_context_set_source(cxt, argv[0]);
mnt_context_set_target(cxt, argv[1]);
rc = mnt_context_mount(cxt);
```

通过各种方法获取挂载信息 `cxt`，然后调用 `mnt_context_mount()` 函数进行挂载。

结构体 `struct libmnt_context` 定义在 `libmount/src/mountP.h` 中，包含各种挂载所需的信息，如：

```c
/*
 * Mount context -- high-level API
 */
struct libmnt_context
{
 int action;  /* MNT_ACT_{MOUNT,UMOUNT} */
 int restricted; /* root or not? */

 char *fstype_pattern; /* for mnt_match_fstype() */
 char *optstr_pattern; /* for mnt_match_options() */

 struct libmnt_fs *fs;  /* filesystem description (type, mountpoint, device, ...) */
// ...
};
```

嵌套在其中的 `struct libmnt_fs` 结构体包含重要信息，定义在同一个文件中：

```c
/*
 * This struct represents one entry in a fstab/mountinfo file.
 * (note that fstab[1] means the first column from fstab, and so on...)
 */
struct libmnt_fs {
 struct list_head ents;
 struct libmnt_table *tab;

 int  refcount; /* reference counter */

 unsigned int opts_age; /* to sync with optlist */
 struct libmnt_optlist *optlist;

 int  id;  /* mountinfo[1]: ID */
 int  parent;  /* mountinfo[2]: parent */
 dev_t  devno;  /* mountinfo[3]: st_dev */

 char  *bindsrc; /* utab, full path from fstab[1] for bind mounts */

 char  *source; /* fstab[1], mountinfo[10], swaps[1]:
                                         * source dev, file, dir or TAG */
 char  *tagname; /* fstab[1]: tag name - "LABEL", "UUID", ..*/
 char  *tagval; /*           tag value */
//...
};
```

来到 `mnt_context_mount()` 函数：

```c
/**
 * mnt_context_mount:
 * @cxt: mount context
 *
 * High-level, mounts the filesystem by mount(2) or fork()+exec(/sbin/mount.type).
 * ...
 */
```

注释已经解释了这个函数的作用。从这里开始，函数调用链为：

- `mnt_context_do_mount()`：决定具体使用哪种方式挂载。
- `do_mount_by_types()`、`do_mount_by_pattern()` 或 `do_mount()`：根据 `cxt` 中的信息采用不同的方式进行挂载。前两者会用不同的逻辑尝试文件系统，最终将挂载任务转交到 `do_mount()` 完成。
- `exec_helper()`：`fork()` 并构造参数，用 `execv()` 执行 `mount.type`（由 `mnt_context_prepare_helper()` 从`PATH` 中找到）。

### [`mount.nfs`](http://git.linux-nfs.org/?p=steved/nfs-utils.git;a=summary)

`mount.nfs` 是 NFS 的挂载工具，它是 `nfs-utils` 的一部分。早期 `mount.nfs4` 是独立的，但现在 `mount.nfs` 可以挂载任何版本的 NFS。

`nfs-utils/utils/mount/mount.c` 是 `mount.nfs` 的源码，它的 `main()` 函数与 `mount` 类似，但只负责 NFS 相关的逻辑。其中 `try_mount()` 根据 `string` 和 `fs_type` 决定具体采用哪个挂载函数，有可能为以下三种之一：

- `nfsmount_string()`
    - 当 `string` 被设置时使用该方式。从目前的源码来看，Linux 2.6.3 以上都不会使用该方式。
- `nfsmount()`
- `nfs4mount()`
    - 只有在程序名为 `mount.nfs4` 时才会被调用。也就是说，一般情况下（包括使用 `mount` 命令挂载 NFSv4）都不是 `nfs4mount()`。

!!! note "`nfs4mount.c` 中的内容"

    嗯，我想我大概明白了……`mount.nfs4` 可能是当年变化太大临时弄出来兼容的，后来 `mount.nfs` 把 NFSv4.x 都实现了，但 `mount.nfs4` 还保留着。可以看到 `mount.nfs` 的内容比 `mount.nfs4` 丰富很多。

    读都读了，还是记一下吧：

    ```c
    if (parse_devname(hostdir, &hostname, &dirname))
    goto fail;
    if (fill_ipv4_sockaddr(hostname, &server_addr))
    goto fail;
    if (get_my_ipv4addr(ip_addr, sizeof(ip_addr)))
    goto fail;
    ```

    `fill_ipv4_sockaddr()` 将 NFS Server 的 IP 地址填入 `sockaddr_in` 结构体。如果提供的不是 IP 地址，那么会调用库函数 `gethostbyname()` 函数获取 IP 地址。我找到了一例该库函数的[实现](https://web.mit.edu/ghudson/sipb/pthreads/net/gethostbyname.c)，它将调用库函数 `res_search()` 进行 DNS 查询。

    `get_my_ipv4addr()` 使用 `gethostname()` 获取本机主机名，然后将该主机名转交给 `fill_ipv4_sockaddr()` 获取本机 IP 地址。

接下来我们看 `nfs-utils/utils/mount/nfsmount.c` 中的 `nfsmount()` 函数，从这里开始就涉及很多网络栈的内容了。

