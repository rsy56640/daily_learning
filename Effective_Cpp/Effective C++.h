#pragma once
/*****************   Effective C++   *****************
*	RSY注：
*		Effective C++ 第三版在2006年面世，而C++11/14引入了许多新机制，所以有些地方要变通一下。
*		例如：C++11 defaulted/deleted unique/shared/weak_ptr而摒弃auto_ptr
*
*		强烈推荐的几个网站：
*		http://zh.cppreference.com/w/cpp
*		http://www.stroustrup.com/index.html
*		https://isocpp.org/faq
*		http://www.boost.org/
*		http://www.cplusplus.com/
*
*		如果在后面有例子会注明:		ex.			！！！！！！！！！！！！！！！！！！
*		本读书笔记仅供参考，有些地方可以灵活变通。
*		谨此.
*
******************************  Chapter 1 让自己习惯C++  ******************************
***** 条款1 视C++为一个语言联邦
*		C / Object-Oriented C++ / Template C++  (template metaprogramming) / STL
*
*
***** 条款2 尽量以const, enum, inline 替换 #define
*		ex.
*		NB: 对于class的静态常成员 可用const 或 enum
*			enum hack: enum无法取地址
*			虽然优秀的compiler不会为 整形const 设定另外的存储空间（除非创建指针或引用）
*			但不够优秀的compiler可能如此，因此用enum不对导致非必要的内存分配
*			此外enum hack 是template metaprogramming（模板元编程）的基础技术
*		用template inline 代替 宏函数
*
*
***** 条款3 尽可能使用const
*		1.令函数返回常量值，可以高效降低意外  //考虑(a+b)=c;
*		2.至于const参数，除非你有需要改动参数或local对象，否则请声明为const
*	 ***3.const 成员函数
*			1.使class接口容易理解，哪个函数可以改动对象而哪个函数不行
*			2.使“操作const对象”成为可能
*				因为，高效地根本方法是传递“常引用”，
*			 ***而此技术可行的前提是：有const成员函数可用来处理取得的const对象
*			3.可以被重载
*			有两种观点：bitwise constness \ logical constness
*			ex.
*			bitwise constness：不更改对象内任何一个bit，compiler只需要寻找成员变量赋值动作
*							   这正是C++ 对constness的定义
*			logical constness：引入mutable							ex.
*	 ***4.在const和non-const成员函数中避免重复						ex.
*			！！！为了减少额外的代码、编译时间，和维护成本等，必须让其中一个调用另一个
*			这促使我们进行 常量性移除(casting away constness)
*			！！！用const实现non-const！！！形式：
*		class A{
*			const T foo()const;
*			T foo(){
*				return
*					const_cast<T>(							//将返回值const移除
*						static_cast<cnost A&>(*this)		//为this加上const，避免递归
*							.foo()							//调用const foo()
*					);
*			}
*		};
*			反向做法：用non-const函数实现const函数（绝对不该做）
*						因为non-cast有可能改变对象，在const中调用non-cast冒了风险
*
*	NB:	1.将某些东西声明为 const 有助于编译器发现使用错误。
*		  const 能被用于任何 scope（范围）中的 object（对象），
*				    用于 function parameters（函数参数）和 return types（返回类型），
*				    用于整个 member functions（成员函数）。
*		2.编译器坚持 bitwise constness（二进制位常量性），
*		  但是你应该用 logical constness（逻辑上的常量性）来编程。
*		3.当 const 和 non-const member functions（成员函数）具有本质上相同的实现的时候，
*		  使用 non-const 版本调用 const 版本可以避免 code duplication（代码重复）。
*
*
***** 条款4 确定对象被使用前已被初始化
*			1.C++规定，对象的成员变量的初始化动作发生在进入构造函数本体之前
*			  在构造函数的函数体内的操作是 赋值(assignment)！！
*			  因此最好使用  初始化列 表(member initialization list)！！
*			2.初始化列表 对成员调用其copy ctor！！
*			  对于不在初始化列表中的用户自定义类型，compiler会在构造函数体执行之前
*			   调用其default ctor
*			3.对于内置类型，最好也使用初始化列表，如果是const或reference，
*			  一定需要初值而不能被赋值！！
*			总结：为避免记住成员何时必须使用初始化列表，
*				  最简单的做法就是：总是使用 初始化列表！！！！！（最好以声明次序初始化）
*
*			对于non-local static对象，全部要做的就是
*			将每一个 non-local static object（非局部静态对象）移到它自己的函数中，
*			在那里它被声明为 static（静态）。这些函数返回它所包含的 objects（对象）的引用。
*			客户可以调用这些函数来代替直接涉及那些 objects（对象）。
*			换一种说法，就是用 local static objects（局部静态对象）
*			取代 non-local static objects（非局部静态对象）。
*			注：这是Singleton模式中一个常见实现手法
*
*	NB: 1.手动初始化 built-in type（内建类型）的 objects（对象），
*		  因为 C++ 只在某些时候才会自己初始化它们。
*		2.在 constructor（构造函数）中，用 member initialization list（成员初始化列表）
*		  代替函数体中的 assignment（赋值）。
*		  initialization list（初始化列表）中 data members（数据成员）
*		  的排列顺序要与它们在 class（类）中被声明的顺序相同。
*		3.通过用 local static objects（局部静态对象）
*		  代替 non-local static objects（非局部静态对象）
*		  来避免跨 translation units（转换单元）的
*		  initialization order problems（初始化顺序问题）。
*
*
*
******************************  Chapter 2 构造/析构/赋值运算  ******************************
*	确保ctor、dtor、copy assignment行为正确
*
*
***** 条款5 了解C++默认编写并调用了哪些函数
*			见operator overload.cpp
*
*		1.如果class内含 const或reference 成员，必须自己定义copy assignment！！！！！
*		  copy ctor可以使用初始化列表对初始化const/ref
*		2.如果base class将copy assignment声明为private，
*		  compiler将拒绝为derived class生成默认copy assignment。
*
*	NB: 编译器可以隐式生成一个 class（类）的 default constructor（缺省构造函数），
*	    copy constructor（拷贝构造函数），copy assignment operator（拷贝赋值运算符）
*	    和 destructor（析构函数）。
*
*	注：关于move constructor！！！！
*		If the definition of a class X does not explicitly declare a move constructor,
*	one will be implicitly declared as defaulted if and only if:
*		当没有显式声明move ctor时，compiler隐式声明缺省move ctor当且仅当：
*		1.X does not have a user-declared copy constructor.			//未声明copy ctor
*		2.X does not have a user-declared copy assignment operator.	//未声明copy assignment
*		3.X does not have a user-declared move assignment operator.	//未声明move assignment
*		4.X does not have a user-declared destructor.				//未声明dtor
*		5.the move constructor would not be implicitly defined as deleted.
*		当没有move ctor时，A a2(std::move(a1))，会调用copy ctor
*
*	An implicitly-declared copy/move constructor is an inline public member of its class.
*	A defaulted copy/move constructor for a class X is defined as deleted if X has:
*	https://stackoverflow.com/questions/4819936/why-no-default-move-assignment-move-constructor
*
*
***** 条款6 若不想使用compiler自动生成的函数，就该明确拒绝
*		如果某个对象理应是独一无二的并且没有副本，就应该禁止copy操作！！！
*
*		1.声明 copy(ctor/assignment) 为 private，并不实现
*		  但是并不绝对安全，如果member/friend函数调用copy，会获得一个连接错误(linkage error)
*		class A{
*		public:
*			......
*		private:
*			A(const A&);					//声明为private，并不予实现
*			A& operator=(const A&);
*		};
*		2.将link time error转移至compile time是可能的					ex.
*		  专门建立一个base class，将copy声明为private
*		class Uncopyable {
*		public:
*			Uncopyable() {}
*			~Uncopyable() {}
*		private:
*			Uncopyable(const Uncopyable&);
*			Uncopyable& operator=(const Uncopyable&);
*		};
*		class A :public Uncopyable {};
*		这样如果A的member/friend函数调用copy，会被compiler拒绝
*
*		3.C++11  default/delete											ex.
*			推荐：即使不需要写comiler默认生成的函数，最好也写成default！！！！
*			原因：随着移动语义(右值引用)的引入，某些情况下会变得蹊跷——不是很懂。。。
*
*			class C {
*				C(const C&) = default;
*				C(C&&) = default;
*				C& operator=(const C&) & = default;			//& is ref qualifier
*				C& operator=(C&&) & = default;
*				virtual ~C() = default;
*			};
*			注：上面的 &= 是 ref qualifier，表明
*				想要被赋值的对象是non-const lvalues。
*				如果没有& 则可以对rvalue赋值。
*	参考：http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2009/n2819.html
*
*		  对于 2个ctor、dtor 和 operator=，
*		  哪怕函数体是空的，增加了程序员的编程工作量。
*		  更值得一提的是，手动定义的析构函数的代码执行效率要低于编译器自动生成的析构函数
*		  因此可以用 =default 或 =delete
*		  此外，
*		  i)delete可以禁止某些类型转换构造函数，如：
*		  A(double);						//A a(3.14)		OK
*		  A(int) = delete;					//A a(2)		错误
*		  ii)delete可以禁止用户进行new操作
*		  void *operator new(std::size_t) = delete;
*		  void *operator new[](std::size_t) = delete;
*		  iii)delete必须在第一次声明是声明，即inline；default类内外都可以
*
*
*	NB: 为拒绝compiler自动提供的机能，可将相应的member函数声明为private并且不予实现。
*		使用像 Uncopyable/noncopyable 这样的 base class是一种做法。
*
*
***** 条款7 为多态基类声明virtual析构函数
*		设计原理：（多态本质）通过base class接口处理derived class对象！！！！！
*		Factory Method: 定义一个用于创建对象的接口，让子类决定实例化哪一个类。
*						Factory Method 使一个类的实例化延迟到其子类。
*		1.在工厂模式中，通常动态分配的派生类对象，并返回一个基类指针(which指向它)		ex.
*		  在删除时！！理应调用派生类析构函数！！
*		**析构函数是一级一级往基类调用的，所以最好声明dtor为virtual**
*		2.任何class只要带有virtual函数，都几乎确定理应有一个virtual dtor！！
*		3.如果class不含virtual函数，通常表示它并不意图被用作一个base class。
*		  当class不起图作为base class时，声明dtor为virtual往往是个馊主意！！
*
*		4. vptr 虚函数表指针
*			对象携带某些信息，在runtime决定哪一个virtual函数被调用。
*			vptr指向一个有函数指针构成的数组，成为vtbl（虚函数表）。
*			每个含有virtual函数的class都有一个相应的vtbl，当对象调用某一virtual函数，
*			实际被调用的函数取决于该对象的vptr所指的vtbl——compiler在其中寻找适当的函数指针
*
*			vptr增加了对象的体积，并存储在对象的前四个字节(可以用reinterpret_cast取出来玩:D)
*			盲目地声明virtual dtor的后果：
*				1.增加对象体积(4B)		2.不再具有移植性
*
*		5.对于并非用作 多态base class，时，有时会！错误！地运用abstract工厂模式！！！
*		  比如std::string/STL container，它们并非设计用来：
*		 “由base class接口处理derived class对象”
*		6.pure virtual function													ex.
*		  纯虚函数导致抽象类，即不能被实体化(instantiated)
*		  有时你希望拥有abstract class，但没有纯虚函数。这这时，你可以：
*			1.声明一个pure virtual dtor
*			2.然而这里有个窍门，你必须为这个pure virtual dtor提供一份定义
*			  否则会出现linkage error
*
*
*	NB: 1.polymorphic base classes（多态基类）应该声明 virtual destructor（虚拟析构函数）。
*		  如果一个 class（类）有任何 virtual functions（虚拟函数），
*		  它就应该有一个 virtual destructor（虚拟析构函数）。
*
*		2.不是设计用来作为 base class（基类）或不是设计用于 polymorphically（多态）
*		  的 classes（类）就不应该声明 virtual destructor（虚拟析构函数）。
*
*
***** 条款8 别让异常(Exception)逃离析构函数(Dtor)
*			忠告：析构函数吐出异常 == 危险，
*				  因此给用户一个处理异常的机会（提供一个接口，并在dtor中“双保险”）
*			考虑一个对象数组，析构时，连续抛出若干Exception
*			在两个Exception同时存在的情况下，程序不是 *导致不明确行为* 就是 *结束执行* ！！！
*		1.Solution: 下述两者都无法对 抛出异常的情况 做出反应
*			i)调用abort()，抢先制止 不明确行为
*			~A(){
*				try{......}
*				catch(...){
*					记录异常日志
*					std::abort();			//强迫结束程序
*				}
*			}
*			ii)吞下异常
*			~A(){
*				try{......}
*				catch(...){
*					记录异常日志			//然后继续
*				}
*			}
*		2.较佳策略：		ex.
*			***重新设计接口，赋予用户一个机会得以处理 因该操作而发生的异常***
*				并在dtor中提供双保险。
*
*	NB: 1.dtor绝对不要抛出Exception，如果dtor调用的函数抛出异常，那么dtor应该将其捕捉，
*		  然后吞下异常 或 结束程序。
*		2.如果客户需要对某个操作函数运行期间抛出的异常做出反应，那么class应该提供一个普通函数
*		  而非在dtor中执行该操作。
*
*
***** 条款9 绝不在ctor和dtor中调用virtual函数
*		考虑这样的情况：生成一个多态派生类对象，首先调用base ctor，然后调用派生类ctor
*		问题在于：如果构造函数中调用了virtual函数，不一定会按照预想的发展
*		原因：1.执行base ctor期间，vptr指向base class的虚函数表！！！！
*			  2.执行某类dtor期间，vptr指向该类的虚函数表！！
*
*
*		补充：！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
*		初始化列表执行顺序为：（与初始化列表的顺序无关）
*			基类构造函数、（compiler隐式）虚函数表（指针）、按声明顺序初始化成员变量！！！！
*		派生类构造函数执行过程：
*		在调用base ctor时，将base vptr放在对象前4个字节，
*		出来后，derived class将自己的vptr放在对象前4个字节，
*		从而将基类的虚函数表覆盖，实现多态。
*		之后按照成员变量！！声明次序！！依次进行初始化。
*		！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
*
*		一种更为糟糕的情况是：
*		在ctor中调用non-virtual函数，但是该函数内部调用virtual函数。
*
*		唯一能够避免此问题的做法就是：
*			确定ctor和dtor都没有调用virtual函数，
*			并且它们所调用的函数也服从这一约束。
*
*		那么如何弥补这一没有调用的损失，
*		一种方法是：
*			将函数改为non-virtual，并在base class ctor中加一个参数，
*			在derived class ctor中***传递必要信息***给base ctor，
*			然后base ctor便可以安全调用non-virtual函数
*
*	****换句话说,由于你无法使用virtual函数从base class向下调用。				ex.
*		在构造期间，你可以藉由“令derived class将必要的构造信息向上传递至base class ctor”
*		替换之而加以弥补。
*		这里还有个小trick，创建private static函数
*		比起在成员初始化列表内给予base class所需数据，利用辅助函数创建一个值传给base ctor
*		往往较为方便，也比较可读。
*		令此函数为static，也就不可能意外指向该class对象中未初始化的成员
*
*	NB: 1.在ctor和dtor期间不要调用virtual函数，因为这类调用从不下降至derived class
*		2.弥补措施是改为non-virtual函数，并从derived ctor传递必要信息到base ctor
*
*
***** 条款10 令 operator= 返回一个 reference to *this
*		赋值采用右结合律，为了实现赋值，必须返回一个指向操作符左侧的reference
*
*		A& operator= (const A&){ return *this; }
*
*		也适用于所有赋值运算符，如 *=  += 等
*		特殊情况：		A& operator= (int);		A& operator= (B);
*
*	NB: 令assignment操作符返回一个reference to *this
*
*
***** 条款11 在 operator= 中处理“自我赋值”
*		讨论两个问题：自我赋值安全性、异常安全性
*		1.自我赋值安全性：											ex.
*			在 operator= 中加入“证同测试”(identity test)
*				即if(this == &T) return *this;
*		2.异常安全性
*			在 operator= 中进行new操作时，可能出现如下两种异常
*			1.内存分配不足 2.copy ctor抛出异常
*			令人高兴的是，让operator=具备异常安全性，往往自动获得自我赋值安全性。
*			因此很多人把焦点放在异常上，对自我赋值倾向于不去管它，
*			条款29深入探讨了异常安全性，
*		 ex.方案1.这里，只要明白一群精心编排的语句就可以得到异常安全（以及自我赋值安全）
*			记住原先的内容，创建副本，然后删除原先内容
*			注：书上这里有一个很小细节的trade-off：		对效率的思考令人废书长叹！！！
*				如果关心效率，可以加入identity test，但考虑到自我赋值发生的频率，
*				这会使得代码体积变大（包括原始码和目标码），并引入新的控制流分支，
*				Prefetching，Caching，Pipelining等指令的效率都会降低。
*		 ex.方案2.一个替代方案是，使用所谓的copy and swap技术
*			这个技术和“异常安全性”密切相关，在条款29中有详细说明。
*		 ex.方案3.另一个变奏曲是基于以下3点
*					1.copy assignment 可以被声明为“以pass by value方式接受实参”。
*					2.以pass by value传参会调用copy ctor，生成一个副本。
*					3.copying动作 从函数体内 移至 参数构造阶段 让compiler生成更高效的代码。
*				这种做法是令人忧虑的，它为了伶俐巧妙的修补而牺牲了清晰性。
*
*	NB: 1.确保当对象自我赋值时 operator= 有良好行为。其中技术包括：
*		  比较“来源对象”和“目标对象”的地址、精心周到的语句顺序、以及copy-and-swap。
*		2.确定任何函数如果操作一个以上的对象，而其中多个对象是同一个对象时，其行为仍然正确。
*
*
***** 条款12 复制对象时勿忘其每一个成分
*		对于copy ctor和copy assignment
*		确保：1.复制所有local成员变量											ex.
*			  2.调用所有base class内的适当copy函数
*			1.在copy ctor中，如果没有在初始化列表中调用base class任何ctor，
*			  compiler会隐式调用base default ctor，如果没有，compile error.
*			2.在copy assignment函数体中，调用格式如下：
*			  Base::operator=(rhs);
*
*		此外，这两个函数虽然实现类似，但不要试图用一个去调用另一个
*		如果实在想消去重复代码，建立一个新的函数给他们调用
*		这个函数往往是private，并且常被命名为init。
*
*	NB: 1.Copying函数应该确保复制“对象内的所有成员变量”及“所有base class成分”。
*		2.不要尝试以某个copying函数实现另一个copying函数。
*		  应将共同机能放进第三个函数中，并由两个copying函数共同调用。
*
*
*
******************************  Chapter 3 资源管理  ******************************
*	资源包括：内存资源、文件描述器、互斥锁、数据库连接、网络sockets。
*	不论哪一种资源，重要的是，当你不再使用它时，必须将它还给系统。
*
*
***** 条款13 以对象管理资源
*		工厂模式下，有些函数返回资源，那么如何确保这些资源被释放。				ex.
*	两个关键想法：1.获得资源后立刻放进管理对象内。
*					此观念常被称为“资源取得时机便是初始化时机”。
*					Resource Acquisition Is Initialization 即 RAII.
*				  2.管理对象运用析构函数确保资源被释放。
*					把资源放进对象，在离开对象作用域时，compiler自动调用对象的dtor。
*					因此，只要在对象的析构函数内执行delete操作即可释放资源。
*					如果资源释放动作可能导致抛出exception，请参见条款8。
*
*			注：书上这里采用auto_ptr，但实际上已被C++11摒弃。
*			关于智能指针的一些补充描述：
*			共有四个：unique_ptr, shared_ptr, weak_ptr, auto_ptr（已被C++11抛弃）
*				构造函数均为explicit，只有在使用动态内存分配时，才能使用智能指针！！
*										或者你可以自定义deleter。见条款14
*				unique_ptr：对于一个对象，只能有一个指针可拥有它，否则compile error。
*							如果程序试图将一个unique_ptr赋值给另一个时，
*							如果源unique_ptr是个将亡值(xvalue)，编译器是允许的，否则CE。
*							因此可以使用 std::move()或 函数返回值，对unique_ptr进行赋值。
*							此外，unique_ptr有可用于数组的变体 即new[]，相较于auto_ptr来说。
*				shared_ptr：跟踪引用特定对象的智能指针计数，
*							仅当最后一个指针过期时，才调用delete。
*							在unique_ptr为右值时，可以将其赋值给shared_ptr，
*							shared_ptr将接管原来unique_ptr拥有的对象。
*							此外，shared_ptr不能托管数组 即没有new[]/delete[]
*
*			最后，必须指出，返回未加工的指针及其危险，最好将返回值存储于智能指针对象内。
*
*	NB: 1.为防止资源泄漏，请使用RAII对象，它们在ctor中获得资源并在dtor中释放资源。
*		2.常被使用的RAII classes分别是 unique_ptr 和 shared_ptr。
*
*
***** 条款14 在资源管理中小心copying行为
*		条款13引入了RAII的观念，并作为资源管理类的脊柱，
*		然而并非所有资源都是heap-based，于是有时需要建立自己的资源管理类！！！！
*
*		有一个普遍问题是：“当一个RAII对象被复制，会发生什么事？”
*		通常有如下两种选择：
*		ex.	1.禁止复制
*				有时候允许RAII对象被复制并不合理，这时便应该禁止它。
*				参见条款6，声明为delete 或 继承Uncopyable类。
*
*		ex.	2.对底层资源使用“引用计数法”(reference-count)
*				这类似于shared_ptr，通常只要内含一个shared_ptr成员变量，
*				RAII class便可实现出reference-counting copying行为。
*			注：shared_ptr的缺省行为是“当引用次数为0时，删除其所指之物”。
*				但有时我们想要做的动作并非删除动态分配的内存操作。
*				幸运的是，shared_ptr允许指定所谓的“删除器”(deleter)，
*				那是一个函数(Func)或函数对象(Func obj)，当引用次数为0时便被调用。
*				deleter是shared_ptr构造函数的第二缺省参数。
*
*		注：正确使用deep copy   /   move ctor / assignment
*
*	NB: 1.复制RAII对象，必须一并复制它所管理的资源，所以资源的copying行为
*		  决定RAII对象的copying行为。
*		2.普遍而常见的 RAII class copying 行为是：抑制 copying、
*		  施行引用计数法(reference counting)。不过其他行为也都可能被实现。
*
*
***** 条款15 在资源管理类中提供对原始资源的访问
*		许多API直接指涉资源，所以有时必须绕过资源管理对象直接访问原始资源。
*		这时需要一个可将RAII class对象转换为其所内含之原始资源。
*		有两个做法：
*		ex.	1.显式转换：
*				提供get()成员函数
*		ex.	2.隐式转换：
*				重载了 operator* 和 operator->
*				他们允许隐式转换至原始底部指针。
*		此外，还有一种显式做法：
*			在RAII class中提供转型函数
*				operator raw_resources_class(){
*					return raw_resources;
*				}
*		ex.	但有可能因为用户输入错误而产生危险。
*
*		注：是否应该提供一个显式转换函数(例如get成员函数)将RAII class转换为其底部资源，
*			或是应该提供隐式转换，答案主要取决于RAII class被设计执行的特定工作，
*			以及它被使用的情况。最佳设计很可能是坚持条款18的忠告：
*			“让接口容易被正确使用，不易被误用。”
*			通常显式转换函数如get()是比较受欢迎的做法。
*
*		RAII class并不是为了封装某物而存在；他们的存在是为了确保“资源释放”会发生。
*		设计良好的classes隐藏了客户不需要看的部分，但备妥客户需要的所有东西。
*
*	NB: 1.API往往要求访问原始资源(raw resources)，所以每一个RAII class应该提供一个
*		  “取得其所管理之资源”的办法。
*		2.对原始资源的访问可能经由显式转换或隐式转换。一般而言显式转换比较安全，
*		  但隐式转换对客户比较方便。
*
*
***** 条款16 成对使用 new 和 delete 时要采取相同的形式
*		使用new时，有两件事发生：1.分配内存		2.针对此内存会有一个（或若干）ctor被调用。
*		使用delete时，有两件事发生：1. 针对此内存会有一个（或若干）dtor被调用	2.释放内存
*
*		单一对象的内存布局 一般而言不同于 对象数组的内存布局！！！
*		更明确地说，数组所用的内存一通常还包括“数组大小”的记录，
*		以便delete[]知道需要调用多少次dtor。，单一对象的内存则没有这笔记录。
*		可以把内存布局想象如下：（事实上很多compiler都是这么做的）
*					_________
*		单一对象	| Object |
*					|________|
*
*					____________________________
*		对象数组	| n | Object | Object | ...
*					|___|________|________|_____
*
*		唯一能让compiler知道是否存在“数组大小记录”的办法就是：加上[]
*		delete形式使用错误会导致未定义行为，这让人不悦。
*		规则很简单：一个new与一个delete完全对应；一个new[]与一个delete[]完全对应。
*
*		注意：当撰写一个class含有一个指针指向动态分配内存，并提供多个ctor时，
*			  应尽量做到使用相同形式的new将指针成员初始化。
*			  并且在dtor中使用正确形式的delete。
*
*	ex.	关于typedef：
*			typedef的作者必须说清楚，当程序员以new创建该种typedef类型对象时，
*			该以哪一种delete形式删除。
*			为避免诸如此类的错误，最好尽量不要对数组形式做typedef动作。
*
*	NB: 1.new与delete，new[]与delete[]，一一对应。
*
*
***** 条款17 以独立语句将newed对象置入智能指针
*	ex.	如果在函数调用的参数内创建RAII对象，有可能发生资源泄露！！！
*		在调用函数之前，compiler必须创建参数，这个次序是无法预测的。
*		考虑一种情况：
*			在分配一块资源后，compiler执行了其它参数的构造，
*			之后才把资源置入智能指针。如果在中间抛出异常，
*			分配空间返回的指针将遗失，引发资源泄露。
*		是的，因为在“资源被创建”和“资源被转换为资源管理对象”
*		两个时间点之间有可能发生异常干扰！！
*
*		避免这类问题的办法很简单：使用分离语句
*			1.在单独语句内以智能指针存储newed所得对象。
*			2.把智能指针传递给调用函数。
*
*		注：如果在调用函数的参数构造阶段抛出异常，那么直接退出函数。
*
*	NB: 1.以独立语句将newed对象置入智能指针内。
*		  如果不这样做，一旦异常被抛出，有可能导致难以察觉的资源泄漏。
*
*
******************************  Chapter 4 设计与声明  ******************************
*	本章介绍良好C++接口的设计与声明，包括：
*	正确性、高效性、封装性、维护性、延展性，以及协议的一致性。
*
*
***** 条款18 让接口容易被正确使用，不易被误用
*	ex.	1.许多客户端错误可以因为导入新类型而获得预防。
*			比如：用户以错误的顺序传递参数。
*			可以引入外覆类型(wrapper types)
*	ex.		此外，限制其值有时也是理所应当的。
*
*		2.限制用户错误的办法是，限制类型内什么事可做，什么事不能做。
*			除非有好理由，否则 应该尽量令你的types的行为与内置types一致。
*			避免无端与内置类型不兼容，真正的理由是为了提供行为一致的接口。
*			注：不一致性对开发人员造成的心理和精神上的摩擦与争执，没有任何一个IDE可以完全抹除.
*
*	ex.	3.任何接口如果要求用户必须做某事，就是有着“不正确使用”的倾向，因为用户可能会忘记。
*			比如factory函数返回一个资源，为了避免资源泄露，返回的指针必须删除，
*			这会导致用户错误。
*				一个办法是用户将返回的资源置入智能指针内，
*				但如果用户忘记使用智能指针怎么办？
*			许多时候，较佳的接口设计原则是！先发制人！，就令factory函数返回一个智能指针。
*			std::tr1::shared_ptr<Investment> createInvestment();
*			这样便实质上强迫用户将返回值存储于一个std::tr1::shared_ptr内，
*			几乎消弭了忘记删除资源的可能性。
*
*			此外，std::tr1::shared_ptr允许当智能指针建立起来是制定一个资源释放函数
*			(所谓 deleter)绑定到智能指针身上。
*
*	ex.	4.假设现在需要指定一个  资源释放方法！！（而非直接使用delete）
*			针对此问题可以先发制人，
*			把这个资源释放方法在shared_ptr内是可行的，
*			返回一个将“指定 资源释放方法 绑定为deleter”的shared_ptr
*
*			小trick: shared_ptr第一个构造参数要求是指针，可以采用转型：
*				std::tr1::shared_ptr<A> p(static<A*>(0), deleter);
*				std::unique_ptr<A, void(*)(A*)> p(static_cast<A*>(0), deleter);
*			当然，如果创建智能指针之前就获得了对象，直接置入只能指针即可，参考Item 26
*
*		5.此外，shared_ptr有一个特别好的性质是：
*			会自动使用其deleter，因而消除另一个潜在的用户错误：所谓的"cross-DLL problem".
*			这个问题发生于“对象在动态链接程序库(DLL)中被new创建，却在另一个DLL内被delete销毁”
*			这一类错误往往导致运行期错误。而shared_ptr没有这个问题，
*			因为其缺省的deleter是来自“shared_ptr诞生所在的那个DLL”的delete.
*
*
*		关于std::tr1::shared_ptr的一点补充：
*			最常见的shared_ptr来源于Boost.
*			Boost的shared_ptr是原始指针(raw pointer)的两倍大，
*			以动态分配内存作为簿记用途和“删除器值专属数据”，
*			以virtual形式调用deleter，并在多线程程序修改引用次数时
*			蒙受线程同步化(thread synchronization)的额外开销。
*			（只要定义一个预处理器符号就可以关闭多线程支持）
*			总之，它比raw pointer大且慢，并且使用动态分配内存。
*			在许多应用程序中这些额外的执行成本并不显著，
*			然而其“降低用户错误”的成效确实有目共睹。
*
*
*	NB: 1.好的接口容易被正确使用，不易被误用。
*		2.“促进正确使用”的办法包括接口的一致性，以及与内置类型的行为兼容。
*		3.“阻止误用”的办法包括：1)建立新类型 2)限制类型上的操作 3)束缚对象值
*								  4)以及消除用户的资源管理责任！！！
*		4.std::tr1::shared_ptr支持定制型删除器(custom deleter)，这可预防DLL问题，
*		可被用来自动解除互斥锁等。
*
*
***** 条款19 设计class犹如设计type
*	设计class就像设计内置类型一样，其设计规范应该考虑到以下：
*		1.对象的创建和销毁。ctor, dtor, operator new/delete
*		2.initialization vs. assignment
*		3.何时使用pass-by-value，这意味着什么。
*		4.处理数据异常。
*		5.是否存在继承关系。
*		6.需要怎样的type cast
*		7.什么样的函数是合理的，member/friend/non-member？
*		8.什么样的函数应该驳回：private
*		9.成员的封装性。
*		10.未声明接口。
*		11.type是否一般化，若为一般化，就应定义class template。
*
*
***** 条款20 宁以 pass-by-reference-to-const 替换 pass-by-value
*	pass-by-value会调用copy ctor创建参数
*		采用pass-by-reference-to-const的好处：
*			1.没有构造和析构成本，效率高
*			2.避免对象切割(slicing)
*				derived对象如果以pass-by-value形式传递给base对象，
*				那么调用base copy只创建了一个base对象，失去了derived的特化性质。(例如vptr)
*
*		对于内置类型，pass-by-value往往比pass-by-reference-to-const更加高效，
*		这个忠告也适用于 STL的Iterator 和 函数对象，它们被设计为以pass-by-value方式传递。
*		（Iterator和函数对象的实践者有责任确保它们 高效 并且 不受切割问题的影响）
*
*
*		NB: 1.尽量以pass-by-reference-to-const替换pass-by-value，
*			  前者通常比较高效，并可避免切割问题。
*			2.以上规则不适用于内置类型，STL的迭代器和函数对象，
*			  它们往往pass-by-value更加合适。
*
*
***** 条款21 必须返回对象时，别妄想返回其reference
*		任何函数如果返回一个reference指向某个local对象，都将一败涂地。
*		于是，有了如下考虑：
*			我们在heap上构造一个对象，并返回其引用；
*			即使调用者出于谨慎，并有良好习惯，他们还是无法在某些情况下避免内存泄漏。
*			这种情况是：无法取得指针！！！（比如在一个复杂的单语句中，有些参数在其中匿名）
*		也有另一种考虑：
*			让返回的引用指向一个函数内部的static对象。
*			这以意味着一个事实：调用端看到的永远是static变量的“现值”。
*			显而易见的问题是：多线程安全性。
*			此外，如果调用两次函数，那么第一次的结果就失效了，而这难以发觉。
*		最后，C++允许compiler实现者施行optimization，用以改善产出码的效率，
*			而不改变其可观察的行为。
*			例如：Copy Ellision  (N)RVO
*
*		NB: 1.绝不要返回pointer/reference指向local对象。
*			2.除非有把握避免内存泄漏，绝不要返回pointer/reference指向heap-allocated对象。
*			3.绝不要返回pointer/reference指向local static对象，而同时需要多个这样的对象。
*
*
***** 条款22 将成员变量声明为private
*	成员变量应该是private.
*		应该以函数的方式访问数据，理由是：
*			可以替换不同的实现方式，而客户最多只需重新编译。
*				将成员隐藏在函数接口的背后，
*				确保class的约束条件总是满足，并且为“所有可能的实现”提供弹性。
*
*		成员变量的封装性 与“成员变量的内容改变时所破坏的代码数量” 成反比。
*		protected 并不比 public 更具有封装性！！！！
*		从封装的角度看，只有两种访问权限：private(提供封装)和其他(不提供封装)。
*
*		NB: 1.切记将成员变量声明为private
*				这使得约束条件获得保证，并提供充分的实现弹性。
*			2.protected 并不比 public 更具有封装性！！！！
*
*
***** 条款23 宁以 non-member, non-friend 函数替换 member函数
*	愈多东西被封装，我们改变那些东西的能力也就越大。
*	这是我们首先推崇封装的原因：它使我们能够改变事物而只影响有限客户。
*
*		面向对象守则要求数据应该尽可能被封装，
*		然而与直观相反的，member函数带来的封装性比non-member函数低！！
*	ex.	此外，non-member函数允许较大的弹性，最终导致较低的编译依存度(compilation dependency)!
*
*		关于编译依存关系的讨论，参考Item31
*		将函数写在命名空间中，并在每个头文件中声明该命名空间的某些接口。
*		一个明显的好处是：class对于客户是无法扩展的，而命名空间可以。
*
*		NB: 1.宁可拿 non-member/friend函数 替换 member函数。这样做可以
*			  增加封装性、包裹弹性和机能扩充性。
*
*
***** 条款24 若所有参数皆需要类型转换，请为此采用 non-member 函数
*		通常令class支持隐式类型转换通常是个糟糕的主意，
*		当然也有例外，比如 数值类型。
*		考虑
*			a + 1;				//a.operator+(1)		OK
*			1 + a;				//1.operator+(a)		Error
*
*		我们得到一个明显的结论：
*			1.只有参数位于parameter list中，这个参数才是隐式类型转换的合格参与者。
*			2.this无法进行隐式类型转换。
*
*		综上，我们应该让它成为non-member函数，
*		那么有是否有理由设计为friend函数？
*		无论何时如果 可以避免成为friend函数就应该避免！！！！(比如可以调用public接口)
*		不能够只因函数不该成为member，就让它成为friend.
*
*	NB: 1.如果一个函数的所有参数(包括this指针)需要类型转换，那么它一定是non-member.
*
*
***** 条款25 考虑写出一个不抛异常的 swap 函数
*
*	关于C++11以后std::swap的实现使用move ctor
*	VS2015中的实现：
template<class _Ty,
	class> inline
	void swap(_Ty& _Left, _Ty& _Right)
		_NOEXCEPT_OP(is_nothrow_move_constructible<_Ty>::value
			&& is_nothrow_move_assignable<_Ty>::value)
	{	// exchange values stored at _Left and _Right
	_Ty _Tmp = _STD move(_Left);
	_Left = _STD move(_Right);
	_Right = _STD move(_Tmp);
	}
*	由此我们只需使用pImpl手法，并提供该类的move ctor即可。
*	另外注意到如果move ctor中执行了default ctor的功能，有可能会抛出异常！
*
*	发现几个有趣的问题，正是我想知道的：
*	https://stackoverflow.com/questions/6416385/move-semantics-custom-swap-function-obsolete
*	https://stackoverflow.com/questions/8617305/is-specializing-stdswap-deprecated-now-that-we-have-move-semantics
*
*
*	下面讨论本书内容。
*	swap是异常安全性编程的脊柱，以及用来处理自我赋值可能性的一个常见机制。
*	std::swap的早期版本为 1个copy ctor，2个copy assignment和1个dtor.
*	然而，对于很多类，数据较多，我们使用pImpl手法(pointer to implementation)！！！
*	一旦要交换两个对象，我们只需要交换其pImpl指针！！！
*
*	ex.	1.普通类的swap
*			i.	提供public swap接口，做真正的交换工作。
*			ii.	提供所属命名空间专属swap版本，调用public swap接口。
*			iii.提供std全特化版本，调用public swap接口。
*				注：其实ii提供命名空间专属swap版本足够，但是有时客户习惯
*					std::swap();
*				所以对于普通类，std全特化版本也要提供。
*
*
*	ex. 2.模板类的swap		(函数模板无法 偏特化partially speciallize)
*			i.	提供public swap接口，做真正的交换工作。
*			ii.	提供所属命名空间专属swap版本，调用public swap接口。
*					关于无法在std内提供swap的原因：
*						1.函数模板无法偏特化。
*						2.添加一个重载版本。
*							(然而std是个特殊的命名空间，客户可以全特化，但无法加入新内容)
*							(如果你希望你的软件有可预期的行为，请不要添加任何新东西到std内)
*							RSY注：此条并未找到其他依据(std内重载版本可以通过编译)，还需考量。
*
*		客户正确调用swap方式：
*					using std::swap;
*					swap(obj1, obj2);
*
*		当compiler看到对swap的调用，便查找适当的swap。
*		该机制称作ADL：
*			1.查找该类所属命名空间的专属swap版本。
*			2.使用 std::swap 全特化版本		//由于using std::swap; 使得其曝光
*			3.使用 std::swap 缺省版本
*
*	参数依赖查找
*	ADL(argument-dependent lookup)
*	http://zh.cppreference.com/w/cpp/language/adl
*
*
*	总结：
*		如果缺省的std::swap实现效率无法接受，需要自己实现swap。
*		使用pImpl手法
*			1.提供一个public swap接口，做真正的交换工作(调用缺省swap，交换两个指针)
*				注：这个函数绝不应该抛出异常！！！！！
*			2.在所属命名空间中提供一个non-member函数，调用public swap接口。
*			3.如果是普通类，最好对std::swap进行全特化，调用public swap接口。
*		最后，想要调用swap，请确定使用using声明式，以便让std::swap函数曝光，
*		然后不加任何修饰地调用swap，如下：
*			using std::swap;
*			swap(obj1, obj2);
*
*	现在解释成员函数swap绝不能抛出异常！！！
*		那是因为swap一个最好的应用是帮助class提供强烈的异常安全性保障。
*		这一约束只施行与成员版！！不可施行与非成员版！！
*			原因：swap缺省版本以copy ctor/assignment作为基础，而通常这两者都可以抛出异常。
*				  而高效的swap几乎总是对内置类型操作(如指针)，而内置类型操作绝不会抛出异常。
*
*	NB: 1.普通class实现3个版本swap
*		2.class template实现2个版本swap
*		3.使用时	using std::swap;
*					swap(obj1, obj2);
*
*
//由于之前电脑故障，只是把书看完了一遍，但是并未深究新Cpp标准下的各种规范。
//而且最近事还贼多，所以就几乎按照Effective C++(3rd Edition 05年)来完成之后的内容了。
*******************************  Chapter 5 实现  *******************************
*	一般情况下，合理的设计与声明是花费心力最多的两件事。
*	一旦正确完成它们，大多实现都是直截了当。
*	然而还是有些东西要小心：
*		1.太快定义变量可能造成效率上的拖延。
*		2.过度使用转型可能导致代码变慢又难维护，又招来微妙难解的错误。
*		3.返回对象的 handles 可能破坏封装性，并留给客户 dangling handles。
*		4.未考虑异常带来的冲击则可能导致资源泄露和数据败坏。
*		5.过度热心的inlining可能引起代码膨胀。
*		6.过度耦合(coupling)则可能导致让人不悦的冗长建置时间(build times)。
*
***** 条款26 尽可能延后变量定义式的出现时间
*		你不只应该延后变量的定义，直到非得使用该变量的前一刻开始，
*		甚至应该尝试延后这份定义直到能够给它初值实参为止。
*		更确切的说，以“具有明显意义之初值”将变量初始化，还可以附带说明变量的目的。
*
*		关于循环
*		方法一：定义于循环外
*		Widget w;
*		for(int i = 0; i < n; ++i)
*		{
*			//w = ...							(1 ctor) + (1 dtor) + (n copy assignment)
*		}
*
*		方法二：定义于循环内
*		for(int i = 0; i < n; ++i)
*		{
*			//Widget w = ...					(n copy ctor) + (n dtor)
*		}
*
*		应该使用方法二（定义于循环内）
*		除非	1.赋值成本比 构造和析构 成本低。
*				2.代码对效率高度敏感。
*
*	NB: 1.尽可能延后变量定义式的出现。这样可以增加程序的清晰度并改善程序效率。
*
*
***** 条款27 尽量少做转型动作
*	C++规则的设计目标之一是，保证“类型错误”绝不可能发生。
*	不幸的是，转型(cast)破坏了类型系统(type system)。
*
*	C++提供4种新式转型：
*	const_cast<T>(expression);
*	static_cast<T>(expression);
*	dynamic_cast<T>(expression);			//可能耗费重大运行成本
*	reinterpret_cast<T>(expression);		//实际动作取决于compiler，不可移植
*
*	忠告是：始终理智地使用新式转型！！！
*
*	任何一个类型转换往往真的令compiler编译出运行期执行的代码。
*	Base* pb = &d;
*	Derived* pd = &d;
*	这两个指针的值并不相同，原因是在run-time有个偏移量(offset)被施加在第一个指针上，
*	得到正确的Base*指针值。
*	此例表明，一个对象可能拥有多个地址。
*	！！！因此，你通常应该避免做出“对象在C++中如何布局”的假设！！！
*	！！！当然更不应该以此假设为基础执行任何转型动作！！！
*	对象的布局方式和地址计算方式随编译器不同而不同。
*
*
*	另一个需要注意的事情，
*	派生类函数试图调用基类的虚函数，应使用 域运算符::
*	class Base
*	{
*	public:
*		virtual void foo(){}
*	};
*
*	class Derived: public Base
*	{
*	public:
*		virtual void foo()
*		{
*			Base::foo();				正确做法，调用base函数，作用于*this上。
*
*			//错误做法
*			static_cast<Base>(*this).foo();
*			//得到一个“当前对象base class成分”的副本。
*			//如果函数修改对象，那么当前对象不变，被修改的是副本。
*
*		}
*	};
*
*
*	关于dynamic_cast					//RSY注：这里不打算深入探讨typeid和type_info
*	值得注意的是，dynamic_cast的许多实现版本执行速度相当慢。(例如多次strcmp)
*	参考文章: How expensive is RTTI.
*	https://stackoverflow.com/questions/579887/how-expensive-is-rtti
*
*	忠告：应该在注重效率的代码中对dynamic_cast保持机敏与猜疑。
*
*	之所以要使用dynamic_cast是因为，你认定要对一个Derived对象进行操作，(要调用derived函数)
*	但是手上却只有一个Base*指针。
*	如果条件允许，设计之初就应该为每个派生类设计自己的容器。而非全部塞到base容器。
*	如果只提供base容器，可以试着这样做：
*	if(Derived* pd = dynamic_cast<Derived*>(it->get()))			//若转型失败，返回nullptr
*	{......}
*	另一种做法是，在base内提供该函数，并实现default，
*	这样就是如下：
*	it->foo();				//所有类都有foo()函数，但有些是空实现。
*
*
*	最后，绝对要避免的一件事就是“连串dynamic_cast”
*	可以使用type-traits技法，在编译期完成工作。参考Item 47
*
*	我们应该尽可能隔离转型动作，通常是把它隐藏在某个函数内，
*	函数的接口会保护调用者不受函数内部任何肮脏龌龊的动作影响。
*
*	NB: 1.尽量避免转型特别是在注重效率的代码中避免使用dynamic_cast，
*		  如果有个设计需要转型动作，试着发展无需转型的替代设计。
*	！！2.如果转型是必要的，试着将它隐藏在某个函数后面。客户随后可以调用该函数，
*		  而不需要将转型放进自己的代码中。
*		3.始终使用C++新式转型。
*
*
***** 条款28 避免返回 handles 指向对象内部成分
*		考虑一个常成员函数，返回一个指针数据成员，多么糟糕。这立刻带给我们两个教训：
*			1.成员变量的封装性最多只等于“返回其reference”的函数的访问级别。
*			2.如果一个const函数传出一个reference，函数调用者有可能更改那比数据，
*			  这正是bitwise constness的一个附带结果。
*
*		handles就是reference/pointer/iterator，
*		而返回一个handle，随之而来的就是“降低对象封装性”的风险。
*
*		一个简单的解决办法是：返回值加上const
*		const T& foo() const{}
*
*		另一个问题是：dangling handles
*		！！！这里唯一关键是：有个handle被传出去了，一旦如此你就暴露在
*		“handle比其所指对象更长寿”的风险下。！！！
*		总之，返回一个handle代表对象内部成分总是危险的。
*		当然，并不意味禁止，比如operator[]就是返回reference指向内部数据。
*
*	NB: 1.避免返回handles(reference/pointer/iterator)指向对象内部。
*		  这样可以增加封装性。帮助const函数(bitwise constness)表现得更像logic constness
*		  并将dangling handles发生的可能性降至最低。
*
***** 条款29 为“异常安全”而努力是值得的
*	RSY注：关于异常规范，C++11引入了noexcept而摒弃了throw
*	最近比较忙，就按本书上的先来吧。
*
*	异常安全性有几分像是...呃...怀孕。									ex.
*	异常安全性有2个条件：
*		1.不泄露任何资源
*		2.不允许数据败坏
*
*	关于资源泄露的解决方案可以参考Item 13 以对象管理资源(RAII手法)
*
*	本节专注于解决数据败坏问题。
*	异常安全函数提供一下三个保证之一：
*		1.基本承诺
*				如果异常被抛出，程序内的任何事物仍然保持在有效状态下。
*					注：但是程序的现实状态(exact state)不可预料。
*		2.强烈保证
*				如果异常被抛出，程序状态不变。
*					注：如果函数成功，就是完全成功。
*						如果函数失败，程序就会回复到“调用函数之前”的状态。
*		3.不抛掷保证
*				承诺绝不抛出异常。					//throw()
*					注：比如对内置类型的操作。
*		正确性、可移植性、高效性、异常安全性  都是由函数的实现决定，无关乎声明。
*
*		为每个函数提供异常安全性保证。
*		为此，我们的抉择是，该提供哪一种异常安全性保证？
*		对于大部分函数，选择都落在 基本保证 和 强烈保证 之中。
*
*		ex.		在变更资源时，有一种方法是：使用 智能指针 及其 reset()函数。
*				reset(...)只有在参数中资源成功生成时才会调用，而delete在reset内部使用。
*
*				copy and swap	！！！
*		ex.		另外有一个一般化的策略很典型地导致强烈保证。被称为 copy and swap
*				原则很简单：
*					1.为你打算修改的对象做出一份副本。
*					2.然后在副本上做出必要的修改。
*					3.i)	若抛出异常，原对象仍保持原来状态不变。
*					  ii)	若无异常抛出，即副本修改成功，
*							将原对象和副本进行一个不抛出异常的交换(swap)。
*				注：这里使用了pImpl手法。
*
*		即使我们可以让一个函数局部提供强烈保证，也不能保证整个函数具有强烈保证。
*			void doSomething()
*			{
*				...				//copy
*				f1();
*				f2();
*				...				//swap;
*			}
*		考虑f1()，f2()，即使它们提供了强烈保证，
*		如果f1()，f2()抛出异常，那么就要回溯到f1()被调用之前的状态。这很难完成。
*
*		问题出在“连带影响”(side effects)。
*		如果函数只操作局部性状态(local state)，便相对容易提供强烈保证。
*		但是当函数对 非局部性数据(non-local data)有连带影响时，提供强烈保证就困难得多。
*
*		还有一个问题是效率，copy-and-swap的关键在于“修改对象数据的副本，
*		然后在一个不抛出异常的swap函数中将修改后的副本与原件置换”，
*		因此这可能消耗大量的时间和空间。
*
*
*		当强烈保证不切实际时，就必须提供基本保证！！！
*		现实中也许可以为某些函数提供强烈保证，但效率和复杂度令人退却。
*
*		一个软件系统要不就具备异常安全性，要不就全然否定。
*
*		总结：
*			1.首先是以对象管理资源Item 13(RAII)。
*			2.然后是选择一个异常安全保证实施于每一个函数上。并将你的决定写成文档。
*
*		时间不短前进，我们与时俱进!
*
*	NB: 1.异常安全函数即使发生异常也不会泄露资源或数据败坏。
*		  有三种异常安全保证：基本保证、强烈保证、不抛异常保证。
*		2.强烈保证往往能够以copy-and-swap实现，但强烈保证并非对所有函数都可实现或有意义。
*		3.函数提供的“异常安全保证”通常不大于其所调用各个函数的
*		  “”异常安全保证中的最弱者。
*
*
***** 条款30 透彻了解inlining的里里外外
*		好处：
*			compiler optimization通常被设计用来浓缩那些“不含函数调用”的代码，
*			所以对于inline函数，或许compiler因此有能力对函数体进行优化。
*		弊端：
*			增加目标码(object code)大小。
*			函数指针。
*			重新编译。
*			无法调试。
*
*		inline只是对compiler的一个申请，并不是强制命令。
*		隐喻方式是将成员函数定义于class定义式内。
*
*		inline函数通常一定被置于头文件内，因为大多数建置环境(build environment)
*		在编译过程中进行inline。(极少环境会在link time或run time进行inline)
*		inlining在大多C++程序中是编译期行为。
*		注：template的具现化与inlining无关。如果你正在写一个template并认为
*			所有生成的函数都应该inline，那么将这个template声明为inine。(例如std::max)
*			否则应避免将template声明为inline。
*
*		另外，对于virtual函数的调用也会导致无法inline(因为virtual意味着等待到运行期)
*
*		综上所述：depends on compiler
*		幸运的是，大多数编译器提供了诊断信息。如果无法inline，会提出警告。
*
*		关于函数指针：
*			如果取某个inline函数的地址，那么compiler通常为此函数生成一个outline函数本体。
*			这意味着对于inline函数的调用有可能inline，也有可能不inline。
*			例如：
*				inline void f(){...}
*				void (*pf)() = f;
*				f();						line调用
*				pf();						或许不被inline
*
*		此外，compiler有时自动生成ctor和dtor的outline版本，然后获取指针。
*
*		！！！实际上ctor和dtor往往是inline函数的糟糕候选者！！！
*
*		C++对“对象被创建和被销毁是发生了什么事”做了各式各样的保证。
*		“事情如何发生”是compiler在编译期代为产生并插入到程序中的代码，
*		有时就放在ctor和dtor中。
*
*		最后，inline函数无法随着程序库的升级而升级。
*		一旦inline函数被改变，所有客户都必须重新编译！
*		然而对于non-inline函数，只需要重新连接，如果采取动态链接，几乎没有消耗。
*
*		还有一点：大部分调试器对inline函数束手无策。
*		无法设置断点进行调试。
*
*		总结：
*			一开始不要将任何函数声明为inline，或只把inline施加于
*			那些“一定成为inline”或“十分平淡无奇”的函数上。
*			慎重使用inline对日后使用调试器带来帮助。
*			记住80-20法则，找出可以增加程序效率的20%代码然后对其进行评估。
*
*	NB: 1.将大多数inline限定在小型、被频繁调用的函数身上。
*		2.不要只因为function template出现在头文件，就将它们声明为inline。
*		3.ctor和dtor往往是inline函数糟糕的候选者。
*
*
***** 条款31 将文件间的编译依存关系降至最低
*	借此机会深入了解一下 compilation and linking ！！！！
*	当时看书的时候还算顺畅，现在查资料+写这一个条款花了2个多小时。。。
*	https://www.cprogramming.com/compilingandlinking.html
*	http://www.cplusplus.com/articles/2v07M4Gy/
*	https://stackoverflow.com/questions/6264249/how-does-the-compilation-linking-process-work
*	https://stackoverflow.com/questions/188449/what-are-some-techniques-for-limiting-compilation-dependencies-in-c-projects
*	http://gernotklingler.com/blog/care-include-dependencies-cpp-keep-minimum/
*	https://cs.nyu.edu/courses/fall06/G22.1133-001/code-and-notes-from-class/sep-comp.html
*	http://www.cplusplus.com/articles/Gw6AC542/			<-提供了一些思路
*	https://msdn.microsoft.com/en-us/library/aa243604(v=vs.60).aspx
*
*	RSY按：
*	本条款并不完全正确，意味着不要拘泥于此，可以参考上述资料加以判别！！！！！！！！！！！
*
*	假设你对某个class进行了轻微修改，结果导致整个项目都重新编译和链接了。
*
*	问题出在C++并没有把“将接口从实现中分离”这事做得很好。
*	class的定义式不只详细叙述了class接口，还包括数据成员(实现细目)。
*
*	class被定义的文件中，必须包含该class成员被定义的文件！！！！		//#include "...h"
*	这么一来便是形成了编译依存关系(compilation dependency)
*	如果这些头文件中有一个被修改，或这些头文件所依赖的其他头文件有任何改变，
*	那么所有使用这class的文件都需要重新编译。
*
*	注：标准头文件不太可能成为编译瓶颈，特别是使用预编译头文件。
*
*	解决手法：pImpl idiom
*		原因：compiler必须在编译期知道对象的大小。
*		这样我们只需要把一个class分成两个：一个只提供接口，另一个负责实现该接口。
*
*	接口与实现分离！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
*	这个分离的关键在于以“声明的依存性”替换“定义的依存性”，
*	这正是编译依存性最小化的本质：让头文件尽可能自我满足，万一做不到，
*	则让它与其他文件内的声明式(而非定义式)相依。
*
*	这个设计策略有一下3点：！！！
*
*		1.如果使用obj pointer/reference就可以完成任务，就不要用obj。
*			使用前向声明forward declaration
*
*		2.尽量用class声明式替换class定义式。
*			注:即使函数参数pass-by-value或者return value，也仅需要class声明。
*			不过在调用该函数之前，该class定义式必须要曝光。
*			这样做的意义在于：并非所有人都使用这个函数！！！
*
*			假设函数库中有多个函数声明，而并非所有客户都调用所有函数。
*			如果能够将提供class定义式(通过#include)的义务从
*			“函数声明所在”之头文件 转移 到“内含函数调用”之客户文件，
*			便可将“并非真正必要之类型定义”与客户之间的编译依存性除掉。
*
*		3.为声明式和定义式提供不同的头文件。
*			为了严守上述准则，必须提供两个文件，一个声明，一个定义。
*			当然，这些文件必须保持一致性，如果有个声明式被改变，两个文件都得改变。
*
*			因此客户总是应该#include一个声明文件，而非前置声明若干函数，
*			程序库作者也必须提供这两个头文件。
*
*			<iosfwd>将只含声明式的头文件提供给template，而template定义式放在非头文件中。
*
*		总结实现方式：
*	ex.		1.pImpl idiom (handle class)
*				这样的class往往被称为handle class，
*				这样的class如何真正做点事情：
*					将全部函数转交给相应的实现类(Implementation class)，
*					并由后者完成实际工作，handle class调用相同函数即可。
*				注：handle class 与 Implementation class 接口完全相同！！
*				handle class 调用 Implementation class 相应接口。
*
*				让一个class变为handle class并不会改变它做的事，
*				只会改变它做事的方法。
*
*	ex.		2.Interface class
*				令一个class成为abstract base class，称为Interface class
*
*				这种class的目的是详细一一描述derived class的接口，
*				因此通常不带成员变量，也没有构造函数，
*				只有一个virtual析构函数，和一组pure virtual函数，用来叙述整个接口。
*
*				Interface class的客户以指针来撰写程序。
*
*				！！！除非interface class的接口被修改，否则客户不需要重新编译！！！
*
*				Interface class只提供接口，并提供一个static factory函数
*				工厂函数根据参数创建不同类型的派生类对象，
*				并生成interface的智能指针
*				static std::tr1::shared_ptr<BaseInterface> create(Args...)
*				{
*					//according to Args
*					return std::tr1::shared_ptr<BaseInterface>
*						(new DerivedImplementation(Args...));
*				}
*
*		总结：
*			Handle class 和 Interface class 解除了接口和实现之间的耦合关系，
*			从而降低了文件间的编译依存性(compilation dependency)。
*			代价是：它使你在运行期丧失若干速度，又让你为每个对象付出更多内存。
*				对于Handle class，
*					成员函数必须通过pImpl操作，增加了一层访问。
*					内存数量增加了一个智能指针的大小。
*					以及动态内存分配的开销和异常风险。
*				对于Interface class，
*					virtual函数的成本。
*			另外Handle class 和 Interface class隐藏了实现细节，这样就脱离了inline。
*			RSY注：可以将想要inline的函数写在另一个.hpp文件中，还避免了循环编译相依的问题。
*
*			然而如果只因为若干额外成本便不考虑Handle class 和 Interface class
*			将是严重的错误！！！
*			应该考虑以渐进的方式使用这些技术。
*			在程序发展过程中使用Handle class 和 Interface class以求实现代码有所变化时
*			对客户带来最小冲击。
*			而当它们导致速度或大小差异过大以至于class之间的耦合相形之下不成关键时，
*			就以具象类(concrete class)替换Handle class 和 Interface class。
*
*	NB: 1.支持“编译依存性最小化”的一般构想是：
*		  相依与声明式，不要相依与定义式！！！！！
*		  基于此构想的两个手段是Handle class 和 Interface class。
*	***	2.程序库头文件应该以“完全且仅有声明式”(full and declaration-only forms)
*		  的形式存在。这种做法不论是否涉及templates都适用
*
*
*******************************  Chapter 6 继承与面向对象设计  *******************************
*	继承的选项：单一/多、public/protected/private、virtual/non-virtual
*	成员函数和其他特性的交互影响。
*	缺省参数与virtual函数的影响。
*	继承如何影响C++名称查找规则。
*	如果class的行为需要修改，那么virtual函数是最佳选择么？
*	public继承/private继承代表什么？
*	virtual函数意味“接口必须被继承”，non-virtual函数意味“接口和实现都必须被继承”。
*
***** 条款32 确定你的public继承塑模出is-a关系
*	public inheritence 意味 is-a 的关系！！！
*
*	class Derived: public Base {};
*	意味着：每一个Derived对象 同时也是一个Base对象，反之不成立。
*
*	此外，要注意，我们有可能成为不严谨语言的牺牲品！！！
*	设计class时，应谨慎分析其关系，而非由语言常识来建立关系。
*	在处理由语言混淆带来的问题时，解决办法有：		//比如D没有B的某些特性。(企鹅和鸟)
*		1.按真实意思设计。
*		2.产生 Run-time Error。
*		3.只对于non-virtual函数，可以采用C++11关键字 delete(本条可忽略，因为一般都是virtual)
*
*		注意，好的接口可以防止无效代码通过编译。
*
*		is-a 并非唯一的class之间关系，
*		还有 Composition：
*				has-a(有一个) 和 is-implemented-in-terms-of(根据某物实现出)
*
*	NB: 1.public继承意味is-a。适用于base class上的每一件事情
*
*
***** 条款33 避免遮掩继承而来的名称
*	derived class作用域被嵌套在base class内。
*	比如：
*		void Derived::foo()
*		{
*			f();
*		}
*		compiler必须知道这里的f() refer to 什么东西，
*		compiler的做法是查找各个作用域。
*		首先是local作用域，
*		然后是外围作用域，即Derived class覆盖的作用域，
*		最后是base class作用域，
*		最后是global作用域。
*
*		对于base class和derived class中的成员函数，
*		！！！只要函数名相同就会 hide (即使参数类型不同，而且不论是否virtual)！！！
*
*		原因：基本理由是防止你在程序库或应用框架中
*			  建立新的Derived class时附带地从疏远的Base class继承重载函数。
*			  不幸的是，你通常想要继承重载函数。实际上如果你正在使用public继承
*			  而又不继承那些重载函数，就是违反is-a关系。
*		因此你几乎总是想推翻继承而来的hide行为。
*
*		办法是：使用using声明式。
*
*		再看一种情况：
*		有时你不想继承base的所有函数，这是可以理解的。
*		然而这在public继承下是不可能的，(using声明式会让所有同名函数可见，而我们只要某些)
*		但是在private继承下却可能有意义。
*		可以采用转交函数(forwarding function)方法：
*			class Derived: private Base
*			{
*			public:
*				virtual void foo()				//转交函数(forwarding function)
*				{
*					Base::foo();
*				}
*			};
*
*	对于template，我们又将面对“继承名称被遮掩”的一个全然不同的形式。(有可能类被特化)
*
*	NB: 1.derived class内的名称会 遮掩(hide)base class内的名称。
*		  在public继承下从来没有人希望如此。
*		2.为了让被遮掩的名称重见天日，可以使用
*			i) using声明式，所有同名函数均可见
*			ii)转交函数(forwarding function)——private继承
*
***** 条款34 区分接口继承和实现继承
*
*	！！！！！！
*					1.pure virtual函数——只继承接口
*					2.virtual函数——继承接口和缺省实现
*					3.non-virtual——继承接口和强制性实现
*	！！！！！！
*
*	注：pure virtual函数可以有实现，另外，pure virtual dtor必须有实现。
*
*	------------------------------------------------------
*	考虑一个问题：
*		base class中有一个virtual函数，并含有一份缺省实现，用来在derived中调用。
*		后期加入新派生类，忘记override这个virtual函数，有可能出现非预期结果。
*	解决方案1：
*		以不同的函数提供接口和实现。
*		将virtual变为pure virtual函数，并把缺省实现写为另一个函数里，
*		该函数声明为protected。像这样：
*		class Base
*		{
*		public:
*			virtual void foo() = 0;
*		protected:
*			void defaultFoo(){...}					//用于derived调用
*		};
*		这意味着：
*			“提供实现给derived class，但除非它们明白要求否则免谈。”	(要求 即remember覆盖)
*			此伎俩在于切断“virtual函数接口”和其“缺省实现”之间的连接。
*
*	解决方案2：
*		将virtual函数声明为pure virtual函数并提供缺省实现。
*		原理是：
*			pure virtual函数必须在derived class中重新声明，但它们也可以拥有自己的实现。
*		class Base
*		{
*		public:
*			virtual void foo() = 0;
*		};
*		void Base::foo(){...}
*		这意味着：
*			该函数被分割成为2个要素：
*				1.其声明表现接口(要去derived class必须使用)
*				2.其定义表现缺省行为(derived class可能使用，但要明确提出申请 Base::foo();)
*	------------------------------------------------------
*
*	关于non-virtual函数，——继承接口和强制性实现
*		意味着它并不打算在derived class中有不同的行为。
*		实际上一个non-virtual函数所表现的不变性(invariant)凌驾其特异性(specialization)。
*		任何derived class都不应该改变其行为。
*		non-virtual函数绝不该在derived class中被重新定义！！！！！
*
*	综上，当你声明成员函数时，必须谨慎选择(根据 实现的继承)
*	下面讨论2个常见的设计错误：
*		错误1：将所有函数声明为non-virtual，这使得derived class没有空间进行特化工作。
*			   另外，non-virtual dtor在继承体系中带来麻烦。(Item 7)
*			   还有，不要过度设计，不要过分担心virtual函数带来的成本！！！(80-20法则)
*		错误2：将所有函数声明为virtual。
*			   某些函数就是不该在derived class中被重新定义，
*			   果真如此你就应该把那些函数声明为non-virtual。
*			   如果你的不变性(invariant)凌驾其特异性(specialization)，不要害怕表现出来。
*
*	NB: 1.接口继承与实现继承不同。public继承之下，接口总被继承。
*		2.pure virtual函数——只继承 接口。
*		3.virtual函数——继承 接口和缺省实现。
*		4.non-virtual——继承 接口和强制性实现。
*
*
***** 条款35 考虑virtual函数以外的其他选择
*
*	普通虚函数的其它替代设计方案。——很多方案使得同类对象拥有不同的操作函数。
*
*	1.藉由NVI(non-virtual-interface)手法实现Template Method模式
*		该方法主张virtual函数几乎总是成为private。
*		较好的设计是提供public non-virtual函数，并在其中调用private(protected) virtual函数。
*		NVI手法的一个优点是：
*			可以在virtual函数调用之前 设置好场景。(lock，log，约束条件等)
*			并在调用结束之后 清理场景。(unlock，验证约束条件等)
*		实际做法是：将所有virtual函数声明为private，
*					并在base class内提供一个public non-virtual接口，调用virtual函数。
*		注：重新定义virtual函数表示某些事“如何”被完成，
*			调用virtual函数表示“何时”被完成。
*
*		想起来一个东西：关于virtual函数的访问权限。
*		https://stackoverflow.com/questions/5685514/virtual-function-breaking-private-access
*
*	2.藉由Function Pointer实现Strategy模式
*		类中加一个成员：函数指针
*		这个做法提供了某些有趣的弹性：
*			1.同一类对象可以拥有不同的函数指针(操作符)。
*			2.操作符可以在运行期变更(setFunction)。
*
*		注：有时操作符(函数)要声明为friend(因为可能修改内部数据)，降低了封装性。
*			或者为它提供public访问函数。
*
*		使用Function Pointer替换virtual函数，
*			优点：
*				1.每个对象可以拥有自己的函数指针。
*				2.可在运行期改变操作符。
*			缺点：
*				可能降低封装性。
*
*	3.藉由tr1::function实现Strategy模式
*
*		tr1::function对象可持有任何 可调用物(callable entity)
*			——函数指针、成员函数、函数对象、lambda表达式。
*		用法：															ex.
*		class A{
*		public:
*			typedef std::tr1::function<return_type (parameter list)> MyFunc;
*		private:
*			MyFunc myFunc;
*		};
*
*			关于使用成员函数作为函数符：
*			http://zh.cppreference.com/w/cpp/utility/functional/bind
*			A a;							//A有成员函数foo()
*			B b(							//B构造函数，初始化函数符
*				std::bind(					//有趣的std::bind
*					&A::foo,					//成员函数
*					a,							//作为this指针
*					_1							//placeholder
*				)
*			);
*
*	4.传统的Strategy模式												ex.
*		将 virtual成员函数 分离出继承体系，并作为一个base class，
*		这个base class中提供一个public virtual函数，并将该对象作为参数，用来做之前的工作。
*		而原来对象多一个这个base class指针，通过这个pointer来调用工作函数。
*
*		这个解法的吸引力在于：
*			熟悉Strategy模式的人很容易辨识，
*			此外，还可以增加工作函数，只需要添加一个derived class即可。
*
*
*	总结：
*		1.	NVI是Template Method的一种特殊形式。
*			它以public non-virtual函数包裹较低访问性(private/protected)的virtual函数。
*			RSY注：话说那些人居然极力主张将virtual声明为private。。。。。。
*			https://isocpp.org/wiki/faq/strange-inheritance#private-virtuals
*
*		2.	以 函数指针 替换 virtual函数，提供了较大的弹性。
*
*		3.	以 tr1::function成员变量 替换 virtual函数，同上。
*
*		4.	将 继承体系中内的virtual函数 替换为 另一个继承体系内的virtual函数。
*			这是传统Strategy模式手法。
*
*	NB: 1.virtual函数的替代方案包括：NVI手法 和 Strategy模式的多种形式。
*		2.将工作从 成员函数 转移到 class外部函数，带来的一个缺点是：
*		  无法访问non-public成员，需要另外提供接口或声明为friend。
*		3.tr1::function的行为就像一般的函数指针。
*		  该对象可以接纳“与给定目标签名式兼容”的可调用物(callable entity)。
*
*
***** 条款36 绝不重新定义继承而来的non-virtual函数
*	non-virtual函数是静态绑定(statically bound)
*	virtual函数却是动态绑定(dynamically bound)
*
*	public继承意味着is-a关系，适用于Base对象的每件事，也同样适用于Derived对象。
*
*	！！！总之，任何情况下都 不要重新定义 一个继承而来的 non-virtual函数！！！
*
*
*	NB: 1.绝对不要重新定义继承而来的non-virtual函数。
*
*
***** 条款37 绝不重新定义继承而来的缺省参数值
*	继承一个non-virtual函数是绝对错误的。
*	本条款讨论“带有一个缺省参数值的virtual函数”。
*
*	对象的所谓静态类型，就是它在程序中 被声明时所采取的类型。
*	！！！				静态类型——声明			！！！
*
*	对象的所谓动态类型则是指“目前所指对象的类型”。
*	！！！				动态类型——所指对象的类型	！！！
*
*	然而，令人遗憾的是：												ex.
*		virtual函数系动态绑定，而缺省参数值却是静态绑定！！！
*
*	准确地讲：对于virtual函数来说：函数声明——静态绑定；函数体——动态绑定。
*
*		意思就是：你可能会在“调用一个定义于derived的virtual函数”的同时，
*				  却使用base class为它指定的缺省参数值。
*
*	C++ compiler如此做的原因就是：运行期效率。
*	为了获得在compile time就做出决定的效率。其结果就是现在你所享受的效率。
*
*
*	解决办法：
*		使用NVI手法。												ex.
*
*	NB: 1.绝对不要重新定义一个继承而来的缺省参数值。
*		  因为缺省参数值是静态绑定。而virtual函数体——你唯一应该override——却是动态绑定。
*
*
***** 条款38 通过复合塑模出has-a或“根据某物实现出”
*	复合(Composition)意味着：has-a 或 is-implemented-in-terms-of
*
*	！！区分has-a 和 is-implemented-in-terms-of
*		真实存在——应用域——(has-a)
*		辅助实现——实现域——(is-implemented-in-terms-of)
*
*	！！区分is-a 和 is-implemented-in-terms-of
*		关键在于：
*			如果D是一种B，那么对B为真的每一件事对D也都应该为真！！！
*
*	NB: 1.复合(Composition)的意义 和 public继承 完全不同。
*		2.复合意味着：	在应用域 has-a
*						在实现域 is-implemented-in-terms-of
*
*
***** 条款39 明智而审慎地使用private继承
*		private继承的2个特点：
*			1.compiler不会将 derived对象 转换为 base对象。
*			2.继承而来的base成员，全部是private。
*
*	本质含义：
*		private继承意味着 is-implemented-in-terms-of(根据某物实现出)。
*		你的用意是采用base class内已经备妥的特性，
*			而不是因为 base对象 和 derived对象 有任何观念上的关系。
*		private继承纯粹只是一种实现技术。
*
*		！！！private继承意味着：只有实现部分被继承，接口部分略去！！！
*		private继承在软件设计层面没有意义。只有在实现层面有意义。
*
*
*		private继承意味着 is-implemented-in-terms-of(根据某物实现出)。
*		而复合(Composition)的意义也是如此，如何在两者之间取舍？
*		答案是：
*			尽可能使用复合(composition)，必要时才使用private继承。
*			注：“必要时”指含有protected成员或virtual函数时。
*
*		情景：C需要reuse B的一些实现。
*		！！！选择 public继承 + 复合， 而非 private继承！！！				ex.
*		原因：
*			1.不想让C的派生类重新定义virtual函数。
*			2.降低C的编译依存性。(因为继承要求定义必须可见)
*			注：可以使用指针和forward declaration
*
*		何时使用private继承：
*			C①需要访问B的protected成员，或者②需要重新定义virtual函数。
*
*			但这时候2个class的关系是 is-implemented-in-terms-of 而非 is-a。
*
*
*
*
*
*			另一个private继承用途是：空间优化。
*			EBO(empty base optimization)
*			任何“独立(非附属)”对象大小一定不为0。
*			如果 private 单一 继承 一个empty class，那么不增加大小。
*
*			empty class不含non-static变量。
*			通常含有：typedef(using)、enum、static、non-virtual函数。
*
*
*		对比：
*				 ______							______
*				 |  B |							|  B |
*				 |____|							|____|
*		 		   ↑							  ↑
*				   |  private继承				  |	 public继承
*				   |							  |
*				___|___ 					   ___|___		  ______
*				|  C  |						   |  D  |◆------|  C |
*				|_____|						   |_____|		  |____|
*
*		is-implemented-in-terms-of			选择 public继承 + 复合
*												容易理解、降低编译依存度
*
*	NB: 1.private继承意味着 is-implemented-in-terms-of(根据某物实现出)。
*		  private继承通常比复合(composition)的级别低。
*		  但derived需要访问protected成员或需要重新定义virtual函数时，private继承是合理的
*		2.和复合(composition)不同， private继承可以用于empty base最优化。
*
*
***** 条款40 明智而审慎地使用多重继承
*		多重继承multiple inheritance; MI
*
*		首先，MI可能引起歧义。
*			若继承了2个base，且它们都有一个同名接口。
*			那么需要指定调用哪个接口。
*
*		更糟糕的是：
*			“钻石型”多重继承，导致base成员重复。
*		解决办法是：virtual base class				//C++标准库中的例子：
*		即：
*			class Base {};										//basic_ios
*			class D1: virtual public Base {};					//basci_istream
*			class D2: virtual public Base {};					//basic_ostream
*			class Derived: public D1, public D2 {};				//basic_iostream
*
*		从正确行为的角度看：public继承总应该是virtual，
*							但是你得为virtual继承付出代价。
*
*		此外，virtual base的初始化责任是由继承体系的最底层(most derived)负责！！	ex.
*
*		忠告是：
*			1.非必要不要使用virtual base
*			2.如果使用，尽可能 避免在virtual base中放置数据！！
*
*
*		最后，一个多重继承(MI)的用途是：											ex.
*			一个抽象基类B，一个已经有的类C，
*			现在要写D：
*				class D: public B, private C {};
*			根据C的实现来实现B的接口。
*
*		用途：public继承一个interface，并private继承一个协助实现的class。
*
*	NB: 1.MI比单一继承复杂，可能导致歧义，以及对virtual继承的需要。
*		2.virtual继承会增加大小、速度、初始化(和赋值)等成本。最好virtual base不含任何数据。
*		3.MI的确有正当用途。比如：
*		  “public继承某个interface”和“private继承某个协助实现class”。
*
*
*******************************  Chapter 7 模板与泛型编程  *******************************
*	C++ template 机制自身是一部完整的图灵机：它可被用来计算任何可计算的值。
*	于是这导出了 模板元编程(template metaprogramming)，
*	创造出“在compiler内执行并于编译完成时停止执行”的程序。
*
***** 条款41 了解隐式接口和编译期多态
*	OOP总是以显式接口和运行期多态解决问题。
*	对于泛型编程：隐式接口和编译期多态。
*
*		显式接口：由 函数签名式 构成。(也包含typedef)
*
*		隐式接口：由 有效表达式 构成
*
*	NB: 1.class 和 template 都支持interface和polymorphism。
*
*		2.对class而言接口是显式的，以函数签名为中心。多态通过virtual函数发生在运行期。
*
*		3.对template而言，接口是隐式的，奠基于有效表达式。
*		  多态则是通过  template具现化  和  函数重载解析(function overloading resolusion)
*		  发生于编译期。
*
***** 条款42 了解typename的双重含义
*	当声明template参数时，class和typename完全相同。
*
*	template内出现的名称如果相依于template参数，称之为 从属名称(dependent names)。
*	如果从属名称在class内呈嵌套状，我们称它为嵌套从属名称(nested dependent name)。
*		比如：C::const_iterator
*
*	嵌套从属名称有可能导致解析困难。
*	在我们知道C是什么之前，没有任何办法可以知道C::const_iterator是否为一个类型。
*
*	C++有个规则可以解析(resolve)这个歧义状态：
*		如果解析器在template中遭遇一个嵌套从属名称，它便假设这名称不是个类型，
*		除非你告诉它是。(有例外)
*
*			比如： typename C::const_iterator
*
*	例子：dev-C++(gcc 4.9.2)会报出解析错误，不过VS2015没有。。。
*	https://www.zhihu.com/question/264704399
*
*	typename只被用来验证嵌套从属名称。
*
*	例外：typename不可以出现在 继承列表 和 初始化列表 中。
*
*
*	NB: 1.声明template参数时，class与typename可以互换。
*		2.请使用关键字typename标识 嵌套从属类型名称；但不能在继承列表和初始化列表中出现。
*
*
***** 条款43 学习处理模板化基类内的名称
*	ex.
*		template<typename T>
*		class B							//有可能被特化
*		{
*		public:
*			void boo() {}
*		};
*
*
*		template<typename T>
*		class D: public B<T>
*		{
*		public:
*			void doo()
*			{
*				boo();						//compile error ！！！
*			}
*		};
*	当compiler遭遇 class template D时，并不知道它继承什么样的class。
*	当然继承了B<T>，但是直到T被确定，compiler才知道B<T>有什么函数。
*	而B有可能被特化：
*		template<>
*		class B<int> {};
*	而这个特化版本没有boo()函数。
*
*	这就是为什么C++拒绝调用的原因：
*		因为它直到base class template有可能被特化，
*		而那个 特化版本 可能提供 不一样的接口！！！
*	因此compiler往往拒绝在template base class(模板化基类)内寻找继承而来的名称。
*
*	解决办法：																ex.
*		1.加上	this->									this->boo();
*		2.使用using声明式。								using B<T>::boo;
*不提倡	3.指出函数位于base内。(糟糕的做法，关闭了多态)	B<T>::boo();
*
*		上述做法都是：
*			对compiler承诺base class template任何版本都提供一般版本提供的接口。
*
*	NB: 1.可在derived class template内通过 this-> 或 using函数声明 指涉base内的成员。
*
*
***** 条款44 将于参数无关的代码抽离template
*
*		使用template几乎总是导致代码膨胀！！！！
*		共性与变性分析(commonality and variability analysis)
*		必须自己去感受当template被具现化多次时可能发生的重复。
*
*		本条款只讨论  非类型模板参数带来的膨胀。								ex.
*
*		所有指针类型几乎总是有着相同的二进制表述。
*		因此凡template持有指针者，往往应该对每一个成员函数使用唯一一份底层实现。
*		这意味着：！！！！！！
*			如果你实现某些函数，而它们操作强指针类型(T*)，
*			那么你应该令它们调用另一个操作无类型指针(void*)的函数，
*			由后者来完成实际工作。
*
*
*	NB: 1.template生成多个class和多个函数，所以任何template代码
*		  都不该与 某个造成膨胀的 template参数产生相依关系。
*		2.因非类型模板参数(non-type parameter)造成的膨胀
*		  往往可消除，做法是以“函数参数”或“class成员变量”替换“template参数”。
*		3.因类型参数(type parameter)造成的代码膨胀，往往可降低，
*		  做法是让带有相同二进制表述的具现类型共享实现码。
*			即：使用调用相同的底层函数，来完成工作。
*
*
***** 条款45 运用成员函数模板接受所有兼容类型
*		考虑一个简单问题：
*			用B，D两个类型具现化模板，产生出来的两个具现体并不带有 继承关系！！！
*		解决方案：																ex.
*			为它写一个构造模板，使得任何类型间都可进行转换，
*			然后在 初始化列表中进行实际类型的转化，这样就有了“编译期 继承关系保证”。
*
*		注：声明一个“泛化copy函数”，并不阻止编译器生成“普通copy”，
*			所以还要自己声明普通copy。
*
*
*	NB: 1.请使用 函数成员模板(member function template)生成“可接受所有兼容类型”。
*		2.如果你声明member template用于“泛化copy”，还是需要声明“普通copy”。
*
*
***** 条款46 需要类型转换时请为模板定义非成员函数							ex.
*
*		模板实参推导： 实参推导只针对 函数模板 而非 类模板。
*			！！！template实参推导过程中从不将隐式类型转换函数纳入考虑！！！
*		解释：
*		这样的转换在函数调用中的确被使用，但在能够调用一个函数之前，
*		首先必须知道那个函数存在。
*		而为了知道它，必须先为相关的function template推导出参数类型。
*		然而template实参推导过程中并不考虑采纳“通过构造函数发生的”隐式类型转换。
*
*		！！class template并不依赖template实参推导.(后者只施行与function template身上)！！
*
*		只要利用一个事实，就可以缓和这个问题：
*			template class内的friend声明式可以指涉某个特定函数。
*		然而，如果将friend函数定义写在类模板外，则会出现连接错误！！！
*		解决办法：将friend函数定义于类模板内。
*
*		改进：
*			令friend调用辅助函数！！！
*
*
*		另外：
*			在类模板！内！，template参数可以忽略不写。
*			即：只有在类模板内，才可以省略<T>。
*
*			许多compiler会强迫你把所有template定义式放在头文件内。
*
*
*		总结：
*			对于非成员函数模板，如果想允许参数隐式类型转换(第一次调用)，
*			就将其声明为friend。
*			然后让friend调用辅助函数(非成员)。
*
*			1.我们虽然使用了friend，但是并未破坏封装性。
*			2.为了让类型转换发生于所有参数之上，我们需要一个non-member函数。
*			3.为了让这个函数逃避实参推导，将其声明为friend，并定义于class template内。
*			4.令friend函数调用辅助non-member函数。
*
*	NB: 1.当我们编写class template，而它所提供的“与此template相关的”函数支持
*		  “所有参数隐式类型转换”时，请将其定义为class template内部的friend函数，
*			并定义于class内，调用一个辅助non-member函数。
*
***** 条款47 请使用traits class表现类型信息
*		traits技法：允许你在编译期间获得某些类型信息。
*		traits是一种技术，也是C++程序员共同遵守的协议。
*		这个技术的要求之一是：它对内置类型和用户自定义类型的表现必须一样好。
*
*		traits总被实现为struct，但往往又被称为traits class。
*		traits的运作方式是：typedef定义某个type或category。
*
*		设计traits class：
*			1.确认若干你将来可取得的类型相关信息。
*			2.为该信息选择一个名称。
*			3.提供一个template和一组特化版本，内含你希望支持的类型相关信息。
*
*		编译期条件句：重载(overload)，——(功能类似if else)。				ex.
*
*		使用traits class：
*			1.建立一组重载函数，彼此间的差异只在于各自的traits参数。
*			2.建立一个控制函数，它调用上述那些实现函数，并传递traits class所提供的信息。
*
*	NB: 1.traits class使得“类型相关信息”在编译期可用。
*		  它们以template和template特化实现。
*		2.整合重载函数技术后，traits class有可能在编译期对类型执行if...else测试。
*
*
***** 条款48 认识template元编程
*	TMP(template metaprogramming)是执行与compile time的程序。
*	一旦TMP程序结束执行，其输出，也就是从template具现出来的C++源码，
*	便会一如既往地被编译。
*
*	使用TMP的好处：
*		较小的可执行文件，较短的运行期时间、较少的内存需求。
*	然而将工作从运行期转移到编译期的另一个结果是：编译时间变长了。
*
*	著名的比如Boost库中的MPL
*
*	NB: 1.TMP可将工作从运行期转移到编译期，因而得以实现早期错误侦测和更高的执行效率。
*		2.TMP可被用来生成“基于政策选择组合”的客户定制代码，
*		  也可用来避免生成对某些特殊类型并不合适的代码。
*
*
******************************* Chapter 8 定制 new 和 delete *******************************
*
*
*
***** 条款49
*
*
*
*
***** 条款50
*
*
*
*
***** 条款51
*
*
*
*
***** 条款52
*
*
*
*
******************************* Chapter 9 杂项讨论 *******************************
*
*
***** 条款53
*
*
*
*
***** 条款54
*
*
*
*
***** 条款55
*
*
*
*
*/
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <iterator>
#include <memory>
#include <functional>


