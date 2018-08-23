/* http://www.drdobbs.com/exception-safety-analysis/184401728
 *
 */
#include <iostream>
#include <mutex>
#include <thread>

#define INJECT_VARIABLE(type, name) \
   if (bool obfuscatedName = false) ; else \
      for (type name; !obfuscatedName; obfuscatedName = true)

#define SYNCHRONIZED \
   INJECT_VARIABLE(static std::mutex, obfuscatedMutexName) \
   INJECT_VARIABLE(std::lock_guard<std::mutex>, obfuscatedLockName (obfuscatedMutexName))

int num1 = 0, num2 = 0, num3 = 0;
void foo(int k)
{
	for (int i = 0; i < 50; ++i)
	{
		num3++;
		SYNCHRONIZED
		{
			num1++; num2++;
			std::cout << num1 << " " << num2 << " " << num3 << " in the thread: " << k << std::endl;
		}
	}
}

int main()
{
	std::thread t1(foo, 1);
	std::thread t2(foo, 2);
	t1.join();
	t2.join();

	getchar();
	return 0;
}