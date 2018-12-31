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


// parallel tasks
template<typename Ret> class ThreadsPool
{
public:
	enum class schedule_policy { FIFO, LIFO };
public:
	ThreadsPool(schedule_policy policy = schedule_policy::FIFO) :_schedule_policy(policy) {}
	ThreadsPool(const ThreadsPool&) = delete;
	ThreadsPool& operator=(const ThreadsPool&) = delete;
	// return immediately !!!
	[[nodiscard]] std::future<Ret> register_for_execution(std::packaged_task<Ret()> task)
	{
		std::lock_guard<std::mutex> lg{ _mtx };
		std::future<Ret> fut = task.get_future();
		_tasks.push_back(std::move(task));
		return fut;
	}
	void stop()
	{
		std::call_once(_stop_flag,
			[this]()
		{
			_prom.set_value();
			// wait until all treads has accomplished, since this obj may be desturcted.
			std::unique_lock<std::mutex> ulk{ _mtx };
			while (num_threads != max_threads)
				_cv.wait(ulk);
		});
	}
	void start() { std::call_once(_start_flag, [this]() { std::thread(std::mem_fn(&ThreadsPool<Ret>::schedule), this).detach(); }); }
private:
	static constexpr int max_threads = 4;
	int num_threads = max_threads;
	const schedule_policy _schedule_policy;
	std::once_flag _start_flag;
	std::once_flag _stop_flag;
	std::promise<void> _prom;
	std::mutex _mtx;
	std::condition_variable _cv;
	std::list<std::packaged_task<Ret()>> _tasks;

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
				if (num_threads > 0 && !_tasks.empty())
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
		while (num_threads == 0)
			_cv.wait(ulk);
		num_threads--;
		std::packaged_task<Ret()> task;
		switch (_schedule_policy)
		{
		case schedule_policy::FIFO:
			task = std::move(_tasks.front());
			_tasks.pop_front();
			break;
		case schedule_policy::LIFO:
			task = std::move(_tasks.back());
			_tasks.pop_back();
			break;
		default:
			break;
		}

		ulk.unlock();

		std::thread([this, task{ std::move(task) }]() mutable
		{
			// execute task
			task();
			std::lock_guard<std::mutex> lg{ _mtx };
			num_threads++;
			_cv.notify_one();
		}).detach();
	} // end function void schedule_once();
};


int main()
{
	return 0;
}
