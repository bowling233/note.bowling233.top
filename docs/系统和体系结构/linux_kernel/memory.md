# 内存

## 基础知识

### 信息

/proc/meminfo

### sysctl 配置

https://sysctl-explorer.net/kernel/numa_balancing/


## NUMA

### NUMA Balancing

https://docs.redhat.com/en/documentation/red_hat_enterprise_linux/7/html/virtualization_tuning_and_optimization_guide/sect-virtualization_tuning_optimization_guide-numa-auto_numa_balancing

https://www.linux-kvm.org/images/7/75/01x07b-NumaAutobalancing.pdf

自动把 task 调度到访问的数据所在的 NUMA 节点上。

## Hugepage

### hugtlbfs

https://www.kernel.org/doc/html/v5.0/admin-guide/mm/hugetlbpage.html#hugetlbpage

https://www.kernel.org/doc/html/v5.0/vm/hugetlbfs_reserv.html

传统上，应用程序需要 `mmap()` 申请大页。

### libhugetlbfs

手动管理大页的用户空间库

### Transparent Hugepage

https://docs.kernel.org/admin-guide/mm/transhuge.html

内核自动管理大页

- 当一个程序访问某个区域时，内核会自动决定是否使用大页，且为不同大小的内存区域分配大小不一的内存页面
- khugepaged是一个内核守护进程，负责自动扫描和合并多个小页为一个大页。它定期检查内存，寻找机会将多个4KB页面合并成一个2MB的PMD（Page Map Directory）大小的大页，从而提高内存的使用效率。它会尝试将内存区域合并成一个大页，如果内存碎片过多，khugepaged会推迟此操作。

缺点：

- 增加内存碎片和潜在的内存浪费
- 可能会引入不一致的内存访问模式，导致性能测试结果的不稳定

相关配置：

```
/sys/kernel/mm/transparent_hugepage/enabled
```

https://medium.com/@boutnaru/the-linux-process-journey-khugepaged-56df6182c3e9

