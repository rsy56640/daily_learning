# Numpy learning #
[Numpy Tutorial -Youtube](https://www.youtube.com/playlist?list=PLeo1K3hjS3uset9zIVzJWqplaWBiacTEU)   
[python之numpy的基本使用 - CSDN](https://blog.csdn.net/cxmscb/article/details/54583415)    
[(译) Python Numpy Tutorial——CS231n - CSDN](https://blog.csdn.net/g11d111/article/details/78844460)   

<a id="1"></a>
## numpy.array ##
3 benefits of `numpy.array` over `python.list`   

- less memory
- fast
- convinient

一些基本方法：  

>
axis = 0 指矩阵纵向   （感觉很迷？？？）    
axis = 1 指矩阵横向    

    a = np.array([1, 2, 3])
    a = np.arange(1000)
    print(a.size * a.itemsize)  # 4000
    b = np.arange(1000)
    c = a + b
    
    a = np.array([4, 5, 6])
    b = np.array([1, 2, 3])
    print(a + b)                # [5 7 9]
    print(a - b)                # [3 3 3]
    print(a * b)                # [ 4 10 18]
    print(a / b)                # [4.  2.5 2. ]

矩阵：   

    a = np.array([[1, 2], [3, 4], [5, 6]], dtype=np.float64)
    print(a.ndim)
    print(a.shape)
    print(a.dtype)
    print(a.itemsize)
    print(a)

矩阵运算：   

    a = np.array([[1, -2, 3], [4, 17, 9], [9, 7, -5]])
    print(a)
    print("Transpose: \n", a.transpose())
    print("Inversion matrix: \n", inv(a))
    
    b = np.array([1, 2, 3])
    x = solve(a, b)  # ax = b
    print("solve ax = b: \n", x)
    
    c = np.dot(a, b)
    print("a * b: \n", c)

变形：   

    a = np.array([[1, -2, 3], [-4, 5, 6]])
    print(a.shape, "\n", a)
    print(a.reshape(6, 1))
    print(a.reshape(3, 2))
    print(a.reshape(1, 6))
    print(a.ravel())  # matrix -> list

    a = np.array([1, 2, 3],)       # 矩阵3x1
    print(a.shape)                 # (3,)
    
    a = np.array([[1, 2, 3], ])    # 矩阵1x3
    print(a.shape)                 # (1, 3)

遍历：   

    a = np.array([[1, 7, -4], [3, 6, 6], [-8, 9, 2]])
    for row in a:
        print(row)
    for cell in a.flat:
        print(cell)

切片：    

    print(a[1:3, 2])  # 矩阵2x1
    print(a[0:1, :])  # 矩阵1x3

`linspace`:   

    a = np.linspace(1, 10, 10)
    print(a)  # [ 1.  2.  3.  4.  5.  6.  7.  8.  9. 10.]

`vstack`, `hstack`:    

    print(np.vstack((a, b)))    # vertical 拼接
    print(np.hstack((a, a)))    # horizontal 拼接

`vsplit`, `hsplit`:   

    a = np.arange(30).reshape(2, 15)
    print(a)
    print(np.hsplit(a, 3))      # horizontal 分割
    print(np.vsplit(a, 2))      # vertical 分割

index 是 bool矩阵:   

    a = np.arange(12).reshape(3, 4)
    print(a)
    b = (a > 4) & (a < 7)       # boolean matrix
    print(a[b])                 # 只有 5, 6
    a[b] = -1                   # 5, 6 变 -1
    print(a)



&nbsp;   
<a id="2"></a>
## numpy. ##

