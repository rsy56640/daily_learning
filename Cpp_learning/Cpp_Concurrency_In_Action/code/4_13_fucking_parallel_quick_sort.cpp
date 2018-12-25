#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <deque>
#include <functional>
#include <numeric>
#include <type_traits>
#include <set>
#include <map>
#include <unordered_map>
#include <random>
#include <optional>
#include <thread>
#include <future>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <condition_variable>
#include <chrono>

using namespace std::literals::chrono_literals;

inline int get_id(const std::thread::id tid)
{
	std::ostringstream os;
	os << tid;
	return std::stoi(os.str());
}


/***************************************************************************/



template<typename T> std::list<T> sequential_quick_sort(std::list<T> input)
{
	if (input.empty()) return input;
	std::list<T> result;
	result.splice(result.begin(), input, input.begin());
	T const& pivot = *result.begin();
	auto divide_point = std::partition(input.begin(), input.end(),
		[&](T const& t) { return t < pivot; });
	std::list<T> lower_part;
	lower_part.splice(lower_part.end(), input, input.begin(), divide_point);
	auto new_lower = sequential_quick_sort(std::move(lower_part));
	auto new_higher = sequential_quick_sort(std::move(input));
	result.splice(result.end(), new_higher);
	result.splice(result.begin(), new_lower);
	return result;
}


// parallel tasks
template<typename Ret> class ThreadsPool
{
public:
	ThreadsPool() = default;
	ThreadsPool(const ThreadsPool&) = delete;
	ThreadsPool& operator=(const ThreadsPool&) = delete;
	// return immediately
	void register_for_execution(std::packaged_task<Ret()>&& task, std::size_t task_id)
	{
		std::lock_guard<std::mutex> lg{ _mtx };
		_tasks.push_back(std::pair<std::packaged_task<Ret()>, int>(std::move(task), task_id));
	}
	void stop() { _prom.set_value(); }
	void start() { std::call_once(_flag, [this]() { std::thread(std::mem_fn(&ThreadsPool<Ret>::schedule), this).detach(); }); }
private:
	std::thread _threads_pool[2];
	std::once_flag _flag;
	std::mutex _mtx;
	std::condition_variable _cv;
	std::list<std::size_t> _threads_available = { 0, 1 };
	std::list<std::pair<std::packaged_task<Ret()>, int>> _tasks;
	std::promise<void> _prom;

	void schedule()
	{
		std::future<void> f = _prom.get_future();
		while (true)
		{
			if (f.wait_for(50ns) == std::future_status::ready)
				break;

			bool schedule = false;
			{
				std::lock_guard<std::mutex> lg{ _mtx };
				if (!_tasks.empty() && !_threads_available.empty())
					schedule = true;
			}
			if (schedule) schedule_once();
		}
	} // end function void schedule();

	void schedule_once()
	{
		std::size_t tid;
		// consume one thread
		std::unique_lock<std::mutex> ulk{ _mtx };
		while (_threads_available.empty())
			_cv.wait(ulk);
		tid = _threads_available.back();
		_threads_available.pop_back();
		std::packaged_task<Ret()> task = std::move(_tasks.back().first);
		_tasks.pop_back();
		_threads_pool[tid] = std::thread([this, tid, task{ std::move(task) }]() mutable
		{
			// execute task
			task();
			// release that thread
			std::lock_guard<std::mutex> lg{ this->_mtx };
			// before %tid is released, thread%tid has been detached.
			_threads_available.push_back(tid);
			_cv.notify_one();
		});
		_threads_pool[tid].detach();
		// RAII unlock
	} // end function void schedule_once();
};

ThreadsPool<std::list<int>> pool;

template<typename T> std::list<T> parallel_quick_sort(std::list<T> input)
{
	if (input.empty()) return input;
	std::list<T> result;
	result.splice(result.begin(), input, input.begin());
	T const& pivot = *result.begin();
	auto divide_point = std::partition(input.begin(), input.end(),
		[&](T const& t) { return t < pivot; });

	std::list<T> lower_part;
	lower_part.splice(lower_part.end(), input, input.begin(), divide_point);
	/*
	std::packaged_task<std::list<T>()> task(
		[lower_part{ std::move(lower_part) }, task_id]() mutable
	{
		return ::parallel_quick_sort<T>(std::move(lower_part), (task_id << 1) + 1);
	});
	*/
	std::future<std::list<T>> new_lower = //task.get_future();
		//std::async(std::launch::deferred, parallel_quick_sort<T>, std::move(lower_part));
		std::async(parallel_quick_sort<T>, std::move(lower_part));

	// return immediately !!!!!
	//pool.register_for_execution(std::move(task), (task_id << 1) + 1);

	std::list<T> new_higher = parallel_quick_sort(std::move(input));

	//pool.start();

	result.splice(result.end(), new_higher);
	result.splice(result.begin(), new_lower.get());
	return result;
}


void test()
{
	std::list<int> l1;
	std::list<int> l2;
	for (int i = 0; i < 100000; i++)
	{
		int k = rand();
		l1.push_back(k);
		l2.push_back(k);
	}

	std::chrono::system_clock::time_point start, end;

	start = std::chrono::system_clock::now();
	l1 = sequential_quick_sort(std::move(l1));
	end = std::chrono::system_clock::now();
	std::cout << (end - start).count() << std::endl;

	start = std::chrono::system_clock::now();
	l2 = parallel_quick_sort<int>(std::move(l2));
	end = std::chrono::system_clock::now();
	std::cout << (end - start).count() << std::endl;
	//pool.stop();
}


int main()
{
	test();
	std::cout << "--------------------------- end ---------------------------" << std::endl;
	std::getchar();
	return 0;
}