//using namespace std;
//using namespace std::tr1;
using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::tr1::shared_ptr;


//ITEM 1

//ITEM 2
class Test2 {
private:
	static const int num2_1 = 5;//声明，如果为整形(int,char,bool)只要不取地址，就可以不定义
								//定义请放在实现文件而非头文件,这里是in class 初值设定
	static const double num2_2;	//声明，无法赋值
	int scores2_1[num2_1];		//使用常量num2_1
	enum { num2_3 = 5 };		//如果compiler不允许in class初值设定
								//可以采用所谓的 the enum hack
	int scores2_3[num2_3];		//枚举类型的数值可以权充int使用
};


//ITEM 3
class Test3 {
public:
	const char& operator[](std::size_t pos)const {		//operator[] for const obj
		return text3[pos];
	}
	char& operator[](std::size_t pos) {					//operator[] for non-const obj
		return text3[pos];
	}
	void foo(std::size_t x)const {			//bitwise constness
		*y = x;						//虽然没有改变对象的内容，但y所指改变
	}
private:
	std::string text3;
	int *y;
};

//mutable
class CTextBlock {		//ex.高速缓存文本区块的长度以便应付询问
public:
	std::size_t length()const;
private:
	char *pText;
	mutable std::size_t textLength;	//这些成员可能会被更改，即使在const函数内
	mutable bool lengthIsValid;
};
std::size_t CTextBlock::length()const {
	if (!lengthIsValid) {
		textLength = std::strlen(pText);	//mutable
		lengthIsValid = true;				//释放掉了const约束
	}
	return textLength;
}
//在const和non-const成员函数中避免重复
//考虑如下代码 !!!令人头痛的 额外代码、编译时间，和维护成本!!!
class TextBlock_ {	//以便区分，最后加上_
public:
	const char& operator[](std::size_t position) const {
		// do bounds checking 边界检测
		// log access data 日志数据访问
		// verify data integrity 检验数据完整性
		return text[position];
	}
	char& operator[](std::size_t position) {
		// do bounds checking 边界检测
		// log access data 日志数据访问
		// verify data integrity 检验数据完整性
		return text[position];
	}
private:
	std::string text;
};
//进行转型，改成如下：令non-const调用const,不要试图用non-const实现const!!!
class TextBlock {
public:
	const char& operator[](std::size_t position) const {   //same as before
		// do bounds checking 边界检测
		// log access data 日志数据访问
		// verify data integrity 检验数据完整性
		return text[position];
	}
	char operator[](std::size_t position) {					//现在调用const函数
		return
			const_cast<char&>(								//将返回值的const去除
				static_cast<const TextBlock&>(*this)		//为*this加上const，避免递归
				[position]									//调用const函数
		);
	}
private:
	std::string text;
};


