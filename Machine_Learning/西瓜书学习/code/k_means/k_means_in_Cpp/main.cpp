#include <iostream>
#include <string>
#include <numeric>
#include <cmath>
#include <functional>
#include "k_means.hpp"

struct myVec
{
	double v[3];
	myVec()
	{
		v[0] = 0.0;
		v[1] = 0.0;
		v[2] = 0.0;
	}
	myVec(double a, double b, double c)
	{
		v[0] = a;
		v[1] = b;
		v[2] = c;
	}
	myVec(const myVec& other)
	{
		v[0] = other.v[0];
		v[1] = other.v[1];
		v[2] = other.v[2];
	}
	myVec& operator=(const myVec& other)
	{
		v[0] = other.v[0];
		v[1] = other.v[1];
		v[2] = other.v[2];
		return *this;
	}
	myVec& operator+=(const myVec& other)
	{
		v[0] += other.v[0];
		v[1] += other.v[1];
		v[2] += other.v[2];
		return *this;
	}
	myVec& operator/=(std::size_t size)
	{
		v[0] /= size;
		v[1] /= size;
		v[2] /= size;
		return *this;
	}
};

double dist(const myVec& lhs, const myVec& rhs)
{
	double a = lhs.v[0] - rhs.v[0];
	double b = lhs.v[1] - rhs.v[1];
	double c = lhs.v[2] - rhs.v[2];
	return std::sqrtf(a * a + b * b + c * c);
}

bool operator==(const myVec& lhs, const myVec& rhs)
{
	if (abs(lhs.v[0] - rhs.v[0]) < 10e-6 &&
		abs(lhs.v[1] - rhs.v[1]) < 10e-6 &&
		abs(lhs.v[2] - rhs.v[2]) < 10e-6)
		return true;
	return false;
}

std::ostream& operator<<(std::ostream& os, const myVec& other)
{
	os << other.v[0] << "\t" << other.v[1] << "\t" << other.v[2];
	return os;
}

void test()
{
	std::vector<myVec> data =
	{
		myVec{1.2, 2.3, 3.14}, myVec{1.7, 3.3, 4.4}, myVec{1.5, 3, 3.7},
		myVec{7.2, 8, 19}, myVec{7, 8.3, 20}, myVec{7.5, 7.6, 18},
		myVec{11.8, 22, -4.2}, myVec{12, 20, -4}, myVec{10, 19, -2.3}
	};

	std::function<double(const myVec&, const myVec&)> _dist = dist; // necessary or explicit instantiate the funciton template.

	auto result = RSY_TOOL::K_means::k_means<myVec, double>(data, 3u, dist);
	for (auto const& cluster : result)
	{
		for (auto const& vec : cluster)
			std::cout << vec << std::endl;
		std::cout << std::endl;
	}
}


int main()
{
	test();
	system("pause");
	return 0;
}