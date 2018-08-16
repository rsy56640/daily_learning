# Effective STL learning note
--------  
2018-7-3 开始阅读，因为之前自己写过STL的部分组件，所以打算快速的过一遍，就当是某种意义上的补全了。  
我大概浏览了一下，本书标准大概是C++98，也就是说有很多落后的地方，比如： `auto_ptr`, `move semantics`, `unordered_map` 等.   

--------  
看了有10个条款，目前的感觉是：这本书略有点鸡肋，如果是入门水平看这本书，虽有一定收获，但恐怕对很对发展进程与设计都是云里雾里。但如果有了一定经验，比如写过STL的一些组件，对标准委员会的提案有一定理解，并且对整个标准库和设计理念的历史发展有相当认识，那么这本书的感觉就有点像是“段子”一样了。

--------  
看到 [Item 12](#12)，更加印证了之前的看法：如果某些组件没有深入研究过，那么在看的时候即使表面看懂，恐怕也无法领略到深刻的设计思路以及历史上的变迁。

--------  

# 1. Container
----
- choose the appropriate container facing specific constraints
- different low-level detail
- copy relevant trade-off
- move semantics
- auto_ptr / shared_ptr in container
- problem that deletion cause
- using containers in multiple threads
&emsp;    
&emsp;    


<a id="1"></a>
## Item 1 慎重选择容器类型

- Strategy on memory allocation：  
  - contiguous memory allocation   
    &emsp; Advantage: random-access  
    &emsp; Disadvantage: insert/delete/copy overhead  
  - node-based memory allocation  
    &emsp; Advantage: insert/delete   
- element layout in container
- kind of iterator to use
  - forward
  - bidirectional
  - random access
- whether move/copy elements in container matters that when insert/delete
- search
- reference counting
  - `std::string` -> `std::vector<char>`
- transactional semantics
- invalidate iterator, pointer, reference

&emsp;    
&emsp;    


<a id="2"></a>
## Item 2 不要试图编写独立于容器的代码

- use static_assert(since c++11), concept(since c++20)
- try best to achieve low coupling
- the constraints originate from regulations being different for iterators invalidation among different containers.  
  eg. 
  - `deque::insert` invalidates all of iterators, thus you'd assume all the iterators have been invalidated.
  - 臭名昭著的 `std::vector<bool>`   
- containers are different, and they are not designed to use identically.  

Solution: **encapsulation**  

- put the container into a low-level Tool class.(container will not expose to users, and just forward the methods)
- use `typedef` or `using`

&emsp;    
&emsp;    


<a id="3"></a>
## Item 3 确保容器中的对象拷贝正确而高效

copy -> performance bottleneck  
&emsp;&emsp;&nbsp;&nbsp;&nbsp; | object slicing
&nbsp;  
Solution: use pointer  
&emsp;&emsp;&emsp;&emsp; move semantics  

&emsp;    
&emsp;    


<a id="4"></a>
## Item 4 调用 `empty()` 而不是检查 `size()`是否为0

`empty()` costs constant time for all standard containers.  
有趣的是，`std::list::size()` 在C++11中复杂度为 O(1)   
`std::list` 有几个有趣的操作:   

- `splice() O(1) or O(N)`
- `sort()   O(NlogN)`
- `size()   O(1)`  
这个值得一写，可惜我现在还没写过｡:.ﾟヽ(｡◕‿◕｡)ﾉﾟ.:｡+ﾟ  

&emsp;    
&emsp;    


<a id="5"></a>
## Item 5 区间成员函数优先于与之对应的单元素成员函数

Standard sequential containers all have `::assign()` operation.
range function specifies the range according to 2 iterators.

- avoid to write code with explicit loop
- loop will impose to efficiency penalty

使用区间成员函数使逻辑意图更加清晰，增加系统的可维护性。   
减少了多次调用所带来的冗余操作。
重复调用空间置配器。  
In all, using range member function allows container to prepare the resources once, while using single-element operation will incur unnecessary costs.  
就是说，提前知道操作就可以进行规划，表现出更高的效率。  
对顺序容器的优化是明显的，对关联容器的优化值得探究。
容易，清晰，效率。

&emsp;    
&emsp;    


<a id="6"></a>
## Item 6 当心C++编译器最烦人的分析机制 - Most Vexing Parse

参考 [Item7: Distinguish between () and {} when creating objects - <<Effective Modern C++>>](https://github.com/gavincyi/Effective-Modern-Cpp#item-7--distinguish-between--and--when-creating-objects)  

    int f(double);
    int f(double (d));
    //围绕参数名的括号被忽略
    
    int g(double (*pf)());
    int g(double pf());
    int g(double ());
    //独立的括号表明参数列表的存在，说明存在一个函数指针

Most Vexing Parse: 尽可能地解释为函数声明。

Solution:   

- 使用简单的构造，而非匿名对象。(但面临拷贝，`std::forward()`)  
- using {} rather than (), 但有时 {} 不同于 (), 比如 [`std::string`](https://en.cppreference.com/w/cpp/string/basic_string/basic_string).  

&emsp;    
&emsp;    


<a id="7"></a>
## Item 7 如果容器中包含了 `new`，在析构前 `delete`

- RAII
- std::shared_ptr
- STL空间置配器

&emsp;    
&emsp;    


<a id="8"></a>
## Item 8 切勿创建包含 `auto_ptr` 的容器对象

`std::auto_ptr` has been deprecated in C++11 and removed in C++17.

&emsp;    
&emsp;    


<a id="9"></a>
## Item 9 慎重选择删除元素的方法

- For Standard Sequential Containers  
[Erase–Remove idiom](https://en.wikipedia.org/wiki/Erase%E2%80%93remove_idiom)   
`c.erase(std::remove(c.begin(), c.end(), value), c.end());`  
`c.erase(std::remove_if(c.begin(), c.end(), bool (T)), c.end());`  

- For Associate Containers  
`c.erase(value);`  

        // Associate Containers
        for (auto it = c.begin(); it != c.end(); /* do nothing */)
        {
            if (...)
                c.erase(it++);
            else ++it;
        }
        
        // Sequential Containers
        for (auto it = c.begin(); it != c.end(); /* do nothing */)
        {
            if(...)
                it = c.erase(it);
            else ++it;
        }

- For `std::list`
use [std::list::remove](https://zh.cppreference.com/w/cpp/container/list/remove) or [std::list::remove_if](https://zh.cppreference.com/w/cpp/container/list/remove).  


In all

- delete specific element
   -  Sequential Container: erase-remove idiom
   - `std::list`: `std::list::remove`
   - Associate Container: erase()
- do some operations during deletion
   - Sequential Container: `it = c.erase(it);`
   - Associate Container: `c.erase(it++);`

&emsp;    
&emsp;    


<a id="10"></a>
## Item 10 了解分配子（allocator）的约定和限制

（看了有10个条款，目前的感觉是：这本书略有点鸡肋，如果是入门水平看这本书，虽有一定收获，但恐怕对很对发展进程与设计都是云里雾里。但如果有了一定经验，比如写过STL的一些组件，对标准委员会的提案有一定理解，并且对整个标准库和设计理念的历史发展有相当认识，那么这本书的感觉就有点像是“段子”一样了。）  
allocator之前在《STL源码剖析》中看过，但没有写过，而且时间长了，也记不住个大概。这一章的理解恐怕还是不够深刻。  

[C++中为什么要有allocator类？ - 知乎](https://www.zhihu.com/question/274802525)   
[《STL源码剖析》提炼总结：空间配置器(allocator) - 知乎](https://zhuanlan.zhihu.com/p/34725232)    
[C++ 标准库中的allocator是多余的 - CSDN](https://blog.csdn.net/solstice/article/details/4401382)    





&emsp;    
&emsp;    


<a id="11"></a>
## Item 11 




&emsp;    
&emsp;    


<a id="12"></a>
## Item 12 切勿对 STL 容器的线程安全性有不切实际的依赖

唯一的保证：   

- 同时对不同容器进行操作  
- 同时对同一容器进行只读操作  

>
The SGI implementation of STL is thread-safe only in the sense that simultaneous accesses to distinct containers are safe, and simultaneous read accesses to to shared containers are safe. If multiple threads access a single container, and at least one thread may potentially write, then the user is responsible for ensuring mutual exclusion between the threads during the container accesses.  

本书中给出的建议：   
手工同步控制线程安全，使用资源管理和异常处理技术。  

[STL Thread safety](https://en.cppreference.com/w/cpp/container#Thread_safety)   
STL 的线程安全性我没有研究过，所以无法进行深刻地剖析。  

[**线程安全**](https://zh.cppreference.com/w/cpp/container#.E7.BA.BF.E7.A8.8B.E5.AE.89.E5.85.A8)    

- 能同时在不同容器上由不同线程调用所有容器函数。更广泛而言， C++ 标准库函数不读取能通过其他线程访问的对象，除非这些对象能直接或间接地经由函数参数，包含 this 指针访问。   
- 能同时在同一容器上由不同线程调用 const 成员函数。而且，成员函数 begin() 、 end(), rbegin() 、 rend() 、 front() 、 back() 、 data() 、 find() 、 lower_bound() 、 upper_bound() 、 equal_range() 、 at() 和除了关联容器中的 operator[] 对于线程安全的目标表现如同 const （即它们亦能同时在同一容器上由不同线程调用）。更广泛而言， C++ 标准库函数不修改对象，除非这些对象能直接或间接地经由函数参数，包含 this 指针访问。   
- 同一容器中不同元素能由不同线程同时修改，除了 `std::vector<bool>` 的元素（例如， [`std::future`](https://zh.cppreference.com/w/cpp/thread/future) 对象的 `vector` 能从多个线程接收值）。   
- 迭代器操作（例如自增迭代器）读但不修改底层容器，而且能与同一容器上的其他迭代器操作同时由 const 成员函数执行。非法化任何迭代器的容器操作修改容器，且不能与任何在既存迭代器上的操作同时执行，即使这些迭代器未被非法化。  
- 同一容器上的元素可以同时由不指定为访问这些元素的函数修改。更广泛而言， C++ 标准库函数不间接读取能从其参数访问的对象（包含容器的其他对象），除非其规定要求如此。  
- 任何情况下，容器操作（还有算法，或其他 C++ 标准库函数）可于内部并行化，只要不更改用户可见的结果（例如 [`std::transform`](https://zh.cppreference.com/w/cpp/algorithm/transform) 可并行化，但指定了按顺序观览序列的每个元素的 [`std::for_each`](https://zh.cppreference.com/w/cpp/algorithm/for_eachs) 不行）   

可参考：   
[C++11 STL containers and thread safety - Stack Overflow](https://stackoverflow.com/questions/12931787/c11-stl-containers-and-thread-safety)    
[Thread-safety for SGI STL](https://web.archive.org/web/20171221154911/http://www.sgi.com:80/tech/stl/thread_safety.html)   


&emsp;    
&emsp;    

-------
# 2. [`vector`](https://zh.cppreference.com/w/cpp/container/vector) and [`string`](https://zh.cppreference.com/w/cpp/string/basic_string) #
-------

<a id="13"></a>
## Item 13 `vector` 和 `string` 优先于动态分配的数组

再一次看到 string 的引用计数，在此整理一下相关知识：   
- Proxy 设计模式 (区分读和写 - [《More Effective C++》 Item 30](http://www.physics.rutgers.edu/~wksiu/C++/MoreEC++_only.pdf))   
- Linux kernal fork() 机制 ["COW" - 写时复制](https://en.wikipedia.org/wiki/Copy-on-write)   
- [C++ 智能指针库](https://zh.cppreference.com/w/cpp/memory)      
- JVM 垃圾回收 & 引用计数  

如果不想使用引用计数的 `string` ，可以使用 `vector<char>` 代替。

&emsp;    
&emsp;    


<a id="14"></a>
## Item 14 使用 `reserve` 来避免不必要的重新分配

    void std::vector::reserve( size_type new_cap );
>
增加 vector 的容量到大于或等于 new_cap 的值。若 new_cap 大于当前的 capacity() ，则分配新存储，否则该方法不做任何事。   
若 new_cap 大于 capacity() ，则所有迭代器，包含尾后迭代器和所有到元素的引用都被非法化。否则，没有迭代器或引用被非法化。   

使用 `reserve` 有两种方式。  

- 确切知道容器最终要容纳多少元素   
- 先预留足够大的空间，最后再去除多余的容量（参考 [Item 17](#17)）  

&emsp;    
&emsp;    


<a id="15"></a>
## Item 15 注意 `string` 实现的多样性





&emsp;    
&emsp;    


<a id="16"></a>
## Item 16 




&emsp;    
&emsp;   


<a id="17"></a>
## Item 17 




&emsp;    
&emsp;   


<a id="18"></a>
## Item 18 避免使用 `std::vector<bool>`

参考 `std::string` 的 lazy evaluation 实现，为了区分读和写，使用了 `proxy` 对象，但是使用代理对象会引发一些问题，比如模板实参推导等。

&emsp;    
&emsp;   


<a id="19"></a>
## Item 19 



&emsp;    
&emsp;   


<a id="20"></a>
## Item 20 



&emsp;    
&emsp;   


<a id="21"></a>
## Item 21 



&emsp;    
&emsp;   


<a id="22"></a>
## Item 22 



