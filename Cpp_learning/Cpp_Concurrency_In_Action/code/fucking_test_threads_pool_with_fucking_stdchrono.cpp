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
	void stop() { std::call_once(_stop_flag, [this]() { _prom.set_value(); }); }
	void start() { std::call_once(_start_flag, [this]() { std::thread(std::mem_fn(&ThreadsPool<Ret>::schedule), this).detach(); }); }
private:
	std::thread _threads_pool[3];
	const schedule_policy _schedule_policy;
	std::once_flag _start_flag;
	std::once_flag _stop_flag;
	std::promise<void> _prom;
	std::mutex _mtx;
	std::condition_variable _cv;
	std::list<std::size_t> _threads_available = { 0, 1, 2 };
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
	} // end function void schedule_once();
};



int single_thread(std::vector<std::vector<int>> nums)
{
	int sum = 0;
	for (auto const& vec : nums)
		for (auto const& n : vec)
			sum += n;
	return sum;
}

int many_threads(std::vector<std::vector<int>> nums)
{
	constexpr auto num_threads = 4;
	std::thread threads[num_threads];
	int results[num_threads];
	int count = 0;
	int sum = 0;
	for (auto const& vec : nums)
	{
		threads[count] = std::thread([count, results, &vec]() mutable { results[count] = std::accumulate(vec.begin(), vec.end(), 0); });
		count++;
		if (count == num_threads)
		{
			count = 0;
			for (int i : results) sum += i;
			for (auto& t : threads) t.join();
		}
	}
	for (int i = 0; i < count; i++)
	{
		threads[i].join();
		sum += results[i];
	}
	return sum;
}

int threads_pool(std::vector<std::vector<int>> nums)
{
	ThreadsPool<int> pool;
	pool.start();
	std::deque<std::future<int>> futures;
	for (auto const& vec : nums)
		futures.push_back(pool.register_for_execution(
			std::packaged_task<int()>{[&vec]()
	{ return std::accumulate(vec.begin(), vec.end(), 0); }}));
	int sum = 0;
	for (auto& f : futures)
		sum += f.get();
	pool.stop();
	return sum;
}

int sync_threads(std::vector<std::vector<int>> nums)
{
	std::deque<std::future<int>> dq;
	for (std::vector<int> const& vec : nums)
		dq.push_back(
			std::async(std::accumulate<std::vector<int>::const_iterator, int>,
				vec.begin(), vec.end(), 0));
	int sum = 0;
	for (auto& f : dq)
		sum += f.get();
	return sum;
}


void test()
{
	std::vector<std::vector<int>> nums1;
	std::vector<int> vec;
	for (int i = 0; i < 8; i++)
	{
		vec.clear();
		for (int j = 0; j < 6000000; j++)
			vec.push_back(rand());
		nums1.push_back(std::move(vec));
	}
	auto nums2 = nums1;
	auto nums3 = nums1;
	auto nums4 = nums1;

	auto start1 = std::chrono::steady_clock::now();
	int res1 = single_thread(std::move(nums1));
	auto end1 = std::chrono::steady_clock::now();
	std::cout << "single_thread: " << res1 << ", time: " << std::chrono::duration<double>(end1 - start1).count() << std::endl;

	auto start2 = std::chrono::steady_clock::now();
	int res2 = many_threads(std::move(nums2));
	auto end2 = std::chrono::steady_clock::now();
	std::cout << "many_threads:  " << res2 << ", time: " << std::chrono::duration<double>(end2 - start2).count() << std::endl;

	auto start3 = std::chrono::steady_clock::now();
	int res3 = threads_pool(std::move(nums3));
	auto end3 = std::chrono::steady_clock::now();
	std::cout << "threads_pool:  " << res3 << ", time: " << std::chrono::duration<double>(end3 - start3).count() << std::endl;

	auto start4 = std::chrono::steady_clock::now();
	int res4 = sync_threads(std::move(nums4));
	auto end4 = std::chrono::steady_clock::now();
	std::cout << "sync_threads:  " << res4 << ", time: " << std::chrono::duration<double>(end4 - start4).count() << std::endl;
}


int main()
{
	test();
	std::cout << "--------------------------- end ---------------------------" << std::endl;
	std::getchar();
	return 0;
}