//ITEM 4

//ITEM 5

//ITEM 6
	//将link time error转移至compile time是可能的
	//专门建立一个base class，将copy声明为private
class Uncopyable {
public:
	Uncopyable() {}
	~Uncopyable() {}
private:
	Uncopyable(const Uncopyable&);							//声明copy
	Uncopyable& operator=(const Uncopyable&);				//为private
};
class NoWay2Copy :private Uncopyable {

};

//C++ 11 default delete
//考虑输出流cout对象无法被拷贝，用delete禁止copy
class My_basic_ostream {
public:
	My_basic_ostream() = default;
	My_basic_ostream(const My_basic_ostream&) = delete;
	My_basic_ostream& operator=(const My_basic_ostream&) = delete;
	~My_basic_ostream() = default;
	void* operator new(std::size_t) = delete;			//禁止new
	void* operator new[](std::size_t) = delete;			//禁止new[]
};


//ITEM 7  在polymorphic base class(多态基类)中将destructor(析构函数)声明为virtual
class TimeKeeper {
public:
	TimeKeeper();
	virtual ~TimeKeeper();
private:
};
class AtomicClock :public TimeKeeper {};
class WaterClock :public TimeKeeper {};
class WristWatch :public TimeKeeper {};
TimeKeeper* getTimeKeeper();			//

