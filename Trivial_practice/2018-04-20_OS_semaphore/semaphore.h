#pragma once
#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H
#include "semaphoreImpl.h"

namespace RSY_TOOL
{
	namespace OS_Experiment
	{

		template<typename _Ty, typename Resource = void>
		class Semaphore :private SemaphoreType<_Ty, Resource>
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

			using SemaphoreImpl_ptr = std::shared_ptr<SemaphoreImpl<_Ty, Resource> >;

		public:

			Semaphore(_STD size_t size,
				_Func_alloc resource_alloc = [](smartPtr, resource_ptr) {; },
				_Func_free resource_free = [](smartPtr) {; })
				:_semaphoreImpl_ptr{ _STD make_shared<SemaphoreImpl<_Ty, Resource> >
				(size, resource_alloc, resource_free) }
			{}


			/*
			 * copy constructor
			 * the object has the same size, resource_func(s) with the other,
			 * but they manage requests and resources in different pools.
			**/
			Semaphore(const Semaphore& other)
				:_semaphoreImpl_ptr{ _STD make_shared<SemaphoreImpl<_Ty, Resource> >
				(other.size, other.resource_alloc, other.resource_free) }
			{}

			/*
			 * copy assignment
			 * the object has the same size, resource_func(s) with the other,
			 * but they manage requests and resources in different pools.
			**/
			Semaphore& operator=(const Semaphore& other) &
			{
				_semaphoreImpl_ptr.reset(_STD make_shared<SemaphoreImpl<_Ty, Resource> >
					(other.size, other.resource_alloc, other.resource_free));
			}

			/*
			 * move constructor
			 * the object has the same size, resource_func(s) with the other,
			 * and they manage requests and resources in the same pool.
			**/
			Semaphore(Semaphore&& other)
				:_semaphoreImpl_ptr{ other._semaphoreImpl_ptr }
			{}

			/*
			 * move assignment
			 * the object has the same size, resource_func(s) with the other,
			 * and they manage requests and resources in the same pool.
			**/
			Semaphore& operator=(Semaphore&& other) &
			{
				this->_semaphoreImpl_ptr = other._semaphoreImpl_ptr;
			}


			void wait(const smartPtr& ptr, resource_ptr resource = nullptr)
			{
				_semaphoreImpl_ptr->wait(ptr, resource);
			}


			void signal(const smartPtr& ptr = nullptr)
			{
				_semaphoreImpl_ptr->signal(ptr);
			}


		private:

			SemaphoreImpl_ptr _semaphoreImpl_ptr;

		};





		class BoundedBuffer
		{

		public:


		private:


		};



	}//end namespace OS_Experiment

}//end namespace RSY_TOOL

#endif // !_SEMAPHORE_H
