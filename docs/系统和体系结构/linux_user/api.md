# API

- [Standards and Portability (The GNU C Library)](https://www.gnu.org/software/libc/manual/html_node/Standards-and-Portability.html)

本文讨论在用户态，与内核交互或内核 Bypass 的编程接口。各种标准如 ISO C 和 POSIX 定义了操作系统应提供的接口规范，详见 GNU C Library 手册章节。

Linux 的一部分系统调用被 POSIX 规范的用户态接口所覆盖，而没有覆盖的则需要自行使用 `syscall()`。

## ISO C

https://en.cppreference.com/w/c/header.html

## POSIX

- [The Open Group Base Specifications Issue 8](https://pubs.opengroup.org/onlinepubs/9799919799/)