//pure virtual function，，，，，纯虚析构函数，并提供实现（3种方式）
class AWOV {
public:
	virtual ~AWOV() = 0 {};		//pure virtual dtor，1.在这里实现
};
//AWOV::~AWOV() {}				//2.必须提供实现，或者在0后面加{}
//AWOV::~AWOV() = default;		//3.compiler 自动生成 default实现    推荐！！！

class Derived_AWOV :public AWOV {	//AWOV作为抽象基类
public:
	Derived_AWOV() {}
	~Derived_AWOV() {}				//compiler在这里调用~AWOV()
};


//ITEM 8
//重新设计接口，赋予用户一个机会得以处理因该操作而发生的异常
//并在dtor中提供双保险
class DBConn {
public:
	//......
	void close() {				//给用户提供的接口，用来处理异常
		//进行关闭处理
		closed = true;
	}
	~DBConn() {
		if (!closed) {					//双保险
			try {/*进行关闭处理*/ }
			catch (...) {
				//记录异常日志
				//......
			}
		}
	}
private:
	//......
	bool closed;				//judgement

};


//ITEM 9
//一种对于在ctor中不使用virtual函数的弥补措施
class Transaction {
public:
	explicit Transaction(const std::string& logInfo);

	void logTransaction(const std::string& logInfo) const {}   // now a non-virtual func

