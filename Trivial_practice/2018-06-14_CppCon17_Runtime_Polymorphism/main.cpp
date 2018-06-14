#include <iostream>
#include <string>
#include <functional>
#include <vector>
using namespace std;

struct vtable
{
	void(*accelerate)(void* this_);
	void*(*clone_)(void* this_);
	void(*delete_)(void* this_);
};

template<typename T>
vtable const vtable_for = vtable{
   [](void* this_) {
	   (static_cast<T*>(this_))->accelerate();
   },

   [](void* this_)->void* {
	   return new T{*static_cast<T*>(this_)};
   },

   [](void* this_) {
	   if (nullptr != static_cast<T*>(this_))
	   {
		 delete static_cast<T*>(this_);
	   }
   }
};


class Vehicle
{
public:
	template<typename T>
	Vehicle(const T& vehicle)
		:vptr_{ &vtable_for<T> }, ptr_{ new T(vehicle) } {}
	Vehicle(const Vehicle& other)
		:vptr_(other.vptr_), ptr_(vptr_->clone_(other.ptr_))
	{}
	Vehicle(Vehicle&& other)
		: vptr_(other.vptr_), ptr_(other.ptr_)
	{
		other.ptr_ = nullptr;
	}
	void accelerate()
	{
		vptr_->accelerate(ptr_);
	}
	~Vehicle()
	{
		vptr_->delete_(ptr_);
	}
private:
	vtable const* const vptr_;
	void* ptr_;
};


class Car {
public:
	void accelerate() { cout << "car accelerate" << endl; }
};
class Train {
public:
	void accelerate() { cout << "train accelerate" << endl; }
};
class Plain {
public:
	void accelerate() { cout << "plain accelerate" << endl; }
};


int main()
{
	Car c;
	Vehicle car(c);
	Vehicle t(Train{});

	std::vector<Vehicle> v;

	v.push_back(Vehicle{ c });
	v.push_back(c);
	v.push_back(_STD move(t));
	v.push_back(t);
	v.push_back(Plain{});

	for (auto& vehicle : v)
		vehicle.accelerate();

	system("pause");
	return 0;
}
