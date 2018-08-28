//https://zhuanlan.zhihu.com/p/24876423
#include <type_traits>
#define str_cat(first, second) first##second
#define has_xxx(member_name) \
template<class T, class U = void> \
struct str_cat(has_, member_name): std::false_type {};\
template<class T>\
struct str_cat(has_, member_name)<T, typename SFINAE<typename T::member_name>::type>\
	: std::true_type {};\
template<class T>\
struct str_cat(has_, member_name)<T, typename SFINAE<decltype(T::member_name)>::type>\
	: std::true_type {};

template<class T>
struct SFINAE {
	using type = void;
};


struct A {
	int i = 0;
	void foo() {}
	using void_t = void;
	using type = A;
};

struct B {
	int j = 0;
	double goo() {}
};


has_xxx(void_t) //OK to compile
has_xxx(i)
has_xxx(foo)
has_xxx(j)
has_xxx(goo)
//has_xxx(void_t) //compile error

int main()
{

	has_i<A>::value; // true
	has_i<B>::value; // false

	has_foo<A>::value; // true
	has_foo<B>::value; // false
	has_goo<B>::value; // true

	has_void_t<A>::value; // true
	has_void_t<B>::value; // false

	return 0;
}