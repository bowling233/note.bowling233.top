# Operating System Concepts

## 第二章：操作系统结构

### 2.8 操作系统结构

本节介绍了宏、微内核等设计结构，结合具体操作系统的例子。

- 宏内核（monolithic）：
    - 编写简单，内核内部通信快，性能好。但调试和维护困难。
    - 例子：
        - 原始 UNIX 是宏内核，扩展后成为层次化的。
        - Linux、Windows
- 层次化（layered）：
    - 在计算机网络中取得成功。
    - 但事实上没有操作系统这样设计，因为：
        - 难以正确划分层次。
        - 层次间调用开销大。
- 微内核：
    - 内核仅提供最基本服务，如：IPC、内存管理、CPU 调度。
    - 例子：
        - Windows NT 第一版为微内核，但比 Windows 95 性能差很多；Windows NT 4.0 将更多用户空间的服务移到内核，性能提高。到 Windows XP，整体架构变得更偏向宏内核。
        - 苹果 macOS 和 iOS 的内核 Darwin 是微内核。
        - 鸿蒙 OS 是微内核。
- 模块（module）
    - 内核支持在启动时或运行时加载内核模块（loadable kernel modules，LKM）。
    - 是在现代操作系统上最为常见的一种设计。

事实上现代操作系统都是混合架构的，不会严格遵循某种设计结构。

本节剩余部分介绍了 Darwin 和 Android，旁注甚至还介绍了 WSL 的原理。

