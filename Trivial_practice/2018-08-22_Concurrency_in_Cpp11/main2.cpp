/* https://blog.poxiao.me/p/spinlock-implementation-in-cpp11/
 *
 */
#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic>
using namespace std;

class spin_mutex {
	std::atomic<bool> flag = ATOMIC_VAR_INIT(false);
public:
	spin_mutex() = default;
	spin_mutex(const spin_mutex&) = delete;
	spin_mutex& operator= (const spin_mutex&) = delete;
	void lock()
	{
		bool expected = false;
		while (!flag.compare_exchange_strong(expected, true))
			expected = false;
	}
	void unlock()
	{
		flag.store(false);
	}
};


int num = 0;
spin_mutex sm;

void thread_proc()
{
	for (int i = 0; i < 100; ++i)
	{
		sm.lock();
		++num;
		sm.unlock();
	}
}

int main()
{
	std::thread td1(thread_proc), td2(thread_proc);
	td1.join();
	td2.join();
	cout << num << endl;

	getchar();
	return 0;
}
