#include <iostream>
#include <thread>
#include <mutex>
#include <shared_mutex>
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
using dns_entry = std::string;
namespace  boost = std;
class dns_cache
{
	std::map<std::string, dns_entry> entries;
	mutable boost::shared_mutex entry_mutex;

public:

	dns_entry find_entry(std::string const& domain) const
	{
		boost::shared_lock<boost::shared_mutex> lk(entry_mutex); //
		std::map<std::string, dns_entry>::const_iterator const it =
			entries.find(domain);
		return (it == entries.end()) ? dns_entry{} : it->second;
	}

	void update_or_add_entry(std::string const& domain, dns_entry const& dns_details)
	{
		std::lock_guard<boost::shared_mutex> lk(entry_mutex); // 2
		entries[domain] = dns_details;
	}
};

int main()
{
	std::getchar();
	return 0;
}
