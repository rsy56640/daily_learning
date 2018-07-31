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


&nbsp;   
<a id="2"></a>
## numpy.linspace ##

    a = np.linspace(1, 10, 10)
    print(a)  # [ 1.  2.  3.  4.  5.  6.  7.  8.  9. 10.]



&nbsp;   
<a id="3"></a>
## numpy. ##




&nbsp;   
<a id="4"></a>
## numpy. ##




&nbsp;   
<a id="5"></a>
## numpy. ##




