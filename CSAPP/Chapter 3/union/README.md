`union`规避了C语言的类型系统，允许以多种类型来引用一个对象。   
`union`十分有用：  

- 事先知道对一个数据结构中的两个不同字段的使用是互斥的，那么将这两个字段声明为联合的一部分，而不是结构的一部分，会减小分配空间的总量     
- 用来访问不同数据类型的位模式   
  - 验证处理器的字节序，即 Little-ending & Big-ending   
  - 按位读取不同类型的数据   

举个例子，我印象中第一次见到 `union` 在设计中起到的作用：   
[《More Effective C++》 Item 28 - p144](http://www.physics.rutgers.edu/~wksiu/C++/MoreEC++_only.pdf)   
为了让智能指针的模板类型可以从 `const T` 接受 `T` ，将  `const T` 设计为基类，并含有一个 `union`，包含了 `const` 和 `non-const`。

    template<class T>                // 指向 const 对象的
    class SmartPtrToConst {          // 灵巧指针
    ...                              // 灵巧指针通常的
                                     // 成员函数
    protected:
        union {
            const T* constPointee;   // 让 SmartPtrToConst 访问
            T* pointee;              // 让 SmartPtr 访问
        };
    };
    
    template<class T>                // 指向 non-const 对象
    class SmartPtr:                  // 的灵巧指针
        public SmartPtrToConst<T> {
        ...                          // 没有数据成员
    };
