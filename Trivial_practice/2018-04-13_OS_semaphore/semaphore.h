#pragma once
#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H
#include <queue>
#include <memory>
#include <functional>
#include <type_traits>
#include <map>
#ifndef _STD
#define _STD ::std::
#endif // !_STD

namespace RSY_TOOL
{
	namespace OS_Experiment
	{

		//Template class Semaphore
		template<typename _Ty, typename Resource = void> class Semaphore
		{

			using id = _STD size_t;
			using value_type = typename _STD remove_reference<_Ty>::type;
			using resource_type = typename _STD remove_reference<Resource>::type;
			using pointer_type = value_type*;
			using smartPtr = typename _STD shared_ptr<_Ty>;
			using resource_ptr = typename _STD shared_ptr<Resource>;
			using running_queue = typename _STD map<smartPtr, id>;
			using running_id_queue = typename _STD map<id, smartPtr>;
			using waiting_queue = typename _STD queue<_STD pair<smartPtr, resource_ptr> >;
			using _Func_alloc = typename _STD function<void(smartPtr, resource_ptr)>;
			using _Func_free = typename _STD function<void(smartPtr)>;


		public:

			Semaphore(_STD size_t size,
				_Func_alloc resource_alloc = [](smartPtr, resource_ptr) {; },
				_Func_free resource_free = [](smartPtr) {; })
				:_size{ size },
				_id{ 0 },
				_res_alloc{ resource_alloc },
				_res_free{ resource_free }
			{}


			void wait(const smartPtr& ptr, resource_ptr resource = nullptr)
			{

				if (running_list.size() == _size)
					waiting_list.push(_STD make_pair(ptr, resource));

				else
				{
					_res_alloc(ptr, resource);
					running_list.insert(_STD make_pair(ptr, _id));
					running_id_list.insert(_STD make_pair(_id, ptr));
					_id++;
				}

			}


			void signal(const smartPtr& ptr = nullptr)
			{

				if (running_list.empty())return;


				//running_id_list is for this !?!?!
				//why not use KD-Tree !?!?!
				if (ptr == nullptr)
				{
					auto it = running_id_list.begin();
					running_id_list.erase(it);
					running_list.erase(running_list.find(it->second));
					_res_free(it->second);
					return;
				}


				auto it = running_list.find(ptr);
				if (it != running_list.end())
				{

					running_list.erase(it);
					running_id_list.erase(running_id_list.find(it->second));
					_res_free(it->first);

					if (!waiting_list.empty())
					{
						auto ready = waiting_list.front(); waiting_list.pop();
						_res_alloc(ready.first, ready.second);
						running_list.insert(_STD make_pair(ready.first, _id));
						running_id_list.insert(_STD make_pair(_id, ready.first));
						_id++;
					}

				}

			}


		private:

			const _STD size_t _size;

			id _id;

			running_queue running_list;

			running_id_queue running_id_list;

			waiting_queue waiting_list;

			const _Func_alloc _res_alloc;

			const _Func_free _res_free;

		};


		class BoundedBuffer
		{

		public:


		};


	}//end namespace OS_Experiment

}//end namespace RSY_TOOL

#endif // !_SEMAPHORE_H
