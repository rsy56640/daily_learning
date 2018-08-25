/* http://www.drdobbs.com/exception-safety-analysis/184401728
 * Emulate "Synchronized" keyword in Java,
 *     1. SYNCHRONIZED
 *         SYNCHRONIZED{
 *             // ...
 *         }
 *
 *     2. SYNCHRONIZED_OBJ(obj)
 *         1)  SYNCHRONIZED_OBJ(t){
 *                 // ...
 *             }
 *         2)  void A::foo(){
 *                 SYNCHRONIZED_OBJ(*this){
 *                     // ...
 *                 }
 *             }
 *
 *     3. SYNCHRONIZED_METHOD
 *         void foo() SYNCHRONIZED_METHOD{
 *             // ...
 *         }
 *
 *     4. SYNCHRONIZED_MEMBER_FUNCTION
 *         void A::foo() SYNCHRONIZED_MEMBER_FUNCTION{
 *             // ...
 *         }
 */
#include <iostream>
#include <mutex>
#include <thread>

namespace Sync {

#define INJECT_VARIABLE(type, name) \
	if (bool obfuscatedName = false) ; else \
		for (type name; !obfuscatedName; obfuscatedName = true)


	/*
	 * SYNCHRONIZED
	 */
#define SYNCHRONIZED \
	INJECT_VARIABLE(static std::mutex, obfuscatedMutexName) \
	INJECT_VARIABLE(std::lock_guard<std::mutex>, obfuscatedLockName (obfuscatedMutexName))

	int num1 = 0, num2 = 0, num3 = 0;
	constexpr auto LOOP_TIMES = 50;
	void foo_SYNCHRONIZED(int k)
	{
		std::cout << "Test SYNCHRONIZED in thread " << k << std::endl;
		for (int i = 0; i < LOOP_TIMES; ++i)
		{
			num3++;
			SYNCHRONIZED
			{
				num1++; num2++;
				std::cout << num1 << " " << num2 << " " << num3
					<< " in the thread: " << k << std::endl;
			}
		}
	}



	/*
	 * SYNCHRONIZED_OBJ
	 */
#define SYNCHRONIZED_OBJ(obj) \
	INJECT_VARIABLE(std::lock_guard<std::mutex>,            \
						obfuscatedLockName(Sync::Synchronized_OBJ<decltype(obj)>(obj).GetMutex())) 

	template<class T>
	class Synchronized_OBJ
	{
	public:
		Synchronized_OBJ(T& _Synchronized_obj)
			:Synchronized_obj(_Synchronized_obj)
		{}
		std::mutex& GetMutex() {
			return Synchronized_obj.getMutex();
		}
	private:
		T & Synchronized_obj;
	};

	class Test {
		int i = 0;
		std::mutex mutex_;
	public:
		std::mutex& getMutex() { return mutex_; }
		int operator++(int) {
			int temp = i;
			i++;
			return temp;
		}
		friend std::ostream& operator<<(std::ostream& os, const Test& t) {
			os << t.i;
			return os;
		}
	};
	Test t1, t2;

	void foo_SYNCHRONIZED_OBJ_1(int k)
	{
		std::cout << "Test SYNCHRONIZED_OBJ in thread " << k << std::endl;
		for (int i = 0; i < LOOP_TIMES; i++)
		{
			SYNCHRONIZED_OBJ(t1)
			{
				t1++;
				std::cout << t1 << " in the thread: " << k << std::endl;
			}
		}
	}
	void foo_SYNCHRONIZED_OBJ_2(int k)
	{
		std::cout << "Test SYNCHRONIZED_OBJ in thread " << k << std::endl;
		for (int i = 0; i < LOOP_TIMES; i++)
		{
			SYNCHRONIZED_OBJ(t2)
			{
				t2++;
				std::cout << t2 << " in the thread: " << k << std::endl;
			}
		}
	}

	/*
	 * SYNCHRONIZED_METHOD
	 */
#define SYNCHRONIZED_METHOD \
	try { \
		INJECT_VARIABLE(static std::mutex, obfuscatedMutexName) \
		throw std::move(std::unique_lock<std::mutex>(obfuscatedMutexName)); \
	} catch (std::unique_lock<std::mutex>&)


