#pragma once
#ifndef _SEMAPHORE_TYPE_H
#define _SEMAPHORE_TYPE_H
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

		/*
		 *
		 *
		**/
		template<typename _Ty, typename Resource>
		struct SemaphoreType
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
		};


	}//end namespace OS_Experiment

}//end namespace RSY_TOOL

#endif // !_SEMAPHORE_TYPE_H
