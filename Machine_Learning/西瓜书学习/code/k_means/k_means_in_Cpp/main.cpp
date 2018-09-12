#include <iostream>
#include <string>
#include <numeric>
#include <cmath>
#include <functional>
#include <cassert>
#include <initializer_list>
#include "k_means.hpp"

template<std::size_t N, typename = std::enable_if_t<(N > 0u)>> struct myVec
{
	std::vector<double> v;
	bool has_resource = true;

	myVec() :v(N, 0.0) {}
	myVec(const double(&list)[N]) : v(list, list + N) {}
	//myVec(std::initializer_list<double> l) :v(l) {}
	myVec(const myVec& other) :v(other.v) { has_resource = other.has_resource; }
	myVec(myVec&& other) :v(std::move(other.v)) { has_resource = other.has_resource; other.has_resource = false; }
	myVec& operator=(const myVec& other)
	{
		v = other.v;
		has_resource = other.has_resource;
		return *this;
	}
	myVec& operator=(myVec&& other)
	{
		v = std::move(other.v);
		has_resource = other.has_resource;
		other.has_resource = false;
		return *this;
	}
	myVec& operator+=(const myVec& other)
	{
		assert((has_resource));
		for (std::size_t i = 0; i < N; i++)
			v[i] += other.v[i];
		return *this;
	}
	myVec& operator/=(std::size_t size)
	{
		assert((has_resource));
		for (std::size_t i = 0; i < N; i++)
			v[i] /= size;
		return *this;
	}
};
template<std::size_t N> myVec(const double(&list)[N])->myVec<N>;


template<std::size_t N> double dist(const myVec<N>& lhs, const myVec<N>& rhs)
{
	assert((lhs.has_resource && rhs.has_resource));
	double square = 0.0;
	double d;
	for (std::size_t i = 0; i < N; i++)
	{
		d = lhs.v[i] - rhs.v[i];
		square += d * d;
	}
	return std::sqrtf(square);
}

template<std::size_t N> bool operator==(const myVec<N>& lhs, const myVec<N>& rhs)
{
	assert((lhs.has_resource && rhs.has_resource));
	std::size_t i;
	for (i = 0; i < N; i++)
		if (std::abs(lhs.v[i] - rhs.v[i]) >= 10e-4) break;
	return i == N;
}

template<std::size_t N> std::ostream& operator<<(std::ostream& os, const myVec<N>& other)
{
	assert((other.has_resource));
	for (std::size_t i = 0; i < N; i++)
		os << other.v[i] << "\t";
	return os;
}

void test()
{
	constexpr std::size_t dimension = 3U;
	std::vector<myVec<dimension>> data =
	{
		{{ 1.2, 2.3, 3.14 }}, {{ 1.3, 3.01, 4.0 }}, {{ 1.4, 3.1, 3.22 }},
		{{ 1.5, 2.3, 3.64 }}, {{ 1.6, 2.33, 4.2 }}, {{ 1.7, 2.7, 3.76 }},
		{{ 1.8, 2.3, 3.72 }}, {{ 1.88, 3.3, 4.1 }}, {{ 1.9, 3.0, 3.95 }},

		{{ 7.2, 8.1, 19.99 }}, {{ 7, 8.24, 20 }}, {{ 7.54, 7.9, 18.5 }},
		{{ 7.25, 8.1, 19.8 }}, {{ 7, 8.24, 20 }}, {{ 7.77, 7.6, 18.6 }},
		{{ 7.66, 8.2, 19.6 }}, {{ 7, 8.24, 20 }}, {{ 7.52, 7.8, 18.2 }},

		{{ 11.8, 21.7, -34.1 }}, {{ 12.1, 20.7, -33.6 }}, {{ 10.4, 19.4, -33.8 }},
		{{ 11.9, 22.5, -34.2 }}, {{ 12.1, 20.7, -33.6 }}, {{ 10.9, 19.8, -32.9 }},
		{{ 11.2, 22.0, -34.3 }}, {{ 12.2, 20.9, -43.0 }}, {{ 10.5, 19.9, -23.5 }},
	};

	std::function<double(const myVec<dimension>&, const myVec<dimension>&)> _dist = dist<dimension>; // necessary or explicit instantiate the funciton template.

	auto result = RSY_TOOL::K_means::k_means(data, dimension, _dist);
	for (auto const& cluster : result)
	{
		myVec<dimension> v{};
		for (auto const& vec : cluster)
		{
			std::cout << vec << std::endl;
			v += vec;
		}
		v /= cluster.size();
		std::cout << "The mean vector is: " << v << std::endl << std::endl;
	}

	// steal resources
	using RSY_TOOL::K_means::copy_strategy;
	auto result2 = RSY_TOOL::K_means::k_means(data, dimension, _dist, copy_strategy::STEAL);

}


int main()
{
	test();
	system("pause");
	return 0;
}