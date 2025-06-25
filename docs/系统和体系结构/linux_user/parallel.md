# 并行编程

## pthread

绑核：

```c
pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
```
