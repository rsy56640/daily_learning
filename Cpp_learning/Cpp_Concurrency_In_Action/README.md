# C++ Concurrency In Action

- [Hello, world of concurrency in C++!](#1)
- [Managing threads](#2)
- [Sharing data between threads](#3)
- [Synchronizing concurrent operations](#4)
- [The C++ memory model and operations on atomic types](#5)
- [](#6)
- [](#7)
- [](#8)
- [](#9)


&nbsp;   
### 其他参考资料

> 本书的中文翻译坑的一笔（看上去绝对不是机翻，神坑）；   
> 参考标准委员会的文章和邮件；   
> 总之要多看一些文章和帖子；   
> 笔记中掺杂了一些我个人的理解；   

- [线程支持库 - zh/cppreference](https://zh.cppreference.com/w/cpp/thread)
- [C++11 Multithread Tutorial Series](https://thispointer.com/c11-multithreading-tutorial-series/)
- [A Detailed Cplusplus Concurrency Tutorial 《C++ 并发编程指南》](https://github.com/forhappy/Cplusplus-Concurrency-In-Practice)
- [C++11 并发指南系列](https://www.cnblogs.com/haippy/p/3284540.html)
- [Operating Systems: Three Easy Pieces](http://pages.cs.wisc.edu/~remzi/OSTEP/#book-chapters)


&nbsp;   
<a id="1"></a>
## Ch01 Hello, world of concurrency in C++!

C++ 标准并未对 **进程间通信** 提供任何支持，本书只关注多线程的并发。

为什么需要并发？

- 分离不同的功能（在同一时刻会有多种功能）（**对线程的划分是基于概念上的设计，而不是为了增加吞吐量的尝试**）
- 性能
  - 单任务并行：将一个任务切割成多块
      - 算法并行：算法分块执行
      - 数据并行：同样的操作，处理不同数据
  - 使用可并行的方式，处理更多


&nbsp;   
<a id="2"></a>
## Ch02 Managing threads

使用 `std::thread` 要注意

- most vexing parsing
- 在 `thread` 析构之前调用 `.join()` 或 `.detach()`
  - [`detach()`](https://zh.cppreference.com/w/cpp/thread/thread/detach) 可以立即调用
  - [`join()`](https://zh.cppreference.com/w/cpp/thread/thread/join) 需要选择适当的位置，注意**如果在 `join()` 调用之前线程抛出异常**
- 可移动，不可复制
- `thread& operator=( thread&& other ) noexcept`若 `*this` 仍拥有关联的运行中进程（即 `joinable() == true` ），则调用 `std::terminate() `。否则，赋值 `other` 的状态给 `*this` 并设置 `other` 为默认构造的状态。
此调用后，`this->get_id()` 等于 `other.get_id()` 在调用前的值，而 `other` 不再表示执行的线程
- [`static unsigned int std::thread::hardware_concurrency() noexcept;`](https://zh.cppreference.com/w/cpp/thread/thread/hardware_concurrency)：返回实现所支持的并发线程数。应该只把值当做提示
- class [`std::thread::id`](https://zh.cppreference.com/w/cpp/thread/thread/id)：std::thread 的标识符；**一旦线程结束，则 std::thread::id 的值可为另一线程复用**；`std::thread::id` is **comparable** and **hashable**
  - 若两个 id 相等 (`.operator==`)，则
      - 若都持有线程，则 same
      - 否则不确定
- `namespace std::thread::this_thread`
  - [`yield()`](https://zh.cppreference.com/w/cpp/thread/yield)
  - [`get_id()`](https://zh.cppreference.com/w/cpp/thread/get_id)：通常用来判断某些线程是否要做一些特殊操作
  - [`sleep_for()`](https://zh.cppreference.com/w/cpp/thread/sleep_for)
  - [`sleep_until()`](https://zh.cppreference.com/w/cpp/thread/sleep_until)
- 了解一下 [`std::chrono`](https://zh.cppreference.com/w/cpp/chrono)


&nbsp;   
其他：

- 一份关于 [2_4_parallel_accumulate.cpp](https://github.com/rsy56640/daily_learning/blob/master/Cpp_learning/Cpp_Concurrency_In_Action/code/2_4_parallel_accumulate.cpp) 的代码的一些说明：要求结合律，要求默认构造函数，浮点类型求和可能被系统截断。另外，注意到在lambda表达式中 `std::ref()` 和 `std::mem_fn()` 的使用（其实写成lambda就没有必要了）。


&nbsp;   
<a id="3"></a>
## Ch03 Sharing data between threads

- [**race conditions**](https://zh.cppreference.com/w/cpp/language/memory_model#.E7.BA.BF.E7.A8.8B.E4.B8.8E.E6.95.B0.E6.8D.AE.E7.AB.9E.E4.BA.89)
- 设计 **数据结构** 和 **invariant**（即约定好数据结构的性质）
- 事务性支持？？？
- **互斥量**（[**`std::mutex`**](https://zh.cppreference.com/w/cpp/thread/mutex)，使用 [`std::lock_guard`](https://zh.cppreference.com/w/cpp/thread/lock_guard) with RAII 机制）（注：互斥的 **所有权** 即上锁；互斥的 **关联** 则是记录这个锁的索引）
  - 通常 mutex 和 critical data 作为 private member
  - 谨慎设计接口，尤其是函数将 critical data 的 handle 传递到外部
  - 讲了一个 `stakc<vector<int>>` sample，但其实有 guaranteed copy elision。如果连内存都分配失败，那还不如重启
  - 接口：
      - [`std::mutex::lock()`](https://zh.cppreference.com/w/cpp/thread/mutex/lock)（若持有 mutex 的线程调用，则 UB）
      - [`std::mutex::try_lock()`](https://zh.cppreference.com/w/cpp/thread/mutex/try_lock)：不阻塞，立即返回成功或失败（若持有 mutex 的线程调用，则 UB）
      - [`std::mutex::unlock()`](https://zh.cppreference.com/w/cpp/thread/mutex/unlock)（若当前线程不持有 mutex，则 UB）
      - [`std::mutex::~mutex()`](https://zh.cppreference.com/w/cpp/thread/mutex/~mutex)（若 mutex 为任何线程占有，或若任何线程在保有 mutex 的所有权时终止，则 UB）
  - [`std::timed_mutex`](https://zh.cppreference.com/w/cpp/thread/timed_mutex)：定时 mutex，支持 [`std::timed_mutex::try_lock_for()`](https://zh.cppreference.com/w/cpp/thread/timed_mutex/try_lock_for)；代码样例：[std_timed_mutex.cpp](https://github.com/rsy56640/daily_learning/blob/master/Cpp_learning/Cpp_Concurrency_In_Action/code/std_timed_mutex.cpp)
  - [`std::recursive_mutex`](https://zh.cppreference.com/w/cpp/thread/recursive_mutex)：支持同一线程内多次 lock
  - [`std::recursive_timed_mutex`](https://zh.cppreference.com/w/cpp/thread/recursive_timed_mutex)
  - 注意 traits：[BasicLockable](https://zh.cppreference.com/w/cpp/named_req/BasicLockable), [Lockable](https://zh.cppreference.com/w/cpp/named_req/Lockable), [TimedLockable](https://zh.cppreference.com/w/cpp/named_req/TimedLockable)
- [`class std::lock_guard`](https://zh.cppreference.com/w/cpp/thread/lock_guard)：RAII 互斥锁定（允许构造前已经锁定，即 `std::adopt_lock`）（不支持 move）
- [`class std::unique_lock`](https://zh.cppreference.com/w/cpp/thread/unique_lock)：RAII 互斥锁定，但提供了更好的上锁和解锁控制（注意[*构造函数*](https://zh.cppreference.com/w/cpp/thread/unique_lock/unique_lock) 和 [*析构函数*](https://zh.cppreference.com/w/cpp/thread/unique_lock/~unique_lock)，并且可以在 scope 内不断调用 `unlock()` 和 `lock()`）（`std::unique_lock` 比 `std::lock_guard` 灵活，但是占用更大的空间，并且性能稍微差一点）（支持 move ctor 和 move assign）
- **3个tag**：
  - [`std::defer_lock`](https://zh.cppreference.com/w/cpp/thread/lock_tag)：不获得互斥的所有权（只是关联，之后调用 `lock()`）
  - [`std::try_to_lock`](https://zh.cppreference.com/w/cpp/thread/lock_tag)：尝试获得互斥的所有权而不阻塞（之后调用 `bool owns_lock() const noexcept;` 检查）
  - [`std::adopt_lock`](https://zh.cppreference.com/w/cpp/thread/lock_tag)：假设调用方线程已拥有互斥的所有权（*没明白有什么用，相当于只提供了析构，那为什么上锁的时候不构造 `lock_guard` 或 `unique_lock`*；好像是因为有时候上锁只操作 mutex，所以之后 **再补一个自动析构**）
- **DeadLock**
  - [`std::lock()`](https://zh.cppreference.com/w/cpp/thread/lock), [`class std::scoped_lock;`](https://zh.cppreference.com/w/cpp/thread/scoped_lock)：死锁避免；要么全锁，要么全不锁（抛异常）
  - `std::lock()` 要保证自动析构：
      - 之前 `std::unique_lock(mutex, std::defer_lock);`
      - 之后 `std::unique_lock(mutex, std::adopt_lock);` 或者 `std::lock_guard(mutex std::adopt_lock);`（推荐）
      - `std::unique_lock` 析构时会检查 mutex 的所有权来决定是否 unlock
      - `std::lock_guard` 析构时会直接 unlock
- 死锁避免：
  - 避免嵌套锁：最好一个线程只持有一个锁
  - 当需要多个锁时：使用 `std::lock()` 或 `class scoped_lock;`提供了免死锁算法；或者自己设计锁分配算法
  - （尽量）避免在持有锁时调用用户代码。（有时必须要给用户上锁）
  - （priority）使用固定顺序获得多个锁
  - 使用锁的层次结构：运行时记录并检查
- **锁的粒度**：一般情况下，执行必要的操作时，尽可能将持有锁的时间缩减到最小。
  - 当采用细粒度的锁时，注意语义的变化。（对一些值的操作有可能发生在不同时刻）
- [静态局部变量的初始化](https://zh.cppreference.com/w/cpp/language/storage_duration#.E9.9D.99.E6.80.81.E5.B1.80.E9.83.A8.E5.8F.98.E9.87.8F)：标准保证了静态局部变量被准确初始化一次
- Double-Checked Locking pattern：race condition（见下）
- [`class std::once_flag`](https://zh.cppreference.com/w/cpp/thread/once_flag)
- [**`std::call_once()`**](https://zh.cppreference.com/w/cpp/thread/call_once)：**准确执行一次**可调用 (Callable) 对象 f，即使同时从多个线程调用（若抛异常，则传播；并且不消耗 `std::once_flag`）（用途：打开 socket connection；打开文件；等准确初始化一次资源）
- **读写锁**：保护*很少更新*的数据结构
  - [**`class std::shared_mutex`**](https://zh.cppreference.com/w/cpp/thread/shared_mutex)：通常用于多个读线程能同时访问同一资源而不导致数据竞争，但只有一个写线程能访问的情形
  - [`class std::shared_lock`](https://zh.cppreference.com/w/cpp/thread/shared_lock)：共享模式锁定关联的互斥
  - 读上锁：`std::shared_lock slk{smtx};` *可共享互斥*
  - 写上锁：`std::lock_guard lg{smtx};` *排他互斥*
- 嵌套锁：有时成员函数会调用其他成员函数，导致多次上锁
  - [`class std::recursive_mutex`](https://zh.cppreference.com/w/cpp/thread/recursive_mutex)：不推荐使用。（注意维护 *数据结构* 和 *invariant* 的逻辑；提取出一些操作作为私有函数，并不上锁）


&nbsp;   
其他：

> 原子性：**缓存锁**，**总线锁**   
> [聊聊并发（五）原子操作的实现原理](http://ifeve.com/atomic-operation/)

获取 `std::thread::id` 标识的 int   
```c++
inline int get_id(const std::thread::id tid)
{
	std::ostringstream os;
	os << tid;
	return std::stoi(os.str());
}
```

&nbsp;   
声名狼藉的 Double-Checked Locking pattern：   
1 和 3 引发 race condition
```c++
struct some_resource { void do_something() {} };
std::shared_ptr<some_resource> resource_ptr;
std::mutex resource_mutex;
void undefined_behaviour_with_double_checked_locking()
{
	if (!resource_ptr)                               // 1
	{
		std::lock_guard<std::mutex> lk(resource_mutex);
		if (!resource_ptr)                           // 2
		{
			resource_ptr.reset(new some_resource);   // 3
		}
	}
	resource_ptr->do_something();                    // 4
}
```

读写锁 [std_shared_mutex.cpp](https://github.com/rsy56640/daily_learning/blob/master/Cpp_learning/Cpp_Concurrency_In_Action/code/std_shared_mutex.cpp)：使用 `std::shared_mutex`

- 读上锁：`std::shared_lock slk{smtx};` *可共享互斥*
- 写上锁：`std::lock_guard lg{smtx};` *排他互斥*


&nbsp;   
Ref:

- [Is std::lock() ill-defined, unimplementable, or useless?](https://stackoverflow.com/questions/18520983/is-stdlock-ill-defined-unimplementable-or-useless)
- [Massive CPU load using std::lock (c++11)](https://stackoverflow.com/questions/13667810/massive-cpu-load-using-stdlock-c11)
- [如何理解互斥锁、条件锁、读写锁以及自旋锁？](https://www.zhihu.com/question/66733477)


&nbsp;   
<a id="4"></a>
## Ch04 Synchronizing concurrent operations

- 等待时间或条件：
  - (1) shared falg (使用mutex)
      - 等待线程 keeps checking；执行线程 do task，然后写 flag
      - 2个缺点：
          - 1) check flag 上锁，消耗时间
          - 2) 等待线程可能消耗其他资源
      - 改进：等待线程 `std::this_thread::sleep_for();`（但是很难确定休眠时间）
  - (2) 使用条件变量去触发事件并唤醒
     - 虚假唤醒
     - unlock 与 signal 顺序
     - 需要几个条件变量
     - 唤醒特定的线程
- 使用 `std::future` 来等待**一次性事件（one-off event）**
  - [std::future](https://zh.cppreference.com/w/cpp/thread/future) ：提供访问异步操作结果的机制。可移动，不可复制
      - `T/T&/void get();`
      - `void wait();`
      - `bool valid() const noexcept;`
      - `std::shared_future<T> share() noexcept;`
      - `enum class` [`std::future_status`](https://zh.cppreference.com/w/cpp/thread/future_status)
          - `ready`：共享状态就绪
          - `timeout`：共享状态在经过指定的时限时长前仍未就绪
          - `deferred`：共享状态含有延迟的函数，故将仅在显式请求时计算结果
  - [std::promise](https://zh.cppreference.com/w/cpp/thread/promise)：每个 promise 与共享状态关联，共享状态含有一些状态信息和可能仍未求值的结果，它求值为值（可能为 void ）或求值为异常。 promise 可以对共享状态做三件事：（如有疑惑，参考 [promise - 搜索 shared state](http://eel.is/c++draft/futures.promise) 和 [共享状态](http://eel.is/c++draft/futures.state)）
      - **使就绪**：promise 存储结果或异常于共享状态。标记共享状态为就绪，并解除阻塞任何等待于与该共享状态关联的 future 上的线程
      - **释放**：promise 放弃其对共享状态的引用。若这是最后一个这种引用，则销毁共享状态。除非这是 `std::async` 所创建的未就绪的共享状态，否则此操作不阻塞
      - **抛弃**：promise 存储以 `std::future_errc::broken_promise` 为 error_code 的 `std::future_error` 类型异常，令共享状态为就绪，然后释放它
      - `~promise()`：若共享状态就绪，则释放它；若共享状态未就绪，则存储以 `std::future_errc::broken_promise` 为 error_condition 的 `std::future_error` 类型异常对象，令共享状态就绪再释放它
  - [std::packaged_task](https://zh.cppreference.com/w/cpp/thread/packaged_task)：包装任何可调用 (Callable) 目标，使得能异步调用它（通常用于线程间传递任务）
      - 通过 `get_future()` 得到 `std::future<R>`
      - `void operator()(Args... args);`：以 args 为参数调用存储的任务。任务返回值或任何抛出的异常被存储于共享状态。令共享状态就绪，并解除阻塞任何等待此操作的线程；（`void make_ready_at_thread_exit(Args... args);` 仅在当前线程退出，并销毁所有线程局域存储期对象后，才令共享状态就绪。）
      - 由于 `packaged_task` 是可调用的，可以新建 `std::thread(std::move(task), args...);`
  - [std::async](https://zh.cppreference.com/w/cpp/thread/async)（大概会根据硬件情况来调整以获得最佳性能）
      - `enum class` [`std::launch`](https://zh.cppreference.com/w/cpp/thread/launch)
          - `std::launch::async`：运行新线程，以异步执行任务
          - `std::launch::deferred`：调用方线程上首次请求其结果时执行任务（惰性求值）
  - [std::shared_future](https://zh.cppreference.com/w/cpp/thread/shared_future)：允许多个线程等候同一共享状态，`std::shared_future` 可复制，而且多个 `std::shared_future` 对象能指代同一共享状态。（若每个线程通过其自身的 shared_future 对象副本访问，则从多个线程访问同一共享状态是安全的。）
- [namespace std::chrono](https://zh.cppreference.com/w/cpp/header/chrono)
  - `std::chrono::system_clock`
  - `std::chrono::duration`
  - `std::chrono::time_point`
  - `std::literals::chrono_literals`
- Communicating Sequential Processer
- **Message Passing Interface**：没有共享数据，每个线程都是一个**状态机**，通过**消息队列**通信


&nbsp;   
>关于 Condition Variable，参考 [Condition Variables - Operating Systems: Three Easy Pieces](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-cv.pdf)   
>**hold the lock when calling signal(mostly) or wait(always)**   
>[用条件变量实现事件等待器的正确与错误做法 - 陈硕的Blog](http://www.cppblog.com/Solstice/archive/2013/09/09/203094.html)   

&nbsp;   
关于虚假唤醒（[Spurious wakeup](https://en.wikipedia.org/wiki/Spurious_wakeup)）：等待线程有可能偶然返回（**因为接受signal，处理时有可能忽略了notification，所以从wait返回，注意退出wait时已经重新锁定lock**）

&nbsp;   
线程安全队列：[4_5_thread_safe_queue.cpp](https://github.com/rsy56640/daily_learning/blob/master/Cpp_learning/Cpp_Concurrency_In_Action/code/4_5_thread_safe_queue.cpp)（事实上，如何设计要根据具体场景trade-off）

&nbsp;   
当异常类型已知时，建议使用 `prom.set_exception(std::make_exception_ptr(...));` 而不是 `prom.set_exception(std::current_exception());`（这样编译器可以优化；不过异常对象的构造参数也许无法得知，这样就不一样了）

```c++
struct Ex { int i = 9; };
int foo_ex(int i) {
	if (i < 5) throw Ex{};
	return 233;
}
void test()
{
	std::promise<double> prom;
	try {
		prom.set_value(foo_ex(2));
	}
	catch (...) {
		// prom.set_exception(std::current_exception());
		prom.set_exception(std::make_exception_ptr(Ex{})); // better on performance but not the same exception
	}
}
```

&nbsp;   
[异步测试程序](https://github.com/rsy56640/daily_learning/blob/master/Cpp_learning/Cpp_Concurrency_In_Action/code/fucking_test_threads_pool_with_fucking_stdchrono.cpp)：比较了 单线程，线程池异步调度，`std::async`。


&nbsp;   

- [pthread_cond_signal RATIONALE](http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_cond_signal.html#tag_16_418_08)
- [Why does pthread_cond_wait have spurious wakeups?](https://stackoverflow.com/questions/8594591/why-does-pthread-cond-wait-have-spurious-wakeups)
- [Spurious wakeups explanation sounds like a bug that just isn't worth fixing, is that right?](https://softwareengineering.stackexchange.com/questions/186842/spurious-wakeups-explanation-sounds-like-a-bug-that-just-isnt-worth-fixing-is)
- [Do spurious wakeups in Java actually happen?](https://stackoverflow.com/questions/1050592/do-spurious-wakeups-in-java-actually-happen/1051816#1051816)
- [basic question about concurrency - Google Forum](https://groups.google.com/forum/?hl=ky#!msg/comp.programming.threads/wEUgPq541v8/ZByyyS8acqMJ) 参考 Dave Butenhof 的回复（Programming with POSIX threads作者）
- [Real cause of spurius wakeups - Google Forum](https://groups.google.com/forum/#!msg/comp.programming.threads/h6vgL_6RAE0/Ur8sq72OoKwJ)
- [Spurious wakeups](http://blog.vladimirprus.com/2005/07/spurious-wakeups.html)
- [Calling pthread_cond_signal without locking mutex - Stack Overflow](https://stackoverflow.com/questions/4544234/calling-pthread-cond-signal-without-locking-mutex) 第一个答案评论区 R 是某版本pthread作者
- [The actor model in 10 minutes](https://www.brianstorti.com/the-actor-model/)
- [Hewitt, Meijer and Szyperski: The Actor Model (everything you wanted to know...) - YouTube - （这个好厉害）](https://www.youtube.com/watch?v=7erJ1DV_Tlo)


&nbsp;   
<a id="5"></a>
## Ch05 The C++ memory model and operations on atomic types







&nbsp;   
<a id="6"></a>
## Ch06





&nbsp;   
<a id="7"></a>
## Ch07





&nbsp;   
<a id="8"></a>
## Ch08




&nbsp;   
<a id="9"></a>
## Ch09


