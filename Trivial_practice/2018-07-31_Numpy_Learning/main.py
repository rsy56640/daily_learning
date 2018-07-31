import numpy as np
from numpy.random import rand
from numpy.linalg import solve, inv

# array
a = np.array([1, 2, 3])
a = np.arange(1000)
print(a.size * a.itemsize)  # 4000
b = np.arange(1000)
c = a + b

a = np.array([4, 5, 6])
b = np.array([1, 2, 3])
print(a + b)  # [5 7 9]
print(a - b)  # [3 3 3]
print(a * b)  # [ 4 10 18]
print(a / b)  # [4.  2.5 2. ]

a = np.array([[1, 2], [3, 4], [5, 6]], dtype=np.float64)
print(a.ndim)
print(a.shape)
print(a.dtype)
print(a.itemsize)
print(a)

a = np.array([[1, -2, 3], [4, 17, 9], [9, 7, -5]])
print(a)
print("Transpose: \n", a.transpose())
print("Inversion matrix: \n", inv(a))

b = np.array([1, 2, 3])
x = solve(a, b)  # ax = b
print("solve ax = b: \n", x)

c = np.dot(a, b)
print("a * b: \n", c)

print(np.arange(1, 11, 3))

a = np.linspace(1, 10, 10)
print(a)  # [ 1.  2.  3.  4.  5.  6.  7.  8.  9. 10.]

a = np.array([[1, 2, 3], ])
print(a.shape)  # (1, 3)
b = np.array([1, 2, 3], )
print(b.shape)  # (3,)

a = np.array([[1, -2, 3], [-4, 5, 6]])
print(a.shape, "\n", a)
print(a.reshape(6, 1))
print(a.reshape(3, 2))
print(a.reshape(1, 6))
print(a.ravel())  # matrix -> list

a = np.array([[1, 7, -4], [3, 6, 6], [-8, 9, 2]])
for row in a:
    print(row)
for cell in a.flat:
    print(cell)

print(a[1:3, 2])  # 矩阵2x1
print(a[0:1, :])  # 矩阵1x3

b = np.array([[1, 2, 3], ])  # 1x3
c = np.array([1, 2, 3])  # 3x1

print(a)
print(np.sum(a, axis=1))
print(np.vstack((a, b)))
print(np.hstack((a, a)))

a = np.arange(30).reshape(2, 15)
print(a)
print(np.hsplit(a, 3))
print(np.vsplit(a, 2))

a = np.arange(12).reshape(3, 4)
print(a)
b = (a > 4) & (a < 7)  # boolean matrix
print(a[b])
a[b] = -1
print(a)
