#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <functional>
#include <type_traits>
#include <optional>

template<typename T> class thread_safe_queue
{
public:
	using value_type = T;
	using size_type = std::size_t;
public:
	thread_safe_queue() = default;
	thread_safe_queue(const thread_safe_queue&) = delete;
	thread_safe_queue& operator=(const thread_safe_queue&) = delete;
	bool empty() const
	{
		std::lock_guard<std::mutex> lg{ _mtx };
		return _dq.empty();
	}
	size_type size() const
	{
		std::lock_guard<std::mutex> lg{ _mtx };
		return _dq.size();
	}
	void push(T value)
	{
		std::lock_guard<std::mutex> lg{ _mtx };
		_dq.push_back(std::move(value));
		_cv.notify_one();
	}
	std::optional<T> front()
	{
		std::lock_guard<std::mutex> lg{ _mtx };
		if (!_dq.empty())
			return _dq.front();
		return std::nullopt;
	}
	std::optional<T> try_pop()
	{
		std::lock_guard<std::mutex> lg{ _mtx };
		if (!_dq.empty())
		{
			T front = _dq.front();
			_dq.pop_front();
			return front;
		}
		return std::nullopt;
	}
	T wait_and_pop()
	{
		std::unique_lock<std::mutex> ulk{ _mtx };
		while (_dq.empty())
			_cv.wait(ulk);
		T front = _dq.front();
		_dq.pop_front();
		return front; // since guaranteed copy elision, the cost is trivial, otherwise maybe unlock before.
	}
private:
	std::deque<T> _dq;
	mutable std::mutex _mtx;
	std::condition_variable _cv;
};

thread_safe_queue<int> q;
std::mutex io_mtx;
void producer1()
{
	for (int i = 0; i < 1000; i++)
	{
		int r = q.wait_and_pop();
		int size = q.size();
		std::lock_guard<std::mutex> lg{ io_mtx };
		std::cout << "In producer1, size: " << size << "produce value: " << r << std::endl;
	}
}
void producer2()
{
	for (int i = 0; i < 1000; i++)
	{
		int r1 = q.wait_and_pop();
		int r2 = q.wait_and_pop();
		int size = q.size();
		std::lock_guard<std::mutex> lg{ io_mtx };
		std::cout << "In producer2, size: " << size << "produce value: " << r1 << " " << r2 << std::endl;
	}
}
void consumer()
{
	for (int i = 0; i < 1000; i++)
	{
		q.push(1);
		q.push(2);
		q.push(3);
	}
}

void test()
{
	std::vector<std::thread> threads(3);
	threads[0] = std::thread(producer1);
	threads[1] = std::thread(producer2);
	threads[2] = std::thread(consumer);
	for (auto& thread : threads)
		thread.join();
	std::cout << "end" << std::endl;
}


int main()
{
	test();
	std::getchar();
	return 0;
}
