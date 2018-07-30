# Python Practice #
参考资料：《廖雪峰Python教程》

----------------------------

<a id="1"></a>
## 常用数据结构 ##
    list:   [1, '2', [2, 3], "hello"]
    tuple:  ()
    dict:   {"3": 4, 4: 4, int(input()) + 3: 6}


&nbsp;   
<a id="2"></a>
## 函数 ##

    def power(x, n=2):
        s = 1
        tmp = x
        while n:
            if n & 1:
                s *= tmp
            tmp *= tmp
            n >>= 1
        return s

可变参数  

`*arg`:   

    def sum_square(*numbers):
        sum = 0
        for number in numbers:
            sum += number * number
        return sum
        
    print(sum_square(1, 2, 3))
    d = (1, 2, 3)
    print(sum_square(*d))

`**kw`:   

    def test_keyword(name, **kk):
        print(name, kk)
        
    d = {1: 11, 2: 22}
    test_keyword("rsy", a="hhh", b=2, c=d)
    test_keyword("rsy", **{'a': "hhh", 'b': 2, 'c': d})

### 函数参数 ###
>
\*args 是可变参数，args 接收的是一个 tuple； 
\*\*kw 是关键字参数，kw 接收的是一个 dict。  
以及调用函数时如何传入可变参数和关键字参数的语法：   
>>
  可变参数既可以直接传入：func(1, 2, 3)；   
  又可以先组装 list 或 tuple，再通过\*args 传入：func(*(1, 2, 3))； 
>
关键字参数既可以直接传入：func(a=1, b=2)；   
又可以先组装 dict，再通过\*\*kw 传入：func(\*\*{'a': 1, 'b': 2})。   

### 切片 ###
    d = range(100)[7:15]

### 迭代 ###
    for x in d:
        print(x)

### 列表生成式 ###
    d = [x * x for x in range(1, 11)]

### 生成器 ###
    generator = (x * x for x in range(10))  
    for x in range(10):
        print(generator.__next__())
    for x in generator:
        print(x)


    def fib_g(max):
        n, a, b = 0, 0, 1
        while n < max:
            yield b
            a, b = b, a + b
            n = n + 1
    a = fib_g(6)
    for x in a:
        print(x)


&nbsp;   
<a id="3"></a>
## 函数式编程 ##

### Map - Reduce ###
    from functools import reduce
    def str2int(str):
        def char2num(ch):
            return {'0': 0, '1': 1, '2': 2, '3': 3, '4': 4, '5': 5,
                    '6': 6, '7': 7, '8': 8, '9': 9}[ch]
        return reduce(lambda x, y: 10 * x + y, map(char2num, str))
    print(str2int("00106893"))  # output: 106893

**RSY 注：**  