	//......
};

Transaction::Transaction(const std::string& logInfo)
{
	//......
	logTransaction(logInfo);                                 // now a non-virtual call
}

//令derived class将必要的构造信息向上传递至base class ctor
template<class T>
class BuyTransaction : public Transaction {
public:
	BuyTransaction(T _parameters)
		: Transaction(createLogString(_parameters)) {// pass log info to base class
		//...
	}
private:
	static std::string createLogString(T parameters) { return ""; }
};


//ITEM 10

//ITEM 11
//传统自我赋值安全处理
class Subsidiary_11 {};
class T11 {
public:
	T11& operator=(const T11& t11) {
		if (this == &t11)return *this;					//identity test
		delete s11;
		s11 = new Subsidiary_11(*(t11.s11));			//可能异常来源
														//1.内存分配不足 2.copy ctor抛出异常
		return *this;
	}
private:
	Subsidiary_11* s11;
};
//方案1		ex.
//一群精心编排的语句就可以得到异常安全（以及自我赋值安全）
//这里是简单的介绍，在条款29中有详细说明
class T11_1 {
public:
	T11_1& operator=(const T11_1& t11_1) {
		Subsidiary_11 *pS_origin = s11;						//记录原先内存地址
		s11 = new Subsidiary_11(*(t11_1.s11));				//令其指向一个副本
		delete pS_origin;									//删除原先内容
		return *this;
	}
private:
	Subsidiary_11* s11;
};
//方案2		ex.
//copy and swap，在条款29中有详细说明
class T11_2 {
public:
	void swap(T11_2& t11_2) {			//交换 *this 和 t11_2 的数据，详见条款29
		//......
	}
	T11_2& operator=(const T11_2& t11_2) {
		T11_2 temp(t11_2);				//制作一份副本
		swap(temp);						//将 *this 和上述副本 数据交换
		return *this;
	}
private:
	Subsidiary_11* s11;
};
//方案3		ex.
//copy assignment 以 by value 的方式接受实参。
//理由是：copying动作从 函数体内 移至 参数构造阶段 使得 compiler生成更高效的代码。
//这种做法是令人忧虑的，它为了伶俐巧妙的修补而牺牲了清晰性。
class T11_3 {
public:
	void swap(T11_3& t11_3) {				//交换 *this 和 t11_3 的数据，详见条款29
											//......
	}
	T11_3& operator=(T11_3 t11_3) {			//重要区别：pass by value，调用了copy ctor
		swap(t11_3);						//交换 *this 和 t11_3 的数据
		return *this;
	}
private:
	Subsidiary_11* s11;
};