	void foo_SYNCHRONIZED_METHOD1(int k) SYNCHRONIZED_METHOD
	{
		num1 = 0;
		std::cout << "Test SYNCHRONIZED_METHOD in thread " << k << std::endl;
		for (int i = 0; i < LOOP_TIMES; i++)
		{
			num1++;
			std::cout << num1 << " in the thread: " << k << std::endl;
		}
	}
	void foo_SYNCHRONIZED_METHOD2(int k) SYNCHRONIZED_METHOD
	{
		num2 = 0;
		std::cout << "Test SYNCHRONIZED_METHOD in thread " << k << std::endl;
		for (int i = 0; i < LOOP_TIMES; i++)
		{
			num2++;
			std::cout << num2 << " in the thread: " << k << std::endl;
		}
	}


	/*
	 * SYNCHRONIZED_MEMBER_FUNCTION
	 */
#define SYNCHRONIZED_MEMBER_FUNCTION \
	try { \
		throw std::move(std::unique_lock<std::mutex>(Sync::Synchronized_OBJ<decltype(*this)>(*this).GetMutex())); \
	} catch (std::unique_lock<std::mutex>&)


	class A {
		std::mutex mutex_;
	public:
		std::mutex& getMutex() { return mutex_; }
		void foo_SYNCHRONIZED_MEMBER_FUNCTION(int k) SYNCHRONIZED_MEMBER_FUNCTION
		{
			std::cout << "Test SYNCHRONIZED_MEMBER_FUNCTION in thread " << k << std::endl;
			num3 = 0;
			for (int i = 0; i < LOOP_TIMES; i++)
			{
				num3++;
				std::cout << num3 << " in the thread: " << k << std::endl;
			}
		}
		void foo_SYNCHRONIZED_OBJ(int k) {
			SYNCHRONIZED_OBJ(*this)
			{
				std::cout << "Test SYNCHRONIZED_MEMBER_FUNCTION in thread " << k << std::endl;
				num1 = 0;
				for (int i = 0; i < LOOP_TIMES; i++)
				{
					num1++;
					std::cout << num1 << " in the thread: " << k << std::endl;
				}
			}
		}
	};


}//end namespace Sync;


using namespace Sync;

void test_SYNCHRONIZED() {
	std::thread t1(foo_SYNCHRONIZED, 1);
	std::thread t2(foo_SYNCHRONIZED, 2);
	t1.join();
	t2.join();
}
void test_SYNCHRONIZED_OBJ() {
	std::thread t1(foo_SYNCHRONIZED_OBJ_1, 1);
	std::thread t2(foo_SYNCHRONIZED_OBJ_1, 2);
	std::thread t3(foo_SYNCHRONIZED_OBJ_2, 3);
	std::thread t4(foo_SYNCHRONIZED_OBJ_2, 4);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
}
void test_SYNCHRONIZED_METHOD() {
	std::thread t1(foo_SYNCHRONIZED_METHOD1, 1);
	std::thread t2(foo_SYNCHRONIZED_METHOD1, 2);
	std::thread t3(foo_SYNCHRONIZED_METHOD2, 3);
	std::thread t4(foo_SYNCHRONIZED_METHOD2, 4);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
}
void test_SYNCHRONIZED_MEMBER_FUNCTION() {
	A a1, a2;
	auto const ____1 = [](A& a, int k) { a.foo_SYNCHRONIZED_MEMBER_FUNCTION(k); };
	auto const ____2 = [](A& a, int k) { a.foo_SYNCHRONIZED_OBJ(k); };
	std::thread t1(____1, std::ref(a1), 1);
	std::thread t2(____1, std::ref(a1), 2);
	std::thread t3(____1, std::ref(a2), 3);
	std::thread t4(____1, std::ref(a2), 4);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
}

int main()
{
	//test_SYNCHRONIZED();
	//test_SYNCHRONIZED_OBJ();
	//test_SYNCHRONIZED_METHOD();
	//test_SYNCHRONIZED_MEMBER_FUNCTION();

	getchar();
	return 0;
}