- `map()` 在 C++ 中有类似的替代：[`std::for_each()`](https://en.cppreference.com/w/cpp/algorithm/for_each)，然后唯一的一点差别就是 `map()` 可以改变类型（因为Python是解释型）。而 C++17 引入了 [`std::variant`](https://en.cppreference.com/w/cpp/utility/variant)，这使得我们拥有了 "safer union"。  
- `reduce()` 类似 [`std::accumulate()`](https://en.cppreference.com/w/cpp/algorithm/accumulate)，可以自定义二元代数运算。

### filter ###
    def is_odd(n):
        return n % 2 == 1
        
    d = filter(is_odd, [1, 2, 3, 4, 5, 6])
    for x in d:
        print(x)
        
参考 [erase - remove idiom in C++](https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Erase-Remove)

### sorted ###

### 返回函数 ###
    def lazy_sum(*args):
    def sum():
        ans = 0
        for n in args:
            ans = ans + n
        return ans

    return sum

    f = lazy_sum(*(1, 2, 3, 4, 5))
    print(f)
    print(f())  # output 25


### 闭包 ###
闭包引用了外部的变量：   
所以 **返回函数不要引用任何循环变量，或者可能发生变化的变量**。      

    def count():
        fs = []
        for i in range(1, 4):
            def f():
                return i * i

            fs.append(f)
        return fs


    f1, f2, f3 = count()
    print(f1())  # output 9
    print(f2())  # output 9
    print(f3())  # output 9

如果一定要引用循环变量怎么办？方法是再创建一个函数，用该函数的参数绑定循环变量当前的值，无论该循环变量后续如何更改，已绑定到函数参数的值不变：   

    def count2():
        fs = []
        for x in range(1, 4):
            def f(i):
                def g():
                    return i * i
    
                return g
    
            fs.append(f(x))
        return fs
    
    
    f1, f2, f3 = count2()
    print(f1())  # output 1
    print(f2())  # output 4
    print(f3())  # output 9

或者这么写：   

    def count2():
        fs = []
        for x in range(1, 4):
            def f(i):
                return lambda: i * i
    
            fs.append(f(x))
        return fs
    
    
    f1, f2, f3 = count2()
    print(f1())  # output 1
    print(f2())  # output 4
    print(f3())  # output 9


### 匿名函数 ###

    lambda [param] : [return statement]

### 装饰器 ###
这个感觉挺不错   

    def log(func):
        def wrapper(*args, **kw):
            print("call %s():" % func.__name__)
            func(*args, **kw)
            print("call finished")
    
        return wrapper
    
    @log
    def now():
        print("123456")  
    
    now()

### 偏函数 ###
类似 [`std::bind()`](https://en.cppreference.com/w/cpp/utility/functional/bind)   
可变长参数模板也可以用   

    import functools
    
    int2 = functools.partial(int, base=2)
    
    print(int2("10001"))


&nbsp;   
<a id="4"></a>
## 模块 ##



&nbsp;   
<a id="5"></a>
## 面向对象 ##

    class Student(object):
        def __init__(self, name, sex):
            self.__name = name
            self.__sex = sex
    
        def info(self):
            print(self.__name, self.__sex)
    
    me = Student("rsy", "M")
    me.info()         # output "rsy M"
    me.__name = "aaa"
    me.info()         # output "rsy M"
    print(me.__name)  # output "aaa"

###`isinstance()`, `type()`, `dir()`###

    isinstance()      # 返回布尔值，满足里氏替换原则
    type()            # 和另一个 type 比较
    dir()             # 返回一个包含字符串的 list，包含一个对象的所有属性和方法

###`hasattr()`, `setattr()`, `getattr()` ###

    print(hasattr(me, "__name"))   # output "False"
    print(hasattr(me, "age"))      # output "False"
    setattr(me, "age", 21)
    print(getattr(me, "age"))      # output "21"

###`__slots__`###
限制运行时为对象动态添加属性

    class B1(object):
        __slots__ = ("id")
    
    
    class D1(B1):
        __slots__ = ("hh")
    
    
    b1 = B1()
    b1.id = 1
    # b1.name = "rsy" # 报错
    
    d1 = D1()
    d1.id = 2
    d1.hh = "asd"

###`@property`###

    class Student(object):
        def __init__(self, name, sex):
            self.__name = name
            self.__sex = sex
    
        def info(self):
            print(self.__name, self.__sex)
    
        @property
        def name(self):
            return self.__name
    
        @name.setter
        def name(self, name):
            if not isinstance(name, str):
                raise ValueError("string needed")
            self.__name = name
    
    
    me = Student("rsy", "M")
    me.name = "asd"
    print(me.name)

### 多重继承 ###


### 定制类 ###

    __str__(self)      __repr__(self)
    __iter__(self)     next(self)
    __getitem__(self, n) # n 可以是index，也可以是slice
    __getattr__(self, attr) # 什么玩意儿
    __call__(self) # 函数对象

### 使用元类 ###



&nbsp;   
<a id="6"></a>
## 异常处理 ##





&nbsp;   
<a id="7"></a>
## I/O编程 ##



&nbsp;   
<a id="8"></a>
## 进程和线程 ##




&nbsp;   
<a id="9"></a>
## 常用内建模块 ##




&nbsp;   
<a id="10"></a>
## 图形界面 ##




&nbsp;   
<a id="11"></a>
## 网络编程 ##




&nbsp;   
<a id="12"></a>
## PIL图像处理 ##




&nbsp;   
<a id="13"></a>
## numpy ##




