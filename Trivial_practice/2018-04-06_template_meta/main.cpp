#include <iostream>
using namespace std;


//TMP for fibonacci sequence


template<int N>
struct Foo
{
	enum { val = Foo<N - 1>::val + Foo<N - 2>::val };
};
template<>
struct Foo<0>
{
	enum { val = 0 };
};
template<>
struct Foo<1>
{
	enum { val = 1 };
};


//helper
template<int ...values>
struct Ary
{
	static const int value[sizeof ...(values)];
};
template<int ...values>
const int Ary<values...>::value[] = { values... };


//recursion for produce the fib sequenec, than init the Ary with that.
template<int N, int ...values>
struct recur :recur<N - 1, Foo<N>::val, values...> {};
template<int ...values>
struct recur <0, values...> : Ary<0, values...> {};


template<int N>
struct Fib : recur<N> {};


//get fib
inline int fib(int n)
{
	static const auto obj = Fib<50>{};
	return obj.value[n];
}



int main() {


	for (int i = 0; i < 20; ++i)
		cout << fib(i) << endl;


	system("pause");
	return 0;
}