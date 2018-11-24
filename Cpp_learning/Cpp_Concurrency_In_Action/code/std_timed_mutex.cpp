#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <future>
#include <chrono>
#include <vector>
#include <functional>
#include <numeric>
#include <type_traits>
#include <random>
#include <fstream>
#include <sstream>
#include <map>
#include <unordered_map>

using namespace std::literals::chrono_literals;

inline std::string get_id_string(const std::thread::id tid)
{
	std::ostringstream os;
	os << tid;
	return os.str();
}


constexpr std::size_t MAXNUM = 2500;
constexpr std::size_t NUM_THREADS = 10;
volatile int counter1 = 0;
volatile int counter2 = 0;
int counter3 = 0;

void foo()
{
	static std::timed_mutex tmx;
	for (std::size_t i = 0; i < MAXNUM; i++) {
		if (tmx.try_lock_for(100ns)) {
			counter1++;
			std::this_thread::sleep_for(5ns);
			tmx.unlock();
		}
	}
}

void goo()
{
	static std::mutex mx;
	for (std::size_t i = 0; i < MAXNUM; i++) {
		if (mx.try_lock()) {
			counter2++;
			std::this_thread::sleep_for(5ns);
			mx.unlock();
		}
	}
}

void hoo()
{
	static std::mutex mx;
	for (std::size_t i = 0; i < MAXNUM; i++) {
		mx.lock();
		counter3++;
		mx.unlock();
	}
}

static std::timed_mutex tmx;

void test()
{
	tmx.lock();
	std::cout << "in test(): sleep start for 5s" << std::endl;
	std::this_thread::sleep_for(5s);
	std::cout << "in test(): sleep end" << std::endl;
	tmx.unlock();
}


int main()
{
	std::vector<std::thread> ts1(NUM_THREADS);
	for (auto& t : ts1) t = std::thread(foo);

	std::vector<std::thread> ts2(NUM_THREADS);
	for (auto& t : ts2) t = std::thread(goo);

	std::vector<std::thread> ts3(NUM_THREADS);
	for (auto& t : ts3) t = std::thread(hoo);

	for (auto& t : ts1) t.join();
	for (auto& t : ts2) t.join();
	for (auto& t : ts3) t.join();

	std::cout << counter1 << " increaments for counter1" << std::endl;
	std::cout << counter2 << " increaments for counter2" << std::endl;
	std::cout << counter3 << " increaments for counter3" << std::endl;

	std::cout << "\n-----------------\n" << std::endl;

	std::thread t(test);
	std::this_thread::sleep_for(1s);
	std::cout << "in main(): try to get lock from now" << std::endl;
	auto start = std::chrono::system_clock::now();
	if (tmx.try_lock_for(4500ms))
	{
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> diff = end - start;
		std::cout << "in main(): try to get lock time: " << diff.count() << std::endl;
		tmx.unlock();
	}
	else std::cout << "in main(): fuck without lock" << std::endl;
	t.join();

	std::cout << "end" << std::endl;

	getchar();
	return 0;
}