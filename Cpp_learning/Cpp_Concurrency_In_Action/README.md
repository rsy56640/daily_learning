# C++ Concurrency In Action

- [Hello, world of concurrency in C++!](#1)
- [Managing threads](#2)
- [](#3)
- [](#4)
- [](#5)
- [](#6)
- [](#7)
- [](#8)
- [](#9)


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
  - `detach()` 可以立即调用
  - `join()` 需要选择适当的位置，注意**如果在 `join()` 调用之前线程抛出异常**





&nbsp;   
<a id="3"></a>
## Ch03





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