//ITEM 12
//编写copy函数勿忘每一个成分
class Customer {
private:
	std::string name;
};
class PriorityCustomer :public Customer {
public:
	//.......
	PriorityCustomer(const PriorityCustomer& rhs)
		: Customer(rhs), priority(rhs.priority)	//调用base ctor（任何参数列表满足的）
	{											//这里一定会调用(或隐式)，否则compile error
		//日志记录
	}
	PriorityCustomer& operator=(const PriorityCustomer& rhs)
	{
		//日志记录
		Customer::operator=(rhs);				//对base class成分进行赋值动作
		priority = rhs.priority;
		return *this;
	}
private:
	int priority;
};


//ITEM 13		返还资源
class Investment {								//投资行为的root class
public:
	bool isTaxFree()const;
};
Investment* createInvestment() {				//返回指针，指向继承体系的动态分配对象，
	return new Investment();					//调用者有责任删除它
}

//RAII		
//如果dtor抛出异常，参见条款8
//使用unique_ptr
void f13_1() {
	std::unique_ptr<Investment> pInv(createInvestment());	//调用factory函数
	//......												//一如既往地使用pInv
	//......
	auto pInv2 = std::move(pInv);
}											//经由unique_ptr析构函数自动释放资源

//使用shared_ptr
void f13_2() {
	std::tr1::shared_ptr<Investment> pInv1(createInvestment());
	//.......
	auto pInv2(pInv1);										//reference-count++
	//.......
}											//shared_ptr dtor引用为0时调用deleter释放资源


