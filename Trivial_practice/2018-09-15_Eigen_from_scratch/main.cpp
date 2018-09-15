#include <iostream>
#include <vector>
#include <limits>
#include <Eigen/Dense>
using Eigen::MatrixXd;
namespace eigen = Eigen;


namespace Scalar
{
	struct myScalar
	{
		double d;
		myScalar() :d(0.0) {}
		operator double()
		{
			return d;
		}
		myScalar(double _d) :d(_d) {}
		myScalar(const myScalar& other) :d(other.d) {}
		myScalar& operator=(const myScalar& other)
		{
			d = other.d;
			return *this;
		}
		myScalar& operator+=(const myScalar& other)
		{
			d += other.d;
			return *this;
		}
		myScalar& operator-=(const myScalar& other)
		{
			d -= other.d;
			return *this;
		}
		myScalar& operator*=(const myScalar& other)
		{
			d *= other.d;
			return *this;
		}
		myScalar& operator/=(const myScalar& other)
		{
			d /= other.d;
			return *this;
		}
	};


	bool operator,(const myScalar& lhs, const myScalar& rhs)
	{
		return  lhs.d < rhs.d;
	}

	std::ostream& operator<<(std::ostream& os, const myScalar& scalar)
	{
		os << scalar.d;
		return os;
	}

	myScalar operator+(const myScalar& lhs, const myScalar& rhs)
	{
		return myScalar{ lhs } += rhs;
	}
	myScalar operator-(const myScalar& lhs, const myScalar& rhs)
	{
		return myScalar{ lhs } -= rhs;
	}
	myScalar operator*(const myScalar& lhs, const myScalar& rhs)
	{
		return myScalar{ lhs } *= rhs;
	}
	myScalar operator/(const myScalar& lhs, const myScalar& rhs)
	{
		return myScalar{ lhs } /= rhs;
	}


}

// specialization for user defined scalar-type.
namespace Eigen
{
	template<> class NumTraits<Scalar::myScalar> :public GenericNumTraits<Scalar::myScalar>
	{
	public:
		using Real = double;
		using NonInteger = Scalar::myScalar;
		using Literal = Scalar::myScalar;
		using Nested = Scalar::myScalar;
		static constexpr int IsComplex = 0;
		static constexpr int IsInteger = 0;
		static constexpr int IsSigned = 1;
		static const Real epsilon()
		{
			return 10e-6;
		}
		static const Real highest()
		{
			return std::numeric_limits<double>::max();
		}
		static const Real lowest()
		{
			return std::numeric_limits<double>::min();
		}
	};
}


void test()
{

	// dynamic - sizeable
	Eigen::MatrixXd d(2, 2);
	d(0, 0) = 3;
	d(1, 0) = 2.5;
	d(0, 1) = -1;
	d(1, 1) = d(1, 0) + d(0, 1);
	std::cout << "matrix d:" << std::endl;
	std::cout << d << std::endl << std::endl;

	auto d_T = d.transpose();
	std::cout << "d transpose:" << std::endl;
	std::cout << d_T << std::endl << std::endl;

	auto d2 = d * d;
	std::cout << "multiply d and d:" << std::endl;
	std::cout << d2 << std::endl << std::endl;

	auto d3 = d + d_T;
	std::cout << "add d and d_T:" << std::endl;
	std::cout << d3 << std::endl << std::endl;


	// fixed size matrix, only for small fixed matrix.
	Eigen::Matrix3d f;

	f = Eigen::Matrix3d::Random(); // random init
	std::cout << "random matrix3d:" << std::endl;
	std::cout << f << std::endl << std::endl;

	f = Eigen::Matrix3d::Constant(1.7);
	std::cout << "constant matrix3d:" << std::endl;
	std::cout << f << std::endl << std::endl;

	f << 1, 2, 3,
		4, 5, 6,
		7, 8, 9;
	std::cout << "input matrix3d:" << std::endl;
	std::cout << f << std::endl << std::endl;


	// Use dynamic matrix for large fixed matrix.
	Eigen::Matrix<double, 3, 3>(); // The same as Eigen::Matrix3d
	eigen::Matrix<double, eigen::Dynamic, eigen::Dynamic>(); // The same as Eigen::MatrixXd

	eigen::VectorXd v(2);
	v << 1, 3;
	std::cout << "inner product:" << std::endl;
	std::cout << v.transpose() * v << std::endl << std::endl;

	eigen::Vector3d v2(1, 2, 3);
	eigen::Vector3d v3 = f * v2;
	std::cout << "matrix multipy vector:" << std::endl;
	std::cout << v3 << std::endl << std::endl;


	// use customized scalar.
	using namespace Scalar;
	eigen::Matrix<myScalar, 2, 2> mm;
	mm << 1, 2,
		3, 3;
	eigen::Matrix<myScalar, 2, 1> mm2;
	mm2 << 6, 7;
	std::cout << "use defined scalar type:" << std::endl;
	std::cout << mm * mm2 << std::endl << std::endl;

}

int main()
{
	test();
	system("pause");
	return 0;
}