# GCC扩展 [&&](https://gcc.gnu.org/onlinedocs/gcc/Labels-as-Values.html#Labels-as-Values)  
>
You can get the address of a label defined in the current function (or a containing function) with the unary operator ‘&&’. The value has type void *. This value is a constant and can be used wherever a constant of that type is valid.   
The &&foo expressions for the same label might have different values if the containing function is inlined or cloned. If a program relies on them being always the same, __attribute__((__noinline__,__noclone__)) should be used to prevent inlining and cloning. If &&foo is used in a static variable initializer, inlining and cloning is forbidden.

给个trivial的例子：

    void switch_eg(long x, long n, long* dest)
    {
	long val = x;
	switch(n)
	{
		case 100:
			val *= 13;
			break;
			
		case 102:
			val += 10;
			/* fall through */
			
		case 103:
			val += 11;
			break;
			
		case 104:
		case 106:
			val *= val;
			break;
		
		default:
			val = 0;
	}
	    *dest = val;
    }
    
    
    //汇编代码
    switch_eg(long, long, long*):
        sub     rsi, 100
        cmp     rsi, 6
        ja      .L8
        jmp     [QWORD PTR .L4[0+rsi*8]]      //索引 index 由寄存器 %rsi 给出
    .L4:                                      //跳转表
        .quad   .L3
        .quad   .L8
        .quad   .L5
        .quad   .L6
        .quad   .L7
        .quad   .L8
        .quad   .L7
    .L5:
        add     rdi, 10
    .L6:
        add     rdi, 11
        mov     QWORD PTR [rdx], rdi
        ret
    .L3:
        lea     rax, [rdi+rdi*2]
        lea     rdi, [rdi+rax*4]
        mov     QWORD PTR [rdx], rdi
        ret
    .L7:
        imul    rdi, rdi
        mov     QWORD PTR [rdx], rdi
        ret
    .L8:
        xor     edi, edi
        mov     QWORD PTR [rdx], rdi
        ret
        
   
&nbsp;    
现在使用GCC扩展 `&&`，改一下变成这个样子：
        
    void switch_eg_impl(long x, long n, long* dest)
    {
	    static void* jt[7] =
	    {
		    &&loc_A, &&loc_def, &&loc_B,
		    &&loc_C, &&loc_D, &&loc_def,
		    &&loc_D
	    };
	    unsigned long index = n - 100;
	    long val;
	    if(index > 6)
		    goto loc_def;
	    /* Multiway branch */
	    goto *jt[index];
	    
	    loc_A:	    /* case 100 */
		    val *= 13;
		    goto done;
	    
	    loc_B:    	/* case 102 */
		    x = x + 10;
		    /* fall through */
		    
	    loc_C:	    /* case 103 */
		    val += 11;
		    goto done;
		
	    loc_D:	    /* case 104, 106 */
		    val = x * x;
		    goto done;
	    
	    loc_def:	/* Default case */
		    val = 0;
	    
	    done:
		    *dest = val;
    }
        
        
    //汇编代码
    switch_eg_impl(long, long, long*):
        sub     rsi, 100
        cmp     rsi, 6
        ja      .L2
        jmp     [QWORD PTR switch_eg_impl(long, long, long*)::jt[0+rsi*8]]
    .L3:
    .L2:
        xor     edi, edi
        mov     QWORD PTR [rdx], rdi
        ret
    .L5:
    .L6:
        mov     edi, 11
        mov     QWORD PTR [rdx], rdi
        ret
    .L7:
        imul    rdi, rdi
        mov     QWORD PTR [rdx], rdi
        ret
    switch_eg_impl(long, long, long*)::jt:
        .quad   .L3
        .quad   .L2
        .quad   .L5
        .quad   .L6
        .quad   .L7
        .quad   .L2
        .quad   .L7
        
.L4和 `jt` 那一列是跳转表，位置根据索引 `index` 给出，当case比较连续时，可以使用跳转表。  如果case比较离散并且差距较大时，采用 逐条判断 或者 跳转表 均会导致程序效率低。在这种情况下，编译器就会采用二分查找法实现switch语句，程序编译时，编译器先将所有case值排序后按照二分查找顺序写入汇编代码，在程序执行时则采二分查找的方法在各个case值中查找条件值，如果查找到则执行对应的case语句，如果最终没有查找到则执行default语句。   
[C++性能榨汁机之switch语句](https://zhuanlan.zhihu.com/p/38139553)   

关于二分查找：
&nbsp; 

    int test_switch(long n)
    {
	    int i = 0;
        switch(n)
	    {
            case 4: i = 4;break;
            case 10: i = 10;break;
            case 50: i = 50;break;
            case 100: i = 100;break;
            case 200: i = 200;break;
            case 500: i = 500;break;
            default: i = 0;break;
        }
        return i;
    }
    
    
    //汇编代码
    test_switch(long):
        cmp     rdi, 50
        je      .L3
        jle     .L18
        cmp     rdi, 200
        mov     eax, 200
        je      .L5
        cmp     rdi, 500
        mov     ax, 500
        je      .L5
        cmp     rdi, 100
        mov     ax, 100
        je      .L19
    .L2:
        xor     eax, eax
    .L5:
        rep ret
    .L18:
        cmp     rdi, 4
        mov     eax, 4
        je      .L5
        cmp     rdi, 10
        mov     al, 10
        jne     .L2
        rep ret
    .L19:
        rep ret
    .L3:
        mov     eax, 50
        ret

&nbsp;  

一片讲解GCC Extension && 的文章：   
[Computed goto for efficient dispatch tables](https://eli.thegreenplace.net/2012/07/12/computed-goto-for-efficient-dispatch-tables)   
这篇文章认为使用`&&`扩展会比通常的编译器快，有2个原因：  

- switch 还要做边界检查  
- 流水线的分支预测  

不是很理解。    

&nbsp;  

另外，JVM的策略是 `LookupSwitch` 和 `TableSwitch` 。  
[Difference between JVM's LookupSwitch and TableSwitch?  -  StackOverflow](https://stackoverflow.com/questions/10287700/difference-between-jvms-lookupswitch-and-tableswitch)     
[Compiling Switches - ORACLE Doc](https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-3.html#jvms-3.10)    
[match key in table and jump](https://cs.au.dk/~mis/dOvs/jvmspec/ref--41.html)    
LookupSwitch 就是用 hash，TableSwitch 就是跳转表。   


