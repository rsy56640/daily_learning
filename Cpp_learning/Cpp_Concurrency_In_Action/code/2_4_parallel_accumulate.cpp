#include <thread>
#include <iostream>
#include <mutex>
#include <functional>
#include <vector>
#include <numeric>
#include <type_traits>
#include <Windows.h>
#include <random>

template<typename InIt,
	typename T,
	typename = std::enable_if_t<std::is_same_v<T, std::decay_t<decltype(*std::declval<InIt>())>>>
> T parallel_accumulate(InIt first, InIt last, T init)
{
	const std::size_t length = std::distance(first, last);
	if (length == 0) return init;
	constexpr std::size_t data_per_thread = 10000;
	const std::size_t max_threads = (length + data_per_thread - 1) / data_per_thread;
	const std::size_t hardware_threads = std::thread::hardware_concurrency();
	const std::size_t thread_nums = min(max_threads,
		hardware_threads != 0 ? hardware_threads : 2);
	std::cout << "multi-threads: " << thread_nums << std::endl;
	std::vector<T> results(thread_nums);
	std::vector<std::thread> threads(thread_nums);
	InIt block_start = first;
	InIt block_end = block_start;
	const std::size_t block_size = length / thread_nums;
	for (std::size_t i = 0; i < thread_nums - 1; i++)
	{
		std::advance(block_end, block_size);
		threads[i] = std::thread(
			[](InIt first, InIt last, T& val) { val = std::accumulate(first, last, val); },
			block_start, block_end, std::ref(results[i]));
		block_start = block_end;
	}
	threads[thread_nums - 1] = std::thread(
		[](InIt first, InIt last, T& val) { val = std::accumulate(first, last, val); },
		block_start, last, std::ref(results[thread_nums - 1]));
	std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
	return std::accumulate(results.begin(), results.end(), init);
}

int main()
{
	DWORD start, end, time1, time2;

	using T = int;
	constexpr std::size_t size = 200000000;
	std::vector<T> v(size);
	std::srand(time(0));
	for (T& i : v) i = rand();
	std::cout << "data generated" << std::endl;

	T result1, result2;

	start = GetTickCount();
	result1 = std::accumulate<std::vector<T>::iterator, T>(v.begin(), v.end(), 0);
	end = GetTickCount();
	time1 = end - start;

	start = GetTickCount();
	result2 = parallel_accumulate<std::vector<T>::iterator, T>(v.begin(), v.end(), 0);
	end = GetTickCount();
	time2 = end - start;

	std::cout << "result1: " << result1 << ", time: " << time1 << std::endl;
	std::cout << "result2: " << result2 << ", time: " << time2 << std::endl;


	getchar();
	return 0;
}