//ITEM 14
//并非所有资源都是heap-based，建立自己的资源管理类！！！！！！！！！！！！！
class Mutex {};									//互斥锁
void lock(Mutex* pm);							//lock
void unlock(Mutex* pm);							//unlock
//对RAII对象被复制的处理方案：
//1.禁止复制
class Lock_1 {
public:
	explicit Lock_1(Mutex* pm) :mutexPtr(pm) {
		lock(mutexPtr);
	}
	Lock_1(const Lock_1&) = delete;
	Lock_1(Lock_1&&) = delete;						//看情况
	Lock_1& operator=(const Lock_1&) = delete;
	Lock_1& operator=(Lock_1&&) = delete;			//
	~Lock_1() {
		unlock(mutexPtr);
	}
private:
	Mutex* mutexPtr;
};

//2.使用“引用计数法”
class Lock_2 {										//原文没有unique_ptr，这里只是展示用法
public:
	explicit Lock_2(Mutex* pm, Mutex* pm2)
		:mutexPtr(pm, unlock),						//使用函数（对象）初始化deleter
		mutexPtr2(pm2, unlock)						//展示如何在unique_ptr中使用deleter
	{
		lock(mutexPtr.get());
		lock(mutexPtr2.get());
	}
	//不再声明dtor，自动调用non-static成员dtor，当引用计数为0时，调用deleter
private:
	std::tr1::shared_ptr<Mutex> mutexPtr;				//使用shared_ptr替换raw pointer
	std::unique_ptr<Mutex, void(*)(Mutex*)> mutexPtr2;	//unique_ptr有点不同
};


//ITEM 15
class Investment;								//investment继承体系root class
bool Investment::isTaxFree()const {
	return true;
}
Investment* createInvestment();					//factory函数

//经由operator* operator->访问资源
void f15_1() {
	std::tr1::shared_ptr<Investment> pi1(createInvestment());	//管理一笔资源
	bool taxable1 = !(pi1->isTaxFree());						//经由operator->访问资源
	std::unique_ptr<Investment> pi2(createInvestment());
	bool taxable2 = !((*pi2).isTaxFree());						//经由operator* 访问资源

}

//有时候还是必须取得RAII对象内的原始资源
class FontHandle {};							//字体相关类 API
FontHandle getFont() { return FontHandle(); }	//C API
void releaseFont(FontHandle fh) {}				//C API

//提供get()，作为显式转换函数，返回原始资源
class Font15_1 {								//RAII class
public:
	explicit Font15_1(FontHandle _fh) :fh(_fh) {}
	FontHandle get()const { return fh; }		//提供get()接口，取得原始资源

	~Font15_1() { releaseFont(fh); }			//释放资源
private:
	FontHandle fh;								//原始字体资源
};

//如果觉得get()过于麻烦，那么可以提供隐式转换函数，转型为原始资源
//但这有可能带来麻烦
class Font15_2 {
public:
	explicit Font15_2(FontHandle _fh) :fh(_fh) {}

	operator FontHandle()const {				//隐式转换函数
		return fh;								//FontHandle(fh)
	}

	~Font15_2() { releaseFont(fh); }			//释放资源
private:
	FontHandle fh;								//原始字体资源
};
//麻烦在这里
//用户把 Font 错写为 FontHandle
void f15_2() {
	Font15_2 f1(getFont());
	//......	//Font 错写为 FontHandle		//原意是要copy一个Font对象，
	FontHandle f2 = f1;							//却反而将f1隐式转换为其底部的FontHandle，
	//......									//然后才复制它。
}
//书上原话：
//本来有一个FontHandle对象由f1管理，但它也可以通过f2得到。
//当f1被销毁，字体被释放，而f2因此成为虚吊的(dangle)。
//
//但是感觉有点不对劲，因为不知道这两个类的copy是怎样的
//因此都有可能有问题，无论类型是 FontHandle 还是 Font15_2
//这里不清楚，欢迎讨论。。。。。。。。。。。。。。。。。。。。。


//ITEM 16
//typedef的类型注意delete形式！！!
//为避免诸如此类的错误，最好尽量不要对数组形式做typedef动作。
void f16() {
	typedef std::string S[3];			//作用域内有效
	std::string *p = new S;				//返回一个string*
	//......							//就像new string[3]一样
	//......
	//delete p;							//行为未定义！！！
	delete[] p;							//很好
}


//ITEM 17
class Widget {};

//示例：函数参数阶段抛出异常，则导致资源泄漏
int priority() { return 0; }
void processWidget(std::tr1::shared_ptr<Widget> pw, int priority) {}

void f17() {
	//考虑如下调用：
	processWidget(shared_ptr<Widget>(new Widget), priority());
	//执行顺序无法预测：
	//1.new Widget  2.priority()  3.调用shared_ptr ctor
	//如果priority()抛出异常，那么可能引发资源泄漏。

	//正确做法：
	std::tr1::shared_ptr<Widget> pw(new Widget);
	processWidget(pw, priority());						//这样调用不会造成泄漏
}


//ITEM 18

//1.导入新类型预防错误
class Date {
public:
	Date(int _month, int _day, int _year);		//有可能错误调用
private:
	int month, day, year;
};


struct Day {
	explicit Day(int d) :val(d) {}
	int val;
};
struct Month {
	explicit Month(int d) :val(d) {}
	int val;
};
struct Year {
	explicit Year(int d) :val(d) {}
	int val;
};
class Date1 {
public:
	Date1(const Month& m, const Day& d, const Year& y) {}	//导入新类型
private:
};
//这样消除了 用户调用函数时 顺序错误的问题
void test18_1() {
	//Date1(30, 3, 1995);									//error
	//Date1(Day(30), Month(3), Year(1995));					//error
	Date1(Month(3), Day(30), Year(1995));					//OK
}

//对于导入的新类型，有时限制其值是理所当然的
class Month1 {
public:
	static Month1 Jan() { return Month1(1); }			//为什么使用static参考Item4
	static Month1 Feb() { return Month1(2); }
	//....
	static Month1 Dec() { return Month1(12); }

private:
	explicit Month1(int m) :val(m) {}					//阻止生成新的对象
	int val;
};
class Date2 {
public:
	Date2(const Month1& m, const Day& d, const Year& y) {}	//导入新类型
private:
};
void test18_2() {
	Date2(Month1::Feb(), Day(12), Year(1995));
}

//尽量令你的types的行为与内置types一致

//3.较佳接口的设计原则是先发制人
//比如让factory函数返回智能指针
std::tr1::shared_ptr<Investment> createInvestment18_1();

//4.当事先 指定一个 资源释放函数时，可以将其 置入智能指针
//指定 资源释放函数
void getRidOfInvestment(Investment*) {}
std::tr1::shared_ptr<Investment> creatInvestment_18() {
	//将deleter置入shared_ptr
	std::tr1::shared_ptr<Investment> p1(static_cast<Investment*>(0), getRidOfInvestment);
	//令p1指向正确对象
	//p1 = ...

	//注：最好先创建一个对象然后 置入p1，参考Item 26

	//展示将deleter置入unique_ptr
	std::unique_ptr<Investment, void(*)(Investment*)>
		p2(static_cast<Investment*>(0), getRidOfInvestment);

	return p1;
}


//ITEM 19

//ITEM 20

//ITEM 21

//ITEM 22

//ITEM 23
//以non-member函数替换member函数
//考虑一个网页浏览器，可以清除cache，清除历史，移除cookies
//如果想提供一个清除所有的函数，面临是否将其写为member的选择：
namespace WebBrowserStuff
{
	class WebBrowser
	{
	public:
		void clearCache() {}
		void clearHistory() {}
		void reomveCookies() {}
		/*
		void clearEverything()				//member函数降低了封装性
		{
			clearCache();
			reomveCookies();
		}
		*/
	};

	void clearEverything(WebBrowser& wb)		//降低了编译依存关系
	{
		wb.clearCache();
		wb.clearHistory();
		wb.reomveCookies();
	}

}

//ITEM 24

//ITEM 25
//
//1.普通类的swap
//	i.提供public swap接口，做真正的交换工作。
//	ii.提供所属命名空间专属swap版本，调用public swap接口。
//	iii.提供std全特化版本，调用public swap接口。
namespace Widget_1
{
	class WidgetImpl {};						//存放数据成员和一些简单接口

	class Widght
	{
	public:
		void swap(Widght& other)				//提供public swap
		{
			using std::swap;					//这是必要的
			swap(pImpl, other.pImpl);
		}
	private:
		WidgetImpl* pImpl;
	};

	void swap(Widget_1::Widght& lhs, Widget_1::Widght& rhs)		//命名空间专属swap(non-member)
	{
		lhs.swap(rhs);
	}

}

namespace std									//std::swap全特化版本，调用public swap
{
	template<>
	void swap<Widget_1::Widght>(Widget_1::Widght& lhs, Widget_1::Widght& rhs)
	{
		lhs.swap(rhs);
	}
}

//
//2.模板类的偏特化情况
namespace Widget_2 {
	template<typename T>
	class WidgetImpl {};

	template<typename T>
	class Widght
	{
	public:
		void swap(Widght& other)				//提供public swap
		{
			using std::swap;					//这是必要的
			swap(pImpl, other.pImpl);
		}
	private:
		WidgetImpl<T>* pImpl;
	};

	template<typename T>
	void swap(Widght<T>& lhs, Widght<T>& rhs)				//命名空间专属swap(non-member)
	{
		lhs.swap(rhs);
	}

}

namespace std
{

	/*
	Compile Error		企图偏特化一个function template，这是不允许的。
	template<typename T>
	void swap<Widget_2::Widght<T>>(Widget_2::Widght<T>& lhs, Widget_2::Widght<T>& rhs)
	{
		lhs.swap(rhs);
	}
	*/


	/*
	添加了一个swap函数的重载版本。
	虽然通过编译，但不要添加任何新东西到std内。
	template<typename T>
	void swap(Widget_2::Widght<T>& lhs, Widget_2::Widght<T>& rhs)
	{
		lhs.swap(rhs);
	}
	}*/

}


//ITEM 26

//ITEM 27

//ITEM 28

//ITEM 29

//从异常安全性的观点来看，这个函数很糟糕。
//1.资源泄露	2.数据败坏
class Image
{
public:
	Image(std::istream&) {}
};
class PrettyMenu
{
public:
	void changeBackground(std::istream& imgSrc)		//awful
	{
		lock(&mutex);
		delete bgImage;
		++imageChanges;
		bgImage = new Image(imgSrc);
		unlock(&mutex);
	}
private:
	Mutex mutex;
	Image* bgImage;
	int imageChanges;
};

