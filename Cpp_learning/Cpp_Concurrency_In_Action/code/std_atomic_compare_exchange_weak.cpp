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

std::atomic_flag lock = ATOMIC_FLAG_INIT;


// a simple global linked list:
struct Node { int value; Node* next; };
std::atomic<Node*> list_head(nullptr);

void append(int val)
{
	// append an element to the list
	Node* newNode = new Node{ val, list_head };

	// next is the same as: list_head = newNode, but in a thread-safe way:
	while (!list_head.compare_exchange_weak(newNode->next, newNode)) {}
	// (with newNode->next updated accordingly if some other thread just appended another node)
}
void test()
{
	// spawn 10 threads to fill the linked list:
	std::vector<std::thread> threads;
	for (int i = 0; i < 10; ++i) threads.push_back(std::thread(append, i));
	for (auto& th : threads) th.join();

	// print contents:
	for (Node* it = list_head; it != nullptr; it = it->next)
		std::cout << ' ' << it->value;

	std::cout << std::endl;

	// cleanup:
	Node* it; while (it = list_head) { list_head = it->next; delete it; }

}


int main()
{
	test();
	std::cout << "--------------------------- end ---------------------------" << std::endl;
	static_cast<void>(std::getchar());
	return 0;
}