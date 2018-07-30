import numpy as np
import PIL as pil
from functools import reduce
import functools
import os
from PIL import Image
import imageio
from skimage import io
import types

# os.environ["TF_CPP_MIN_LOG_LEVEL"] = "3"

# d = {"3": 4, 4: 4, int(input()) + 3: 6}
#
# print(d[d["3"]])
# print(d)
#
#
# def power(x, n=2):
#     s = 1
#     tmp = x
#     while n:
#         if n & 1:
#             s *= tmp
#         tmp *= tmp
#         n >>= 1
#     return s
#
#
# print(power(5))
# print(power(3, 8))
#
#
# def sum_square(*numbers):
#     sum = 0
#     for number in numbers:
#         sum += number * number
#     return sum
#
#
# print(sum_square(1, 2, 3))
# d = (1, 2, 3)
# print(sum_square(*d))
#
# def test_keyword(name, **kk):
#     print(name, kk)
#
#
# d = {1: 11, 2: 22}
# test_keyword("rsy", a="hhh", b=2, c=d)
# test_keyword("rsy", **{'a': "hhh", 'b': 2, 'c': d})
#
"""
*args 是可变参数，args 接收的是一个 tuple；
**kw 是关键字参数，kw 接收的是一个 dict。
以及调用函数时如何传入可变参数和关键字参数的语法：
    可变参数既可以直接传入：func(1, 2, 3)；
    又可以先组装 list 或 tuple，再通过*args 传入：func(*(1, 2, 3))；
关键字参数既可以直接传入：func(a=1, b=2)；
又可以先组装 dict，再通过**kw 传入：func(**{'a': 1, 'b': 2})。
"""


# # recursion
# def power_(x, n=2):
#     if n == 1:
#         return x
#     v = power_(x, n >> 1)
#     if n & 1:
#         return v * v * x
#     else:
#         return v * v
#
# l_range = range(100)[7:15]
#

#
# print([d for d in os.listdir(".")])
#
# generator = (x * x for x in range(10))
# for x in range(10):
#     print(generator.__next__())
# for x in generator:
#     print(x)
#
#
# def fib_g(max):
#     n, a, b = 0, 0, 1
#     while n < max:
#         yield b
#         a, b = b, a + b
#         n = n + 1
#
#
# a = fib_g(6)
# for x in a:
#     print(x)

# 函数式编程


#
#
# def str2int(str):
#     def char2num(ch):
#         return {'0': 0, '1': 1, '2': 2, '3': 3, '4': 4, '5': 5,
#                 '6': 6, '7': 7, '8': 8, '9': 9}[ch]
#
#     return reduce(lambda x, y: 10 * x + y, map(char2num, str))
#
#
# print(str2int("00106893"))
#
# def is_odd(n):
#     return n % 2 == 1
# d = filter(is_odd, [1, 2, 3, 4, 5, 6])
#
# for x in d:
#     print(x)
#
# def lazy_sum(*args):
#     def sum():
#         ax = 0
#         for n in args:
#             ax = ax + n
#         return ax
#
#     return sum
#
#
# f = lazy_sum(*(1, 2, 3, 4, 5))
# print(f)
# print(f())  # output 25

# def count1():
#     fs = []
#     for i in range(1, 4):
#         def f():
#             return i * i
#
#         fs.append(f)
#     return fs
#
#
# f1, f2, f3 = count1()
# print(f1())  # output 9
# print(f2())  # output 9
# print(f3())  # output 9
#
# def count2():
#     fs = []
#     for x in range(1, 4):
#         def f(i):
#             def g():
#                 return i * i
#
#             return g
#
#         fs.append(f(x))
#     return fs
#
#
# f1, f2, f3 = count2()
# print(f1())  # output 1
# print(f2())  # output 4
# print(f3())  # output 9
#
#
# def count3():
#     fs = []
#     for x in range(1, 4):
#         def f(i):
#             return lambda: i * i
#
#         fs.append(f(x))
#     return fs
#
#
# f1, f2, f3 = count3()
# print(f1())  # output 1
# print(f2())  # output 4
# print(f3())  # output 9

# def log(func):
#     def wrapper(*args, **kw):
#         print("call %s():" % func.__name__)
#         func(*args, **kw)
#         print("call finished")
#
#     return wrapper
#
#
# @log
# def now():
#     print("123456")
#
#
# now()
#
# int2 = functools.partial(int, base=2)
#
# print(int2("10001"))
#
# im1 = io.imread("./img_0394.jpg")
# im2 = imageio.imread("./img_0394.jpg")
# im3 = Image.open("./img_0394.jpg")
# print(im3.format, im3.size, im3.mode)


# # 面向对象
#
# class Student(object):
#     def __init__(self, name, sex):
#         self.__name = name
#         self.__sex = sex
#
#     def info(self):
#         print(self.__name, self.__sex)
#
#     @property
#     def name(self):
#         return self.__name
#
#     @name.setter
#     def name(self, name):
#         if not isinstance(name, str):
#             raise ValueError("string needed")
#         self.__name = name
#
#
# me = Student("rsy", "M")
# me.name = "asd"
# print(me.name)
#
# me.info()  # output "rsy M"
# me.__name = "aaa"
# me.info()  # output "rsy M"
# print(me.__name)  # output "aaa"
#
# isinstance()
# type()
# print(dir(type(me)))
#
# print(hasattr(me, "__name"))  # output "False"
# print(hasattr(me, "age"))  # output "False"
# setattr(me, "age", 21)
# print(getattr(me, "age"))  # output "21"

# class B1(object):
#     __slots__ = ("id")
#
#
# class D1(B1):
#     __slots__ = ("hh")
#
#
# b1 = B1()
# b1.id = 1
#
# d1 = D1()
# d1.id = 2
# d1.hh = "asd"