//智能指针和reset() 提供强烈保证！！
class PrettyMenu_1
{
public:
	void changeBackground(std::istream& imgSrc)
	{
		Lock_1 ml(&mutex);							//Item 13 RAII
		bgImage.reset(new Image(imgSrc));			//reset函数只有在参数成功生成之后调用
		++imageChanges;
	}
private:
	Mutex mutex;
	std::tr1::shared_ptr<Image> bgImage;
	int imageChanges;
};


//copy and swap技术 提供强烈保证！！
//pImpl
struct PMImpl
{
	std::tr1::shared_ptr<Image> bgImage;
	int imageChanges;
};

class PrettyMenu_2
{
public:
	void changeBackground(std::istream& imgSrc)
	{
		using std::swap;							//Item 25
		Lock_1 ml(&mutex);							//Item 13  RAII
		//副本
		std::tr1::shared_ptr<PMImpl> pNew(new PMImpl(*pImpl));		//copy一个副本
		pNew->bgImage.reset(new Image(imgSrc));						//修改副本
		++pNew->imageChanges;
		//使用绝不抛出异常的swap函数，参考Item 25
		swap(pImpl, pNew);											//交换数据
		//然后pNew和ml自动析构
	}
private:
	Mutex mutex;
	std::tr1::shared_ptr<PMImpl> pImpl;
};


//ITEM 30

//ITEM 31


//Handle class
//将真正的实现写在pImpl类中，这个类只需要调用相同函数！！！！！！！！！！！！！！
namespace Test31_1
{
	//Person.h---------------------------------------------------------------
	class PersonImpl;		//forward declaration
	class Date;
	class Address;
	class Person
	{
	public:
		Person(const string&, const Date&, const Address&);
		string name()const;
		Date birthDate()const;
		Address address()const;
	private:
		std::tr1::shared_ptr<PersonImpl> pImpl;
	};

	//PersonImpl.h---PersonImpl.cpp------------------------------------------
	//#include "Date.h"
	//#include "Address.h"
	//假设这两个类已经写好了。。。。。
	class Date {};   class Address {};
	class PersonImpl
	{
	public:
		PersonImpl(const string& name, const Date& Date, const Address& address) {}
		string name()const { return theName; }
		Date birthDate()const { return theBirthDate; }
		Address address()const { return theAddress; }
	private:
		string theName;
		Date theBirthDate;
		Address theAddress;
	};



	//Person.cpp-------------------------------------------------------------
	//#include "Person.h"
	//#include "PersonImpl.h"
	//完全相同的成员函数，两者接口完全相同！！！！！！！！！！
	//直接调用即可！！！！！！
	Person::Person(const string& name, const Date& date, const Address& address)
		:pImpl(new PersonImpl(name, date, address)) {}
	string Person::name() const
	{
		return pImpl->name();
	}
	Date Person::birthDate()const
	{
		return pImpl->birthDate();
	}
	Address Person::address()const
	{
		return pImpl->address();
	}
	//变成handle class并不会改变它做的事，只是改变做事的方法。
	//这样子分离了接口与实现

}


//Interface class
//interface只提供接口
//工厂函数
namespace Test31_2
{
	class Date {};
	class Address {};
	class Person
	{
	public:
		virtual ~Person() = 0 {}
		virtual string name()const = 0;
		virtual Date birthDate()const = 0;
		virtual Address address()const = 0;
		static std::tr1::shared_ptr<Person>							//工厂函数
			create(const string& name, const Date& date, const Address& address);
	};


	class APerson :public Person
	{
	public:
		APerson(const string& name, const Date& date, const Address& address) {}
		~APerson() {}
		string name()const { return theName; }
		Date birthDate()const { return theBirthDate; }
		Address address()const { return theAddress; }
	private:
		string theName;
		Date theBirthDate;
		Address theAddress;
	};

	class BPerson :public Person
	{
	public:
		BPerson(const string& name, const Date& date, const Address& address) {}
		~BPerson() {}
		string name()const { return theName; }
		Date birthDate()const { return theBirthDate; }
		Address address()const { return theAddress; }
	private:
		string theName;
		Date theBirthDate;
		Address theAddress;
	};

	class CPerson :public Person
	{
	public:
		CPerson(const string& name, const Date& date, const Address& address) {}
		~CPerson() {}
		string name()const { return theName; }
		Date birthDate()const { return theBirthDate; }
		Address address()const { return theAddress; }
	private:
		string theName;
		Date theBirthDate;
		Address theAddress;
	};

	std::tr1::shared_ptr<Person>							//工厂函数
		Person::create(const std::string& name, const Date& date, const Address& address)
	{
		//acorrding to Args
		return std::tr1::shared_ptr<Person>(new APerson(name, date, address));
		return std::tr1::shared_ptr<Person>(new BPerson(name, date, address));
		return std::tr1::shared_ptr<Person>(new CPerson(name, date, address));
	}


}


//ITEM 32



//ITEM 33



//ITEM 34

//ITEM 35

//ITEM 36

//使用tr1::function包装 成员函数
namespace Test36_1
{
	class A
	{
	public:
		int foo(int x) { return 1; }
	};

	class B
	{
		typedef std::tr1::function<int(int)> MyFunc;
	public:
		B(MyFunc& f) :myFunc(f) {}
	private:
		MyFunc myFunc;
	};

	void test()
	{
		using namespace std::placeholders;
		A a;
		B b(
			static_cast<std::tr1::function<int(int)>>
			(std::bind(&A::foo, &a, _1))
		);
	}

}

//传统的Strategy模式
//将 工作函数(virtual 成员函数)剥离出来，作为一个base class。
//然后在base class中提供 virtual成员函数 用于工作，
//原class 含有一个base class的pointer，调用工作函数，完成多态。

//实际上，虚函数转移了，并且可以任意丰富、修改。 
//与NVI手法有区别，注意！！！

//Test2是原来的写法，Test3是修改后的写法

namespace Test36_2
{
	class Person
	{
	public:
		virtual void eat()
		{
			//doSomething...
		}

	};

}

namespace Test36_3
{
	class Person;				//forwarding declaration

	//原来的成员函数，现在作为base class
	class EatFunc
	{
	public:
		//virtual函数转移了
		virtual void doEat(const Person&)
		{
			//doSomething...
		}

	};

	class EatA :public EatFunc {};
	class EatB :public EatFunc {};



	class Person
	{
	public:
		//现在是non-virtual
		void eat()
		{
			//这里实现多态
			eatFunc->doEat(*this);
		}
	private:
		EatFunc* eatFunc;

	};


}


//ITEM 37

//函数声明静态绑定，virtual函数体动态绑定。
//缺省参数值根据声明确定。
namespace Test37_1
{
	class Base
	{
	public:
		virtual void foo(int x = 1)
		{
			cout << "Base " << x << endl;
		}
	};

	class Derived : public Base
	{
	public:
		void foo(int x = 2)
		{
			cout << "Derived " << x << endl;
		}
	};


	void test()
	{
		Derived d;
		Base* pb = &d;
		pb->foo();				//output  Derived 1		！！！！！

	}

}

//使用NVI手法来避免
namespace Test37_2
{

	class Base
	{
	public:
		void foo(int x = 1)
		{
			this->doFoo(x);
		}

	private:
		virtual void doFoo(int x)
		{
			cout << "Base " << x << endl;
		}
	};

	class Derived : public Base
	{
	private:
		void doFoo(int x)
		{
			cout << "Derived " << x << endl;
		}
	};


	void test()
	{
		Derived d;
		Base* pb = &d;
		pb->foo();				//output  Derived 1

	}


}


//ITEM 38

//ITEM 39

//将 private继承 转换为 复合(composition)
namespace Test39_1
{
	class B
	{
	public:
		virtual void foo() {}
	};

	//private继承
	class C1 :private B
	{
	private:
		virtual void foo() {}
	};

	//public继承 + 复合composition
	class C2
	{
	private:
		//内部类，也可以将定义放在外面，然后这里存一个指针。
		class C_B :public B {
		public:
			virtual void foo() {}
		};
		C_B b;				//一个对象
	};


}


//ITEM 40

//virtual继承，base初始化
namespace Test40_1
{
	class File {};
	class InputFile :virtual public File {};
	class OutputFile :virtual public File {};
	class IOFile :public InputFile, public OutputFile
	{
	public:
		//承担virtual base初始化的责任
		IOFile() :File(), InputFile(), OutputFile() {}
	};


}


//MI的一个用途：
//public继承一个interface，并private继承一个协助实现的class。
namespace Test40_2
{
	class DatabaseID {};

	//Interface
	class IPerson
	{
	public:
		virtual ~IPerson() = default;
		virtual string name()const = 0;
		virtual string birthDate()const = 0;
	};

	//辅助实现class
	class PersonInfo
	{
	public:
		explicit PersonInfo(DatabaseID pid);
		virtual ~PersonInfo() = default;
		virtual const char* theName()const;
		virtual const char* theBirthDate()const;
		virtual const char* valueDelimOpen()const;
		virtual const char* valueDelimClose()const;
	};

	//MI
	class CPerson :public IPerson, private PersonInfo
	{
	public:
		explicit CPerson(DatabaseID pid) :PersonInfo(pid) {}

		//implement Interface
		virtual string name()const
		{
			return PersonInfo::theName();
		}
		virtual string birthDate()const
		{
			return PersonInfo::theBirthDate();
		}

		//重新定义继承而来的virtual函数。
	private:
		const char* valueDelimOpen()const
		{
			return "";
		}

		const char* valueDelimClose()const
		{
			return "";
		}

	};

}


//ITEM 41

//ITEM 42

//ITEM 43

//compiler拒绝在 模板化基类 内寻找 继承而来的名称
namespace Test43_1
{
	template<typename T>
	class Base
	{
	public:
		void boo() {}
	};

	template<>
	class Base<string> {};							//empty

	template<typename T>
	class Derived :public Base<T>
	{
	public:
		void doo()
		{											//VS居然通过了编译。。。
			//boo();								//GCC-4.9.2没有通过编译。。。
		}											//
	};

	void test()
	{
		Derived<string> d;
		//d.doo();									//CE	(VS2015)	(废话。。。)
	}

}

//solution
//1. this->
//2. using 函数声明
//3. 不推荐。
namespace Test43_2
{
	template<typename T>
	class Base
	{
	public:
		void boo() {}
	};

	template<>
	class Base<string> {};

	template<typename T>
	class Derived :public Base<T>
	{
	public:
		//2. using 函数声明
		using Base<T>::boo;
		void doo()
		{
			//1. this->
			this->boo();							//两个选一个就行
		}
	};

}


//ITEM 44

//非类型模板参数带来的膨胀。
//考虑一个矩阵类，支持求逆运算。
namespace Test44_1
{
	template<typename T, std::size_t n>				//n是非类型参数non-type parameter
	class SquareMatrix
	{
	public:
		void invert() {}
	};

	//考虑如下使用
	void test()
	{
		SquareMatrix<double, 10> sm1;
		//.....
		sm1.invert();								//生产出2份invert
		SquareMatrix<double, 5>sm2;
		//......
		sm2.invert();								//除了常量5、10完全相同
	}

}

//修改如下：
//好处是：几乎所有“同类型矩阵”都可以inline调用base class::invert()函数。
namespace Test44_2
{
	template<typename T>
	class SquareMatrixBase								//与大小无关
	{
	protected:
		SquareMatrixBase(std::size_t n, T* pMem)		//存储矩阵大小和
			:size(n), pData(pMem) {}					//一个指针，指向矩阵内存。
		void invert(std::size_t n) {}					//这里指定大小
	private:
		std::size_t size;
		T* pData;
	};

	template<typename T, std::size_t n>
	class SquareMatrix :private SquareMatrixBase<T>		//private继承，帮助实现
	{
	public:
		SquareMatrix() :								//送出矩阵大小和
			SquareMatrixBase<T>(n, data) {}				//数据指针给base class
		using SquareMatrixBase<T>::invert;
	protected:
		void invert() { this->invert(n); }				//inline调用，this-> 参考Item 43
	private:
		T data[n*n];
	};

}


//ITEM 45

//
namespace Test45_1
{
	template<typename T>
	class SmartPtr
	{
	public:
		explicit SmartPtr(T* realPtr) :heldPtr(realPtr) {}
		template<typename U>
		SmartPtr(const SmartPtr<U>& other)
			: heldPtr(other.get()) {}								//编译保证 继承关系
		T* get() const { return heldPtr; }
	private:
		T* heldPtr;
	};

	class Base {};
	class Derived :public Base {};

	void test()
	{
		SmartPtr<Base> pb = SmartPtr<Derived>(new Derived);			//继承关系
	}

}


//ITEM 46

//
namespace Test46_1
{
	template<typename T>
	class Rational
	{
	public:
		Rational(const T& _numerator = 0, const T& _denominator = 1)
			:Numerator(_numerator), Denominator(_denominator) {}
		const T numerator()const;
		const T denominator()const;
		friend										//如果不声明friend，无法通过编译
			const Rational operator*(const Rational& lhs, const Rational& rhs)
		{											//定义在类模板内
			return doMultiply(lhs, rhs);
		}
	private:
		T Numerator;
		T Denominator;
	};

	/*						无法link
	template<typename T>
	const Rational<T> operator*(const Rational<T>& lhs, const Rational<T>& rhs)
	{
		return Rational<T>();
	}
	*/

	template<typename T>
	const Rational<T> doMultiply(const Rational<T>& lhs, const Rational<T>& rhs)
	{
		return Rational<T>();
	}

	void test()
	{
		Rational<int> half = { 1,2 };
		Rational<int> result = half * 2;				//如果不声明friend，无法通过编译
	}

}


//ITEM 47

//考虑一个STL迭代器向前移动的函数
namespace Tes47_1
{
	//这份实现用于random access迭代器
	template<typename IterT, typename DistT>
	void doAdvance(IterT& iter, DistT d, std::random_access_iterator_tag)
	{
		iter += d;
	}

	//这份实现用于bidirectional迭代器
	template<typename IterT, typename DistT>
	void doAdvance(IterT& iter, DistT d, std::bidirectional_iterator_tag)
	{
		if (d >= 0) { while (d--)++iter; }
		else { while (d++)--iter; }
	}

	//这份实现用于input迭代器
	template<typename IterT, typename DistT>
	void doAdvance(IterT& iter, DistT d, std::input_iterator_tag)
	{
		if (d < 0)
		{
			throw std::out_of_range("Negative distance")
		}
		while (d--)++iter;
	}

	//有了这些doAdvance重载版本，advance只需要做的事调用它们，
	//并额外传递一个对象(迭代器分类)。
	template<typename IterT, typename DistT>
	void advance(IterT& iter, DistT d)
	{
		doAdvance(
			iter, d,
			typename std::iterator_traits<IterT>::iterator_category()
		)
	}

}



//ITEM 48

//ITEM 49


//ITEM 50


//ITEM 51


//ITEM 52


//ITEM 53


//ITEM 54


//ITEM 55

