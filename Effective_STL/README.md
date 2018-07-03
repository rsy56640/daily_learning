# Effective STL learning note#
----
2018-7-3 开始阅读，因为之前自己写过STL的部分组件，所以打算快速的过一遍，就当是某种意义上的补全了。  
我大概浏览了一下，本书标准大概是C++98，也就是说有很多落后的地方，比如： `auto_ptr`, `move semantics`, `unordered_map` 等.
#1. Container##
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


##Item 1 慎重选择容器类型##
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


##Item 2 不要试图编写独立于容器的代码##
- use static_assert(since c++11), concept(since c++20)
- try best to achieve low coupling
- the constraints originate from regulations being different for iterators invalidation among different containers.  
  eg. 
  - `deque::insert` invalidates all of iterators, thus you'd assume all the iterators have been invalidated.
  - 臭名昭著的 `std::vector<bool>`   
- containers are different, and they are not designed to use identically.  

Solution: **encapsulation**   
&emsp;- put the container into a low-level Tool class.(container will not expose to users, and just forward the methods)  
&emsp;- use `typedef` or `using`  

&emsp;    
&emsp;    


##Item 3 确保容器中的对象拷贝正确而高效##
copy -> performance bottleneck  
&emsp;&emsp;&nbsp;&nbsp;&nbsp; | object slicing
&nbsp;  
Solution: use pointer  
&emsp;&emsp;&emsp;&emsp; move semantics  

&emsp;    
&emsp;    


##Item 4 调用 empty 而不是检查 size()是否为0##
`empty()` costs constant time for all standard containers.  
有趣的是，`std::list::size()` 在C++11中复杂度为 O(1)   
`std::list` 有几个有趣的操作:  
- `splice() O(1) or O(N)`  
- `sort()   O(NlogN)`  
- `size()   O(1)`  
这个值得一写，可惜我现在还没写过｡:.ﾟヽ(｡◕‿◕｡)ﾉﾟ.:｡+ﾟ  

&emsp;    
&emsp;    


##Item 5 区间成员函数优先于与之对应的单元素成员函数##
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


##Item 6 ##




&emsp;    
&emsp;    

##Item 7 ##




&emsp;    
&emsp;    

##Item 8 ##




&emsp;    
&emsp;    

##Item 9 ##




&emsp;    
&emsp;    

##Item 10 ##




&emsp;    
&emsp;    

##Item 11 ##




&emsp;    
&emsp;    

##Item 12 ##




&emsp;    
&emsp;    

##Item 13 ##




&emsp;    
&emsp;    

##Item 14 ##




&emsp;    
&emsp;    

##Item 15 ##




&emsp;    
&emsp;    

##Item 16 ##




&emsp;    
&emsp;   

##Item 17##




