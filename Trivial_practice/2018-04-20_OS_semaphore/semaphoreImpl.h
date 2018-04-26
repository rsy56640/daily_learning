#pragma once
#ifndef _SEMAPHOREIMPL_H
#define _SEMAPHOREIMPL_H
#include "semaphore_type.h"
#ifndef _STD
#define _STD ::std::
#endif // !_STD

namespace RSY_TOOL
{
	namespace OS_Experiment
	{

		//Template class Semaphore
		template<typename _Ty, typename Resource>
		class SemaphoreImpl :private SemaphoreType<_Ty, Resource>
		{

			using id = typename SemaphoreType<_Ty, Resource>::id;
			using value_type = typename SemaphoreType<_Ty, Resource>::value_type;
			using resource_type = typename SemaphoreType<_Ty, Resource>::resource_type;
			using pointer_type = typename SemaphoreType<_Ty, Resource>::pointer_type;
			using smartPtr = typename SemaphoreType<_Ty, Resource>::smartPtr;
			using resource_ptr = typename SemaphoreType<_Ty, Resource>::resource_ptr;
			using running_queue = typename SemaphoreType<_Ty, Resource>::running_queue;
			using running_id_queue = typename SemaphoreType<_Ty, Resource>::running_id_queue;
			using waiting_queue = typename SemaphoreType<_Ty, Resource>::waiting_queue;
			using _Func_alloc = typename SemaphoreType<_Ty, Resource>::_Func_alloc;
			using _Func_free = typename SemaphoreType<_Ty, Resource>::_Func_free;


		public:

			SemaphoreImpl
			(_STD size_t size, _Func_alloc resource_alloc, _Func_free resource_free)
				:_size{ size },
				_id{ 0 },
				_res_alloc{ resource_alloc },
				_res_free{ resource_free }
			{}

			SemaphoreImpl(const SemaphoreImpl&) = default;
			SemaphoreImpl& operator=(const SemaphoreImpl&) & = default;
			SemaphoreImpl(SemaphoreImpl&&) = delete;
			SemaphoreImpl& operator=(SemaphoreImpl&&) = delete;


			void wait(const smartPtr& ptr, resource_ptr resource)
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


			void signal(const smartPtr& ptr)
			{

				if (running_list.empty())return;


				//running_id_list is for this !?!?!
				//why not use KD-Tree !?!?!
				if (ptr == nullptr)
				{
					auto it = running_id_list.begin();
					smartPtr _smartPtr = it->second;
					running_id_list.erase(it);
					running_list.erase(running_list.find(_smartPtr));
					_res_free(_smartPtr);
					return;
				}


				auto it = running_list.find(ptr);
				if (it != running_list.end())
				{

					smartPtr _smartPtr = it->first;
					id tmpid = it->second;
					running_list.erase(it);
					running_id_list.erase(running_id_list.find(tmpid));
					_res_free(_smartPtr);

					if (!waiting_list.empty())
					{
						_STD pair<smartPtr, resource_ptr> ready = waiting_list.front();
						waiting_list.pop();
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


	}//end namespace OS_Experiment

}//end namespace RSY_TOOL

#endif // !_SEMAPHOREIMPL_H
