#include <iostream>
#include <memory>
#include "pattern.hpp"
using namespace std;

struct ExprNum { int value; };
struct ExprNeg;
struct ExprAdd;
struct ExprMul;
using Expr = tagged_union<ExprNum, ExprNeg, ExprAdd, ExprMul>;
struct ExprNeg { std::unique_ptr<Expr> sub; };
struct ExprAdd { std::unique_ptr<Expr> l, r; };
struct ExprMul { std::unique_ptr<Expr> l, r; };

int eval(const Expr &e) {
	return match<int>(e
		, [](const ExprNum &e) { return e.value; } // I like shadowing
	, [](const ExprNeg &e) { return -eval(*e.sub); }
	, [](const ExprAdd &e) { return eval(*e.l) + eval(*e.r); }
	, [](const ExprMul &e) { return eval(*e.l) * eval(*e.r); }
	);
}

bool is_add_or_mul(const Expr &e) {
	return match<bool>(e
		, [](const ExprAdd &) { return true; }
	, [](const ExprMul &) { return true; }
	, [](auto &) { return false; }
	);
}

void test() {
	auto e = Expr{ ExprAdd{
		make_unique<Expr>(ExprMul{
		make_unique<Expr>(ExprNum{ 2 }),
		make_unique<Expr>(ExprNum{ 22 }),
			}),
			make_unique<Expr>(ExprNeg{
		make_unique<Expr>(ExprNum{ 2 }),
				}),
	} };
	cout << eval(e) << endl; // 42
	cout << boolalpha << is_add_or_mul(e) << endl; // true
}


int main() {

	test();

	getchar();
	return 0;
}