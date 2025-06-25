# C++ 并发编程

!!! abstract

    本篇笔记关注 C++ 标准提供的并发编程功能。以《C++ 并发编程实战（第 2 版）》为主线。

    - **中文版：**[C++ 并发编程实战（第 2 版）](https://book.douban.com/subject/35653912/)
    - **英文版：**[C++ Concurrency in Action, Second Edition](https://www.oreilly.com/library/view/c-concurrency-in/9781617294693/)

## 附录 A：C++ 11 精要：部分语言特性

### 右值引用

!!! quote

    - [A Brief Introduction to Rvalue References - Bjarne Stroustrup](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2006/n2027.html)

```cpp
A& a_lref = a; // lvalue reference
A&& a_rref = a; // rvalue reference
```

我们知道：

- **非 const 左值引用**不能绑定到右值。
- **右值引用**绑定到临时对象（右值），而不绑定到左值，这就是它的名字的由来。、

### 移动语义

右值引用的诞生是为了实现**移动语义（move semantics）**，即将资源从一个对象转移到另一个对象，而不是拷贝。在 `swap()` 操作中，我们可以看到这种效果。

```cpp
template <class T> swap(T& a, T& b) {
    T tmp = std::move(a);
    a = std::move(b);
    b = std::move(tmp);
}
```

**如果不使用 `move()`，则每一步都会调用拷贝构造函数**，产生 `a`、`b` 和 `tmp` 各自的副本。而使用 `move()`，则不会产生任何副本。

### `std::move()`

接下来看 `move()` 如何实现：接受一个左值或右值引用，返回一个右值引用。**这个过程不会调用拷贝构造。**

```cpp
template <class T>
typename remove_reference<T>::type&&
move(T&& a)
{
    return a;
}
```

那么 `swap()` 的关键就在于客户端如何实现：**对左值和右值重载拷贝构造和赋值运算符。当传入左值时，应该拷贝；当传入右值时，可以安全地移动。**对比下面的拷贝和移动语义实现：

```cpp
// copy semantics
clone_ptr(const clone_ptr& p)
    : ptr(p.ptr ? p.ptr->clone() : 0) {}

clone_ptr& operator=(const clone_ptr& p)
{
    if (this != &p)
    {
        delete ptr;
        ptr = p.ptr ? p.ptr->clone() : 0;
    }
    return *this;
}

// move semantics
clone_ptr(clone_ptr&& p)
    : ptr(p.ptr) {p.ptr = 0;}

clone_ptr& operator=(clone_ptr&& p)
{
    std::swap(ptr, p.ptr);
    return *this;
}
```

并对比下面两段使用代码：

```cpp
clone_ptr<base> p1(new derived);
// ...
clone_ptr<base> p2 = p1;  // p2 and p1 each own their own pointer
clone_ptr<base> p1(new derived);
// ...
clone_ptr<base> p2 = std::move(p1);  // p2 now owns the pointer intead of p1
```

下面是一些可移动但不可拷贝的类型：

- `fstream`、`unique_ptr`
- 线程

使用这些类型时：

- 始终只有一个底层对象
- 该类型的容器只能移动，拷贝（如 `v2 = v1`）造成编译错误。

### 完美转发

此外，右值引用还实现了**完美转发（perfect forwarding）**，即将参数传递给另一个函数，不改变其左值或右值属性。这涉及比较复杂的模板参数推导，暂且不表。

- 尽管右值引用的形参与传入的右值实参绑定，但进入函数后参数即**被当作左值**处理。因为在 C++ 中，具有名字的东西都是左值。
- 关于函数模板：如果参数是右值引用

    ```cpp
    template <class T>
    void f(T&& a);
    ```

    - 传入左值，模板参数推导为左值引用 `T` -> `T&`，使得 `T&&` 成为引用的引用，发生**引用折叠（reference collapsing）**，成为左值引用 `T&`。
    - 传入右值，模板参数推导为**无修饰类型** `T` -> `T`，使得 `T&&` 成为右值引用。

### lambda 表达式

`condition_variable` 的 `wait()` 函数中广泛使用 lambda 表达式，利用它捕获外部调用语境中的本地变量。这样避免另外设计带有函数调用操作符的类，并借助成员变量捕获必须的状态。

```cpp
[]{ return 42; }();
[]() -> int { return 42; };
[=](int a) { return a + x; };
[=, &x](int a) { return a + x; };
```

### （C++ 17）类模板参数推导（class template argument deduction）

对于简单的类模板，可以**省略模板参数**，由编译器推导。

```cpp
std::pair p(1, 2);  // std::pair<int, int>
std::vector v{1, 2, 3};  // std::vector<int>
```

## 第一章：你好，C++ 并发世界

略。

## 第二章：线程管控

### 线程的基本管控

- 在 `thread` 对象被销毁前，应当决定线程汇合或分离，否则析构函数调用 `terminate()` 终止程序，引发异常。
    - 一旦调用 `join()`，属于线程的存储空间被清空，`thread` 也不再关联到线程。
    - 如果线程启动后抛出异常，可能导致 `join()` 未被调用。最优的方案是使用 RAII 技术，将线程对象封装在一个类 `thread_guard` 中，析构函数中调用 `join()`。

    !!! note "RAII"

        - **Resource Acquisition Is Initialization**，资源获取即初始化。
        - 通过构造函数获取资源，通过析构函数释放资源，确保资源在任何情况下都能被正确释放。

- 生存期问题。一般让线程函数 self-contain，将所需数据复制到线程内部。

!!! warning "C++ 最麻烦的解释：只要 C++ 语句有可能被解释成函数声明，编译器就一定将其解释为函数声明"

    ```cpp
    std::thread my_thread(background_task());
    ```

    声明一个函数 `my_thread`，它的返回值是 `std::thread`，参数是一个函数指针，没有参数传入，返回 `background_task` 对象。

    可以使用列表初始化或增加一对括号。

### 向线程函数传递参数

- `thread` 的构造函数只是简单地拷贝实参到新线程的空间，将其当作右值传给新线程上的可调用对象。
    - **`thread` 的构造函数对可调用对象的参数类型并不知情**，参数转换发生在进入新线程上下文后。
    - 如果传递指针作为实参，可能导致转换发生时指针所指的对象已被销毁，需要提前完成转换。

        ```cpp
        thread t(string(buffer));
        ```

    - 如果可调用对象的参数是引用，通过上面的方式它将得到以右值形式传递的参数的副本，导致编译失败。应当使用 `ref()` 包装引用。
    - 如果需要移动对象（比如 `unique_ptr`），可以使用 `move()`。

- 使用 `get_id` 获取线程 ID，使用 `this_thread::get_id()` 获取当前线程 ID。

## 第三章：在线程间共享数据

### 不变量 (invariant)

可以将不变量理解为：**当数据结构处于正常状态时总是成立的断言**。比如，对于双向链表，不变量为：

- 如果从节点 A 的 `next` 指针可以到达节点 B，则节点 B 的 `prev` 指针可以到达节点 A。

对数据结构进行操作时可能破坏不变量，但应当维护操作前后不变量成立。

在并发程序设计中，一个线程进行操作时，另一个线程可能看见不变量被破坏的中间状态，产生问题。

### 条件竞争（race condition）

结果取决于线程执行的顺序，就说明存在条件竞争。典型场景：完成一项操作需要改变多份不同的数据，如双向链表需要改变 `prev` 和 `next` 指针。

有三种机制防止条件竞争：

- 使用互斥保护数据结构：对于其他访问的线程来说，改动要么尚未开始，要么全部完成。
- 使用无锁数据结构：深入理解内存模型，用不可拆分的改动完成数据变更，每个改动都保持不变量成立。
- 使用软件事务内存（STM）：类似数据库事务，仍在研究中。

### 互斥（mutual exclusion，mutex）

互斥是一种**同步原语**，访问数据结构前需要先锁住相关的互斥。当互斥被锁住时，其他线程要加锁则必须等待。

- 构造 `mutex` 实例创建互斥
- 调用成员函数 `lock()` 和 `unlock()` 锁住和解锁互斥

不推荐按上面的方式使用，否则必须确保在所有代码路径都要调用 `unlock()`，包括异常。

!!! note "类模板 `lock_guard<>`"

    对互斥类使用 RAII 手法，保证互斥总是正确解锁。

    ```cpp
    std::mutex m;
    void f() {
        std::lock_guard guard(m);
        // ...
    }
    ```

### 常见的线程安全问题

- **不得向锁的作用域之外传递指针和引用**，指向受保护的共享数据，无论是：
    - 通过函数返回值将它们保存到对外可见的内存
    - 将它们作为参数传递给**使用者提供的函数**
- **接口固有的条件竞争**：栈的操作都是线程安全的，但存在两个固有条件竞争
    - `empty()` 和 `size()` 一旦返回，结果便不再可信
    - `top()` 和 `pop()` 操作分离

    ```cpp
    if (!data.empty()) {
        T const& value = data.top(); // 可能为空
        data.pop(); // pop 的不一定是 top 的值
        return value;
    }
    ```

    本质上来说，这是因为锁的控制粒度太小，**需要保护的操作未被完整覆盖。**

    为了设计线程安全的栈，**需要简化接口、合并操作**。书本选择删去了 `top()`，并给出两种 `pop()` 操作：

        - `shared_ptr<T> pop()` **返回指针**。缺点：产生内存管理开销。一般用 `shared_ptr`。
        - `void pop(T& val)` **传入外部变量接收**栈容器弹出的元素。缺点：要求类型可赋值。

!!! info "为什么 `stack` 被设计为 `top()` 和 `pop()` 分离？"

    如果只有 `pop()` 函数：当栈的改动完成后，元素被返回给调用者。但拷贝数据的过程中可能遇到构造函数发生 `bad_alloc` 异常等情况，此时**数据已从栈上移除，但复制却未成功，导致数据丢失**。

    分离 `top()` 和 `pop()` 操作，则可以安全地复制数据，再将其从栈上移除。

### `lock()` 和 `scoped_lock<>` 同时锁住多个互斥避免死锁

某些操作可能需要同时锁住多个互斥，但如果加锁顺序不当，可能导致死锁。

`lock()` 能够同时锁住多个互斥，没有发生死锁的风险。然后可以用 `lock_guard (m, adopt_lock)` 传入已锁住的互斥。

```cpp
std::lock(m1, m2);
std::lock_guard<std::mutex> lock1(m1, std::adopt_lock);
```

!!! note "（C++ 17）可变参数模板 `scoped_lock<>`"

    与 `lock_guard` 等价，可以接收多个互斥对象构造。

    ```cpp
    std::mutex m1, m2;
    std::scoped_lock lock(m1, m2);
    ```

其他避免死锁的准则：

1. 避免嵌套锁，不要获取第二个锁。
2. 一旦持有锁，就必须避免调用用户提供的程序接口。
3. 按固定顺序获取锁。
4. 按层级加锁，自行实现一个 `hierarchical_mutex`。

### `unique_lock<>` 灵活加锁

!!! note "类模板 `unique_lock<>`"

    不一定始终占有与之关联的互斥。可以传入：

    - `adopt_lock`：管理互斥上的锁。
    - `defer_lock`：不立即锁住互斥，构造完成后处于无锁状态。再其上调用 `.lock()` 或传递给 `lock()` 加锁。

    比 `lock_guard` 占用空间更多也略慢。

    属于可移动但不可复制类型，能够转移互斥的归属权。

    ```cpp
    std::unique_lock<std::mutex> get_lock() {
        std::unique_lock<std::mutex> lk(m);
        prepare_data();
        return lk;
    }

    std::unique_lock<std::mutex> lk(get_lock()); // 移动构造
    ```

### `call_once()` 和 `once_flag` 保护初始化过程

!!! info "场景"

    所有线程需要某个共享数据，创建的开销较大，需要延迟初始化，只需要初始化一次。

!!! bug "错误做法"

    所有线程被迫按序等待。

    ```cpp
    std::unique_lock<std::mutex> lk(m);
    if (!ptr) {
        ptr.reset(new data_type);
    }
    lk.unlock();
    ptr->do_something();
    ```

!!! note "`call_once()` 和 `once_flag`"

    所有线程共同调用 `call_once(once_flag, f)`，保证 `f` 只被安全且唯一地调用。

    `once_flag` 用于标记是否已经调用过 `f`。

    `f` 的参数可以附加在 `call_once()` 后面。

    与 `mutex` 一样，`once_flag` 不可移动也不可复制。

### 共享和排他锁

- `shared_mutex`：读写锁，允许多个线程同时读取，但只允许一个线程写入。
- 读者使用 `shared_lock`，写者使用 `lock_guard` 或 `unique_lock`。

### `recursive_mutex` 递归锁

## 第四章 前半：并发操作的同步

对于需要等待事件的多线程任务，条件变量会比 `sleep_for` 等待更高效。

### `condition_variable` 等待条件成立

两种实现：

- `condition_variable`：仅能与 `mutex` 一起使用。
- `condition_variable_any`：可与任何互斥一起使用，可能产生额外开销。

条件变量需要与互斥配合使用：

```cpp hl_lines="5 10"
std::mutex m;
std::condition_variable cv;
void prepare() {
    // ...
    cv.notify_one();
}
void process() {
    while(true){
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, []{return data_ready;});
        // fetch data
        lk.unlock();
        // process data
    }
}
```

!!! note "`std::condition_variable::wait()`"

    `wait()` 有两个重载版本：

    ```cpp
    void wait( std::unique_lock<std::mutex>& lock );

    template< class Predicate >
    void wait( std::unique_lock<std::mutex>& lock, Predicate pred )
    ```

    1. **原子地释放互斥锁并阻塞当前线程**，直到被通知。
    2. 等价于 `while (!pred()) wait(lock);`。即在等待期间，忽略 `pred` 返回 `false` 的通知。

    !!! warning "因为 `wait()` 会释放互斥锁，所以条件变量需要与 `unique_lock` 而不是 `lock_guard` 一起使用。"

### TODO 实例：简单的并发安全队列

**分析接口**：

- 查询整体：`empty()`、`size()`
- 查询元素：`front()`、`back()`
- 修改：`push()`、`pop()`、`emplace()`

**设计**：

合并 `front()` 和 `pop()`，提供 `try_pop()` 和 `wait_and_pop()`。

```cpp
private:
    std::queue<T> data_queue;
    mutable std::mutex m;
    std::condition_variable cv;
public:
    void push(T new_value) {
        std::lock_guard<std::mutex> lk(m);
        data_queue.push(new_value);
        cv.notify_one();
    }
    void wait_and_pop(T& value) {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [this]{return !data_queue.empty();});
        value = data_queue.front();
        data_queue.pop();
    }
    bool try_pop(T& value) {
        std::lock_guard<std::mutex> lk(m);
        if (data_queue.empty()) return false;
        value = data_queue.front();
        data_queue.pop();
        return true;
    }
```

!!! note "一个需要深入理解的问题：`mutable` [c++ - Should mutexes be mutable? - Stack Overflow](https://stackoverflow.com/questions/4127333/should-mutexes-be-mutable)"

### `future<>` 等待一次性事件

两步走：

- 使用 `async()` 创建异步任务，返回 `future` 对象。`async()` 与 `thread()` 用法类似。
- 需要用到该值时，调用 `.get()` 获取结果。

    ```cpp hl_lines="1 3"
    std::future<int> f = std::async(do_work);
    do_other_work();
    std::cout << f.get() << std::endl;
    ```

两种实现，均为类模板，与智能指针类似：

- `future<>`：只能关联到一个事件。
    - 只能移动，不能复制。
    - `.get()` 仅能被有效调用一次，发生**移动操作**。如果多个线程要访问同一个 `future<>` 对象，需要采用互斥等手段保护。
- `shared_future<>`：可以关联到同一事件，可以派生多个副本。

    - 由 `future<>` 实例移动构造，或使用 `.share()` 得到。原 `future<>` 对象变为无效状态。

    - 派生的**多个副本**由各个线程独占，访问无需同步。而**同一对象**仍然需要同步。

        ```cpp
        auto sf = p.get_future().share();
        void thread1() {
            auto local = sf;
            local.wait();
        }
        ```

    - 事件发生时所有实例同时就绪，都可以访问数据。

使用 `void` 特化模板时不与任何数据关联，只用于通知事件发生。

可以为 `async()` 指定采用哪种策略，否则将自动决定：

- `std::launch::async`：立即执行
- `std::launch::deferred`：到 `.wait()` 或 `.get()` 调用时才执行，有可能永不执行

### `packaged_task<>` 包装任务

`packaged_task<>` 模板用于实现调度器、线程池等。它可以将任务包装成 `future` 对象，**从而不用纠缠形形色色的任务函数**，只需要处理 `packaged_task<>` 实例。

!!! example

    在下面的代码中，任务 `f` 包装在 `packaged_task<>` 中，放入任务队列。根据 `f` 的结果 `res` 是否被需要，相关代码将任务投递到 GUI 线程。

    ```cpp hl_lines="3-4 12 19"
    template <typename Func>
    std::future<void> post_task_for_gui_thread(Func f) {
        std::packaged_task<void()> task(f);
        std::future<void> res = task.get_future();
        std::lock_guard<std::mutex> lk(gui_thread_queue_mutex);
        gui_thread_queue.push(std::move(task));
        return res;
    }

    void gui_thread() {
        while(true) {
            std::packaged_task<void()> task;
            {
                std::lock_guard<std::mutex> lk(gui_thread_queue_mutex);
                if (gui_thread_queue.empty()) continue;
                task = std::move(gui_thread_queue.front());
                gui_thread_queue.pop();
            }
            task();
        }
    }
    ```

`packaged_task<>` 可以被：

- 直接执行：它具备函数调用操作符，是可调用对象。
- 包装在 `function<>` 中，传递给 `thread` 对象。
- 传递给需要可调用对象的函数。

### `promise<>` 和 `future<>` 配对

`promise<>`：

- `.get_future()` 获取关联的 `future` 对象。
- `.set_value()` 对 `promise` 赋值，使 `future` 就绪。


```cpp
std::promise<int> p;
std::future<int> f = p.get_future();
std::thread t([](std::promise<int>& p) {
    p.set_value(42);
}, std::ref(p));
```

### 将异常保存到 `future<>` 对象

- `async()` 调用函数抛出的异常保存到 `future<>` 对象中，在 `.get()` 时抛出。
- `promise<>.set_exception()` 也可以保存异常。
- 与 `future<>` 关联的承诺未被实现：
    - `promise<>` 未调用 `.set_value()` 或 `.set_exception()` 就被析构。
    - `packaged_task<>` 未被执行就被析构。

### TODO 限时等待

- 时钟类：
    - 提供信息：当前时刻，时间值类型，计时单元长度（tick period），是否为恒稳时钟（计时速率稳定）
    - `std::chrono::system_clock`、`steady_clock`、`high_resolution_clock`
- 时长类：`std::chrono::duration<short, std::ratio<60, 1>>`，第一个参数是计数类型，第二个参数是对秒的比例。
    - 预设的时长类：`nanoseconds`、`microseconds`、`milliseconds`、`seconds`、`minutes`、`hours`。
    - `std::chrono_literals` 命名空间提供了 `ns`、`us`、`ms`、`s`、`min`、`h` 的字面量后缀运算符。可以写 `auto one_day = 24h;`。
    - 时长转换：`std::chrono::duration_cast<>`。

延迟超时使用 `std::chrono::duration` 的实例：

```cpp
std::future<int> f = std::async(do_work);
if(f.wait_for(std::chrono::seconds(1)) == std::future_status::ready) {
    // ...
}
```

超时返回 `future_status::timeout`。

- 时点类：`std::chrono::time_point<Clock, Duration>`，`Clock` 为时钟类（参考时钟），`Duration` 为时长类（计时单元）。

## 第四章 后半：运用同步操作简化代码

### 函数式编程

函数调用的结果完全取决于参数，不依赖任何外部状态。

- lambda 表达式 和 `bind()` 实现纯函数。
- `future<>` 使一个计算任务可以依赖于另一个计算任务的结果，而不必显式访问共享数据。

### 消息传递同步



## 第五章：C++ 内存模型和原子操作

## 第六章：设计基于锁的并发数据结构

### 设计原则

- **确保访问安全：**
    - 保护不变量
    - 排除接口固有的条件竞争
    - 异常
- **实现真正的并发访问：**
    - 能否限制锁的作用域？
    - 不同部分能否采用不同互斥？
    - 是否所有操作的保护程度相同？

### 实例：采用精细粒度的锁和条件变量实现线程安全的队列容器

在简单的线程安全容器实现中，往往使用一个互斥锁保护整个标准库容器，本质上是将标准库容器视为一项大的数据。如果要进一步提高效率，我们需要深入底层的数据结构，将互斥锁的粒度降低到更小的单元。

这个例子展示了如何使用精细粒度的锁和条件变量实现线程安全的队列容器。一句话总结：仔细控制头尾指针的同步关系、精简接口，推入操作向后移动尾指针，弹出操作先锁住头指针，仅在读取时锁住尾指针。

数据结构定义如下：

```cpp
template<typename T>
class threadsafe_queue
{
private:
    struct node
    {
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;
    };
    std::mutex head_mutex;
    std::unique_ptr<node> head;
    std::mutex tail_mutex;
    node* tail;
    std::condition_variable data_cond;
public:
    threadsafe_queue():
        head(new node),tail(head.get())
    {}
    threadsafe_queue(const threadsafe_queue& other)=delete;
    threadsafe_queue& operator=(const threadsafe_queue& other)=delete;
    std::shared_ptr<T> try_pop();
    bool try_pop(T& value);
    std::shared_ptr<T> wait_and_pop();
    void wait_and_pop(T& value);
    void push(T new_value);
    bool empty();
};
```

- 队列容器一般使用链表实现，需要维护 `head` 和 `tail` 指针，分别采用互斥锁保护。
- 使用 `unique_ptr` 管理节点，自动删除不需要的节点。
- 使用 `shared_ptr` 存储数据，在复制智能指针实例时不会抛出异常，保证弹出操作的异常安全。
- 加入哑节点（dummy node），使 `push()` 和 `pop()` 不再同时操作相同的节点，实现并发。

`push()` 操作：

- 只访问 `tail` 指针
- 在不持锁的情况下创建新节点，让分配内存的操作得以并行

```cpp
template<typename T>
void threadsafe_queue<T>::push(T new_value)
{
    std::shared_ptr<T> new_data(
        std::make_shared<T>(std::move(new_value)));
    std::unique_ptr<node> p(new node);
    {
        std::lock_guard<std::mutex> tail_lock(tail_mutex);
        tail->data=new_data;
        node* const new_tail=p.get();
        tail->next=std::move(p);
        tail=new_tail;
    }
    data_cond.notify_one();
}
```

弹出操作需要用到两个私有接口，它们将访问和加锁包装成一个函数：

```cpp
node* get_tail()
{
    std::lock_guard<std::mutex> tail_lock(tail_mutex);
    return tail;
}
std::unique_ptr<node> pop_head()
{
    std::unique_ptr<node> old_head=std::move(head);
    head=std::move(old_head->next);
    return old_head;
}
```

`try_pop()` 操作：

- 需要访问 `tail`，但**只需在临近读取时加锁即可**，未读取时不阻塞 `push()` 操作
- `get_tail()` 应当在 `head_mutex` 的保护下调用
- `unique_ptr<node>` 的析构函数在 `head_mutex` 的保护范围外，使得多个 `try_pop()` 可以安全删除各自的节点，增加并行度

```cpp
template<typename T>
class threadsafe_queue
{
private:
    // 两个私有接口负责加锁和比较
    std::unique_ptr<node> try_pop_head()
    {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        if(head.get()==get_tail())
        {
            return std::unique_ptr<node>();
        }
        return pop_head();
    }
    std::unique_ptr<node> try_pop_head(T& value)
    {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        if(head.get()==get_tail())
        {
            return std::unique_ptr<node>();
        }
        value=std::move(*head->data);
        return pop_head();
    }
public:
    // 公有接口负责返回值等的处理
    std::shared_ptr<T> try_pop()
    {
        std::unique_ptr<node> old_head=try_pop_head();
        return old_head?old_head->data:std::shared_ptr<T>();
    }
    bool try_pop(T& value)
    {
        std::unique_ptr<node> const old_head=try_pop_head(value);
        return old_head!=nullptr;
    }
    bool empty()
    {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        return (head.get()==get_tail());
    }
};
```

`wait_and_pop()`：

```cpp
template<typename T>
class threadsafe_queue
{
private:
    std::unique_lock<std::mutex> wait_for_data()
    {
        std::unique_lock<std::mutex> head_lock(head_mutex);
        data_cond.wait(head_lock,[&]{return head.get()!=get_tail();});
        return std::move(head_lock);
    }
    std::unique_ptr<node> wait_pop_head()
    {
        std::unique_lock<std::mutex> head_lock(wait_for_data());
        return pop_head();
    }
    std::unique_ptr<node> wait_pop_head(T& value)
    {
        std::unique_lock<std::mutex> head_lock(wait_for_data());
        value=std::move(*head->data);
        return pop_head();
    }
public:
    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_ptr<node> const old_head=wait_pop_head();
        return old_head->data;
    }
    void wait_and_pop(T& value)
    {
        std::unique_ptr<node> const old_head=wait_pop_head(value);
    }
};
```

## 第九章：高级线程管理

### 线程池

将可同时执行的任务提交到线程池，放入任务队列等待，由工作线程领取并执行。

```cpp
class thread_pool
{
    std::atomic_bool done;
    thread_safe_queue<std::function<void()>> work_queue;
    std::vector<std::thread> threads;
    join_threads joiner;

    void worker_thread()
    {
        while (!done)
        {
            std::function<void()> task;
            if (work_queue.try_pop(task))
            {
                task();
            }
            else
            {
                std::this_thread::yield();
            }
        }
    }
}
```

### 中断线程

在适当的位置安插中断点 `interruption_point()`，检查中断标志。但是这并不理想：我们往往需要在线程被阻塞时将其中断，此时无法执行中断点。引入新函数 `interruptible_wait()`，在等待时检查中断标志。

```cpp
class interrupt_flag
{
public:
    void set();
    bool is_set() const;
};
thread_local interrupt_flag this_thread_interrupt_flag;    ⇽---  ①
class interruptible_thread
{
    std::thread internal_thread;
    interrupt_flag* flag;
public:
    template<typename FunctionType>
    interruptible_thread(FunctionType f)
    {
        std::promise<interrupt_flag*> p;     ⇽---  ②
        internal_thread=std::thread([f,&p]{    ⇽---  ③
                p.set_value(&this_thread_interrupt_flag);
                f();    ⇽---  ④
            });
        flag=p.get_future().get();    ⇽---  ⑤
    }
    void interrupt()
    {
        if(flag)
        {
            flag->set();    ⇽---  ⑥
        }
    }
};

void interruption_point()
{
    if(this_thread_interrupt_flag.is_set())
    {
        throw thread_interrupted();
    }
}
```

## 第十章：并行算法函数

C++ 17 提供了并行算法函数。与串行版本相比，新增了第一个参数，用于设定执行策略：

```cpp
std::sort(std::execution::par, my_data.begin(), my_data.end());
```

其他不同：

- 如果串行版本接受输入/输出迭代器，则并行版本只接收前向迭代器。

    前向迭代器的递增操作不会令副本失效，使得各线程可以分别操作自己的前向迭代器。

`<algorithm>` 和 `<numeric>` 中的大多数算法都有并行版本。

??? info "支持并行的算法"

    | 算法接口 | 含义 |
    | --- | --- |
    | `std::all_of` | 检查范围内所有元素是否满足条件 |
    | `std::any_of` | 检查范围内是否有元素满足条件 |
    | `std::none_of` | 检查范围内是否没有元素满足条件 |
    | `std::for_each` | 对范围内所有元素执行操作 |
    | `std::for_each_n` | 对范围内的前 n 个元素执行操作 |
    | `std::find` | 在范围内查找元素 |
    | `std::find_if` | 在范围内查找满足条件的元素 |
    | `std::find_end` | 在范围内查找子序列 |
    | `std::find_first_of` | 在范围内查找第一个匹配元素 |
    | `std::adjacent_find` | 在范围内查找相邻重复元素 |
    | `std::count` | 计算范围内满足条件的元素个数 |
    | `std::count_if` | 计算范围内满足条件的元素个数 |
    | `std::mismatch` | 在范围内查找第一个不匹配元素 |
    | `std::equal` | 比较两个范围是否相等 |
    | `std::search` | 在范围内查找子序列 |
    | `std::search_n` | 在范围内查找连续 n 个相同元素 |
    | `std::copy` | 复制范围内的元素到另一个范围 |
    | `std::copy_n` | 复制范围内的前 n 个元素到另一个范围 |
    | `std::copy_if` | 复制范围内满足条件的元素到另一个范围 |
    | `std::move` | 移动范围内的元素到另一个范围 |
    | `std::swap_ranges` | 交换两个范围的元素 |
    | `std::transform` | 对范围内的元素执行操作并存储到另一个范围 |
    | `std::replace` | 替换范围内的元素 |
    | `std::replace_if` | 替换范围内满足条件的元素 |
    | `std::replace_copy` | 替换范围内的元素并存储到另一个范围 |
    | `std::replace_copy_if` | 替换范围内满足条件的元素并存储到另一个范围 |
    | `std::fill` | 填充范围内的元素 |
    | `std::fill_n` | 填充范围内的前 n 个元素 |
    | `std::generate` | 生成范围内的元素 |
    | `std::generate_n` | 生成范围内的前 n 个元素 |
    | `std::remove` | 移除范围内的元素 |
    | `std::remove_if` | 移除范围内满足条件的元素 |
    | `std::remove_copy` | 移除范围内的元素并存储到另一个范围 |
    | `std::remove_copy_if` | 移除范围内满足条件的元素并存储到另一个范围 |
    | `std::unique` | 移除范围内的连续重复元素 |
    | `std::unique_copy` | 移除范围内的连续重复元素并存储到另一个范围 |
    | `std::reverse` | 反转范围内的元素 |
    | `std::reverse_copy` | 反转范围内的元素并存储到另一个范围 |
    | `std::rotate` | 旋转范围内的元素 |
    | `std::rotate_copy` | 旋转范围内的元素并存储到另一个范围 |
    | `std::is_partitioned` | 检查范围是否被分割 |
    | `std::partition` | 将范围内的元素分割 |
    | `std::stable_partition` | 将范围内的元素稳定分割 |
    | `std::partition_copy` | 将范围内的元素分割并存储到两个范围 |
    | `std::sort` | 对范围内的元素排序 |
    | `std::stable_sort` | 对范围内的元素稳定排序 |
    | `std::partial_sort` | 对范围内的元素部分排序 |
    | `std::partial_sort_copy` | 对范围内的元素部分排序并存储到另一个范围 |
    | `std::is_sorted` | 检查范围是否有序 |
    | `std::is_sorted_until` | 检查范围的无序部分 |
    | `std::nth_element` | 对范围内的元素进行部分排序 |
    | `std::merge` | 合并两个有序范围 |
    | `std::inplace_merge` | 合并范围内的元素 |
    | `std::includes` | 检查一个范围是否包含另一个范围 |
    | `std::set_union` | 计算两个有序范围的并集 |
    | `std::set_intersection` | 计算两个有序范围的交集 |
    | `std::set_difference` | 计算两个有序范围的差集 |
    | `std::set_symmetric_difference` | 计算两个有序范围的对称差集 |
    | `std::is_heap` | 检查范围是否为堆 |
    | `std::is_heap_until` | 检查范围的堆部分 |
    | `std::min_element` | 查找范围内的最小元素 |
    | `std::max_element` | 查找范围内的最大元素 |
    | `std::minmax_element` | 查找范围内的最小和最大元素 |
    | `std::lexicographical_compare` | 比较两个范围的字典序 |
    | `std::reduce` | 对范围内的元素进行归约 |
    | `std::transform_reduce` | 对范围内的元素进行变换和归约 |
    | `std::exclusive_scan` | 对范围内的元素进行前缀和 |
    | `std::inclusive_scan` | 对范围内的元素进行前缀和 |
    | `std::transform_exclusive_scan` | 对范围内的元素进行变换和前缀和 |
    | `std::transform_inclusive_scan` | 对范围内的元素进行变换和前缀和 |
    | `std::adjacent_difference` | 计算范围内的相邻差值 |

### `std::execution` 执行策略

标准库定义三个类及对应的策略对象：

| 策略 | 策略对象 | 含义 |
| --- | --- | --- |
| `std::execution::sequenced_policy` | `std::execution::seq` | 串行执行 |
| `std::execution::parallel_policy` | `std::execution::par` | 并行执行 |
| `std::execution::parallel_unsequenced_policy` | `std::execution::par_unseq` | 并行执行，不允许串行化 |

- **策略对象**需要进行特别的初始化，不得直接创建，**只能从上面 3 个复制构造得出**。

执行策略指定了算法函数中间步骤的

- **执行主体（execution agent）**

    - CPU 线程
    - 向量流（vector stream）：SIMD 指令集
    - GPU 线程
    - 其他运算单元：由编译器提供

- **内存次序**

#### `sequenced_policy`

几乎没有施加内存次序限制，可以自由同步。

!!! example

    下面的填充次序并不确定。

    ```cpp
    std::for_each(std::execution::seq, my_data.begin(), my_data.end(), [&](int& x) { x = ++ count; });
    ```

    如果不指定策略，填充次序是确定的。

#### `parallel_policy`

函数的内部操作可以在发起调用的线程上执行，也可以由程序库另外创建线程执行。如果相关数据结构涉及并行操作，应当使用同步，不得引发数据竞争。

!!! example

    ```cpp
    std::for_each(std::execution::par, my_data.begin(), my_data.end(), [](auto &x){ ++x; });
    ```

    如果执行 `[&](int& x) { x = ++ count; }`，则会引发数据竞争。

!!! example

    与 OpenMP 比较：

    ```cpp
    #pragma omp parallel for
    for(unsigned i=0;i<my_data.size();++i)
    {
        do_stuff(my_data[i]);
    }
    ```

    ```cpp
    std::for_each(std::execution::par, my_data.begin(), my_data.end(), do_stuff);
    ```

#### `parallel_unsequenced_policy`

对相关数据结构施加最严格的内存次序限制，以便最大程度发挥并行化潜力。在相关数据结构上的操作不得同步。