- Darwin：源码在 [:simple-github: apple-oss-distributions/xnu](https://github.com/apple-oss-distributions/xnu)，描述如下：

    > **XNU kernel is part of the Darwin operating system** for use in macOS and iOS operating systems. XNU is an acronym for X is Not Unix. XNU is **a hybrid kernel combining the Mach kernel** developed at Carnegie Mellon University **with components from FreeBSD and a C++ API** for writing drivers called IOKit. XNU runs on x86_64 and ARM64 for both single processor and multi-processor configurations.

### 2.10 调试操作系统

本节主要介绍 BCC/eBPF。

## 第六章：进程同步工具

当共享的数据访问不受控制时，存在竞争条件，可能导致数据不一致。进程同步指的是使用一些工具控制对共享数据的访问，以避免竞争条件。

### 6.1 背景

进程的并发和并行执行使得下面的简单程序都可能产生不同的结果：

```c
int count = 0;
// 进程 1
count = count + 1;
// 进程 2
count = count - 1;
```

### 6.2 临界区

临界区问题：当一个进程进入临界区时，其他进程不能进入。

临界区问题的解需要满足三个条件：

- Mutual exclusion（互斥）：如果一个进程在临界区内，其他进程不能进入。
- Progress（进展）：如果没有进程在临界区内，等待进入的进程最终能进入。
- Bounded waiting（有界等待）：在一个进程请求进入临界区和进入临界区之间，其他进程的进入次数是有限的。

在单核系统上，可以通过禁用中断来解决临界区问题，使得进程在临界区内时不会被抢占。在多核系统上无法这样做的原因有两点：

- 慢。消息需要传到所有核心。
- 如果系统时钟依靠中断，会影响系统时钟。

### Peterson 算法

Peterson 算法在软件层面解决两个进程的临界区问题。

```c
int turn;           // 现在轮到谁进入临界区
boolean flag[2];    // 是否准备好进入临界区

while(true)
{
    flag[i] = true; // 我已准备好进入临界区
    turn = j;       // 先让对方进入临界区
    while(flag[j] && turn == j);    // 等待对方离开临界区
    // 临界区
    flag[i] = false;
    // 余下部分
}
```

- 互斥：
    - 如果进程进入临界区，那么要么 `flag[j] == false`，要么 `turn == i`。
    - 注意到进入临界区时 `flag[i] = true`。
    - 如果两个进程都进入临界区，则有 `flag[0] == flag[1] == true`，那么一定是 `turn == i && turn == j`，矛盾。
- 进展：
    - 如果进程在等待，说明 `flag[j] == true && turn == j`。
    - 如果另一个进程退出临界区，那么 `flag[j] = false`，等待的进程可以进入。
- 有界等待：
    - 如果另一个进程重新设置 `flag[j] = true`（即再次进入临界区），必定同时设置 `turn = i`。
    - 此时等待的进程在 while 循环中，不会改变 `turn`，所以可以进入。
    - 证明了在进程请求进入临界区和进入临界区之间，其他进程最多进入一次。

### 6.4 硬件支持同步

#### 内存屏障

现代计算机体系结构中，内存模型可以分为两类：

- Strongly ordered：一个处理器对内存的修改立刻对其他处理器可见。
- Weakly ordered：一个处理器对内存的修改不立刻对其他处理器可见。

Memory barriers（也称为 fences）是一种硬件指令，用于强制对内存的修改立刻对所有处理器可见。这可以避免指令重排产生的问题。

#### 硬件指令

现代计算机指令集提供一些原子指令，比如 `test_and_set`、`compare_and_swap`。可以用它们方便地解决临界区问题。

TS 锁：

```c
boolean test_and_set(boolean *lock)
{
    boolean old = *lock;
    *lock = true;
    return old;
}

do {
    while(test_and_set(&lock)); // 当 lock 为 false 时，将取得锁并进入临界区
    // 临界区
    lock = false;
    // 余下部分
} while(true);
```

CAS 锁：

```c
int compare_and_swap(int *lock, int old, int new)
{
    int temp = *lock;
    if(temp == old)
        *lock = new;
    return temp;
}

while(true)
{
    while(compare_and_swap(&lock, 0, 1) != 0);  // 当 lock 为 0 时，将取得锁并进入临界区
    // 临界区
    lock = 0;
    // 余下部分
}
```

上面两段只实现了互斥，没有实现有界等待。下面的算法可以实现有界等待：

```c
// 共享数据
boolean waiting[n]; // n 个进程，初始化为 false
int lock;           // 初始化为 0

while(true){
    waiting[i] = true;  // 我在等待
    key = 1;
    while(waiting[i] && key == 1)
        key = compare_and_swap(&lock, 0, 1);    // 尝试取得锁
    waiting[i] = false;
    // 临界区
    j = (i + 1) % n;    // 下一个进程
    while(j != i && !waiting[j])    // 找到下一个等待的进程
        j = (j + 1) % n;
    if(j == i)
        lock = 0;
    else
        waiting[j] = false;
}
```

- 互斥：
    - 如果进程进入临界区，那么要么 `waiting[j] == false`，要么 `key == 0`。
    - 只有第一个执行 CAS 的进程能够成立 `key == 0`。
    - 只有其他进程离开临界区时，才可能使 `waiting[j] = false`，并且只使一个 `waiting[j] = false`。
- 进展：
    - 进程退出临界区时，要么将 `waiting[j] = false`，要么将 `lock = 0`，都能使等待的进程进入。
- 有界等待：
    - 进程退出临界区时，按序找到下一个等待的进程。因此每个进程最多等待 $n-1$ 次。

#### 原子变量

大多数支持原子变量的系统提供了原子类型，底层使用 CAS 等实现。例子：

```c
void increment(atomic_int *v)
{
    int temp;
    do {
        temp = *v;
    } while(temp != compare_and_swap(v, temp, temp + 1));   // 不断尝试获取锁
}
```

### 6.5 互斥锁

硬件支持的同步对应用开发者来说太难用。操作系统包装好互斥锁，提供了更高层次的抽象。

互斥锁 `mutex` 有成员 `available` 和操作 `acquire()`、`release()`，这两个操作都是原子的（可以用 CAS 实现）。

```c
acquire()
{
    while(!available);
    available = false;
}

release()
{
    available = true;
}
```

上面这种互斥锁称为自旋锁（spin lock），因为它需要忙等（busy waiting）。

实践中，spin lock 最好用在临界区很短的情况下（比相邻两次进程上下文切换的时间短）。

### 6.6 信号量

Semaphore 只能被两种原子操作 `wait()`/`P()` 和 `signal()`/`V()` 修改。前者减少信号量，后者增加信号量。

```c
wait(S)
{
    while(S <= 0);
    S--;
}

signal(S)
{
    S++;
}
```

信号量分为 count 和 binary 两种。

## 第八章：死锁

多个 thread 竞争有限资源。如果一个线程请求的资源不可用，它会进入等待状态。有时它永远无法退出等待，因为资源被其他等待的线程占用，这就是死锁。

!!! note "定义：死锁"

    一个进程集合中的每个进程都在等待一个事件，这个事件只能由集合中的其他进程引发。

操作系统一般不提供死锁预防（prevention）机制，要求程序员设计不死锁（deadlock-free）的程序。

!!! abstract "本章目标"

    - 演示使用互斥锁（mutex）时死锁如何发生。
    - 定义死锁的四个必要条件。
    - 在资源等待图（resource-allocation graph）中识别死锁。
    - 评估四种预防死锁（deadlock preventing）的方法。
    - 使用银行家（banker's）算法避免死锁（deadlock avoidance）。
    - 使用死锁检测（deadlock detection）算法。
    - 评估从死锁中恢复（deadlock recovery）的方法。

### 8.1 系统模型

本书只讨论内核中资源的死锁。对于应用程序，死锁的资源可能位于其他进程，涉及进程间通信等。

- **资源（resource）**可以按种类划分。每种资源有若干个实例，其中的每一个实例都可以满足对一种资源的请求。
    - 进程同步工具（mutex、semaphore 等）也是资源，并且**它们是最常见的导致死锁的资源**。
- 线程使用资源的步骤：请求（request）、使用（use）、释放（release）。如果请求时无法立即分配，线程必须等待。
    - 请求和释放一般都是系统调用，如文件的 `open()` 和 `close()`、内存的 `allocate()` 和 `free()`、信号量的 `wait()` 和 `signal()`、互斥锁的 `acquire()` 和 `release()`。

本书考虑的死锁时等待的事件一般是资源的请求和释放。此外死锁可能等待其他时间，比如从网络接口或 IPC 接收数据等。

!!! example

    哲学家就餐问题中，如果所有哲学家都拿起自己右手边的筷子，就进入了死锁状态。每一位哲学家都必须等待另一位哲学家放下筷子。

### 8.2 多进程应用中的死锁

本节通过 Pthread 介绍进程中的死锁。

```c
pthread_mutex_t mutex1, mutex2;
// 初始化为 unlocked
pthread_mutex_init(&mutex1, NULL);
pthread_mutex_init(&mutex2, NULL);
// 线程 1
void *thread1(void *arg)
{
    // 该函数将阻塞，直到 mutex 可用
    pthread_mutex_lock(&mutex1);
    pthread_mutex_lock(&mutex2);
    // do something
    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);
}
// 线程 2
void *thread2(void *arg)
{
    pthread_mutex_lock(&mutex2);
    pthread_mutex_lock(&mutex1);
    // do something
    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);
}
```

死锁究竟会不会发生，还取决于线程调度器的行为。

!!! note "定义：活锁（livelock）"

    活锁与死锁都意味着进程无法继续执行，但不同在于：活锁不是进程因为**等待**而无法继续执行，而是进程**不断重复相同的操作但始终失败**。

    活锁通常可以通过让进程延迟**随机的时间再次尝试**来避免。在以太网中，碰撞检测就是通过随机时延避免活锁的。

    !!! example

        两个人在走廊中相遇，一人总是向右让，另一人总是向左让，两人始终无法通过。

        `pthread_mutex_trylock()` 可以非阻塞地尝试获取锁，可以演示活锁的发生：

        ```c
        void *thread1(void *arg)
        {
            int done = 0;
            while(!done)
            {
                pthread_mutex_lock(&mutex1);
                if(pthread_mutex_trylock(&mutex2) == 0){
                    pthread_mutex_unlock(&mutex2);
                    pthread_mutex_unlock(&mutex1);
                    done = 1;
                }
                else
                    pthread_mutex_unlock(&mutex1);
            }
        }
        // 线程 2 同理
        ```

### 8.3 死锁特征

!!! note "死锁的必要条件"

    如果下面四个条件在系统中同时成立，那么死锁可能发生：

    - **互斥（mutual exclusion）**：至少有一个资源以非共享方式使用。
    - **占有并等待（hold and wait）**：一个进程持有一个资源，并且希望获取正在被其他进程持有的资源。
    - **无抢占（no preemption）**：资源只能在进程使用完后才能释放，不允许抢占。
    - **循环等待（circular wait）**：存在一个进程等待链，每个进程都在等待下一个进程持有的资源。$\{T_0, T_1, \ldots, T_n\}$。
        - 这隐含了占有并等待。

!!! note "资源分配图（resource-allocation graph）"

    - **顶点 $V$**分为两类：线程 $T$ 用圆形表示，资源 $R$ 用方形表示。如果资源有多个实例，用方形中的点表示。
    - **边 $E$** 也有两类：
        - **请求边（request edge）** $T_i \rightarrow R_j$
        - **分配边（assignment edge）** $R_j \rightarrow T_i$


    使用资源的三个步骤对应图中的三个操作：建立请求边，请求边变为分配边，删除分配边。

!!! note "识别 RAG 中的死锁"

    - 如果每个资源都只有一个实例，则分配图中的环表明死锁发生。这种情况既是充分（sufficient）也是必要（necessary）条件。
    - 如果资源有多个实例，那么环是必要条件，但不是充分条件。
    - 总之：环是死锁的必要条件。

### 8.4 处理死锁的方法

概括地说有三种：

- 假装死锁永远不会发生。
- 使用**协议（protocol）**预防死锁，保证系统永远不会进入死锁状态。

    !!! tip

        还记得数据库系统课上的**两阶段锁协议**吗？

- 允许系统进入死锁状态。检测（detect）并恢复（recover）。

第一种方法被大多数操作系统使用（包括 Linux 和 Windows）。第二种方法被内核和程序开发者使用。第三种方法在数据库系统中使用。

为了保证死锁永远不会发生：

- **死锁预防（deadlock prevention）**破坏死锁的四个必要条件中的至少一个。一般通过限制对资源的请求实现。
- **死锁避免（deadlock avoidance）**向操作系统提供额外的信息，即线程在生命周期中将要请求的资源及其数量。操作系统根据这些信息**决定线程是否需要等待**。操作系统需要为每个线程考虑：当前分配（allocated）的资源、未来的请求（request）和释放（release）。

### 8.5 死锁预防

从死锁的四个条件入手：

- **互斥**：该条件必须成立，因为非共享资源本来就没法在多个进程间共享。
- **占有并等待**：保证**当线程请求资源时，不持有其他资源**。
    - 一种协议：线程在开始执行前请求并分配得到所有资源。然而对大部分应用来说不可行，它们都需要动态分配资源。
    - 另一种协议：线程在请求资源时**释放已经持有的资源**。

    两种协议都会导致资源利用率降低，且可能产生饥饿，因为等待较多资源的线程可能永远无法满足。

- **无抢占**：
    - 一种协议：当线程持有资源，并且要请求其他**不能被立即分配**的资源（必须等待）时，它自己持有的资源应当可以被抢占（可以理解为隐式地释放了）。当资源被抢占后，应当加入其请求的资源队列。
    - 另一种协议：
        - 首先检查线程请求的资源是否可用，可用就分配。
        - 不可用，则检查是否被分配到其他在等待更多资源的线程。如果是，就从等待的线程抢占资源。
        - 如果没有，就让线程等待。此时它的资源可以被抢占。

    这些协议往往用在容易保存和恢复状态的资源，比如 CPU 寄存器和数据库事务。

- **循环等待**：四个条件中最能实践的一种条件。

    主要思想是**线性化资源**，即给资源编号。考虑函数 $F: R \rightarrow N$：

    - 一种协议：线程只能按编号递增的顺序请求资源 $F(R_j) > F(R_i)$。
    - 另一种协议：要求线程在请求资源 $R_i$ 时，必须释放编号更大的资源 $F(R_j) \geq F(R_i)$。

    编号是针对资源的类别。对于同种资源，必须用一个请求请求所有实例。

    利用反证法可以证明使用这种协议可以避免循环等待。

    !!! info "证明"

        - 假设发生了循环等待，其中 $T_i$ 等待资源 $R_i$，被 $T_{i+1}$ 持有。
        - 因为 $T_{i+1}$ 持有 $R_i$ 并且在等待 $R_{i+1}$，所以 $F(R_{i+1}) > F(R_i)$。
        - 依此类推，得到 $F(R_0) < F(R_1) < \ldots < F(R_n) < F(R_0)$，矛盾。

    !!! warning

        编程时有几个难点：

        - 资源数量非常多时，难以建立编号并保证按顺序请求。
        - 资源动态分配时，编号可能会变化。

        下面的代码就会产生死锁：

        ```c
        // 两个线程
        transaction(checking account, savings account, 25.0);
        transaction(savings account, checking account, 50.0);
        void transaction(Account from, Account to, double amount)
        {
            mutex lock1, lock2;
            lock1 = get lock(from);
            lock2 = get lock(to);
            acquire(lock1);
                acquire(lock2);
                    withdraw(from, amount);
                    deposit(to, amount);
                release(lock2);
            release(lock1);
        }
        ```

### 8.6 死锁避免

!!! info

    在 Linux 中可以使用 `lockdep` 调试、检测死锁。

- **安全状态（safe state）**：如果系统可以按**某种顺序**分配资源，同时保证不会进入死锁状态，那么系统处于安全状态。
    - 安全状态意味着死锁一定不会发生。不安全状态意味着死锁可能发生，但不一定会发生。
    - 序列写为 $<T_1, T_2, \ldots, T_n>$。对于每个 $T_i$，有两种意思：前面的线程的资源已经满足后，仍然能满足 $T_i$ 的资源请求（就是 $T_i$ 不需要等待）；前面的线程的资源已经满足后执行完毕，释放资源后，然后可以满足 $T_i$ 的资源请求（就是 $T_i$ 需要等待）。
    - 死锁避免算法就是要保证系统始终处于安全状态。

#### 资源分配图算法

适用于每种资源只有一个实例的情况。

向资源分配图中添加一种边：**需求边（request edge）** $T_i \rightarrow R_j$，为虚线。表示线程 $T_i$ 对 $R_j$ 有需求，但尚未请求。

- 当线程请求资源时，将需求边变为请求边。
- 当线程释放资源时，将请求边变为需求边。

在线程开始执行时，就必须告知操作系统所有需求，在图中绘制所有需求边。

!!! note "资源分配图算法"

    当线程请求资源（将**需求边** $T_i \rightarrow R_j$ 变为**请求边**）时，使用环检测算法（$n^2$）检查：将该**请求边**转换为**分配边** $R_j \rightarrow T_i$ 时是否会导致环。

    如果会，那么就会让系统进入不安全状态，需要让线程等待。否则，就可以分配资源。

#### 银行家算法

适用于每种资源有多个实例的情况。

设 $n$ 为线程数，$m$ 为资源种类数。需要维护下列数据结构：

- **可用向量** $Avail$：表示系统中每种资源的可用数量。
- **最大需求**矩阵 $Max$：$n \times m$，表示每个线程对每种资源的最大需求。
- **分配矩阵** $Alloc$：$n \times m$，表示每个线程已经分配的资源。其中的每一列表示该进程已经分配各类资源的数量。
- **需求矩阵** $Need$：$n \times m$，表示每个线程还需要的资源。每一列表示该进程还需要各类资源的数量。

算法如下：

- **安全算法**：检查系统是否处于安全状态。复杂度 $O(n^2 \times m)$。
    1. 令 $Work = Avail$，$Finish[i] = false$ 表示进程是否结束。
    2. 找到一个进程 $i$，满足 $Finish[i] = false$ 且 $Need_i \leq Work$。即寻找尚未结束的进程，且它的需求小于等于当前可用资源。如果找不到，转到步骤 4。
    3. $Work = Work + Alloc_i$，$Finish[i] = true$。即为该进程分配资源，并等待进程结束。转到步骤 2。
    4. 如果所有进程都结束，那么系统处于安全状态。否则，系统处于不安全状态。

!!! note

    一句话总结安全算法：遍历所有进程，寻找可行的资源分配序列。

- **资源分配算法**：决定资源能否被安全地分配。
    1. 令 $Request_i$ 为线程 $i$ 对资源的请求。若 $Request_i \leq Need_i$，进入下一步，否则报错，请求超过最大需求。
    2. 若 $Request_i \leq Avail$，进入下一步，否则线程 $i$ 必须等待。
    3. 假设系统分配资源给线程 $i$，即 $Avail = Avail - Request_i$，$Alloc_i = Alloc_i + Request_i$，$Need_i = Need_i - Request_i$。然后运行安全算法。

!!! note

    一句话总结资源分配算法：试探性地分配资源，然后运行安全算法。

### 8.7 死锁检测

### 做题笔记

- 至少要多少资源才能保证死锁不发生？见 [[Solved] What is the minimum number of resources required to ensure t](https://testbook.com/question-answer/what-is-the-minimum-number-of-resources-required-t--6076f2c44a5b2634735c3716)。思路是首先让所有进程都持有一个资源，然后计算每个进程需要的资源（即 $max - 1$）。只要比所有进程的 $max - 1$ 求和再多一个资源，就有让死锁不发生的空间。