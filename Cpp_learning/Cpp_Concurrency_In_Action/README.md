# C++ Concurrency In Action

- [Hello, world of concurrency in C++!](#1)
- [Managing threads](#2)
- [Sharing data between threads](#3)
- [](#4)
- [](#5)
- [](#6)
- [](#7)
- [](#8)
- [](#9)


&nbsp;   
### 其他参考资料

> 本书的中文翻译坑的一笔（看上去绝对不是机翻，神坑）；   
> 参考标准委员会的文章和邮件；   
> 总之要多看一些文章和帖子；   

- [线程支持库 - zh/cppreference](https://zh.cppreference.com/w/cpp/thread)
- [C++11 Multithread Tutorial Series](https://thispointer.com/c11-multithreading-tutorial-series/)
- [A Detailed Cplusplus Concurrency Tutorial 《C++ 并发编程指南》](https://github.com/forhappy/Cplusplus-Concurrency-In-Practice)
- [C++11 并发指南系列](https://www.cnblogs.com/haippy/p/3284540.html)
- [秒杀多线程面试题系列](https://blog.csdn.net/MoreWindows/column/info/killthreadseries)（不确定好不好）


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
- [**`std::call_once()`**](https://zh.cppreference.com/w/cpp/thread/call_once)：准确执行一次可调用 (Callable) 对象 f，即使同时从多个线程调用（若抛异常，则传播；并且不消耗 `std::once_flag`）（用途：打开 socket connection；打开文件；等准确初始化一次资源）
- **读写锁**：保护*很少更新*的数据结构
  - [`class std::shared_mutex`](https://zh.cppreference.com/w/cpp/thread/shared_mutex)：通常用于多个读线程能同时访问同一资源而不导致数据竞争，但只有一个写线程能访问的情形
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

&nbsp;   
Ref:

- [Is std::lock() ill-defined, unimplementable, or useless?](https://stackoverflow.com/questions/18520983/is-stdlock-ill-defined-unimplementable-or-useless)
- [Massive CPU load using std::lock (c++11)](https://stackoverflow.com/questions/13667810/massive-cpu-load-using-stdlock-c11)
- [如何理解互斥锁、条件锁、读写锁以及自旋锁？](https://www.zhihu.com/question/66733477)


&nbsp;   
<a id="4"></a>
## Ch04




&nbsp;   
<a id="5"></a>
## Ch05





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


