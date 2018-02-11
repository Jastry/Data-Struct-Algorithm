#ifndef __ALLOC_H__
#define __ALLOC_H__

#include <iostream>
#include <stdarg.h>
#include <cstdlib>

namespace SpatialConfigurator {
	
	typedef void(*HANDLE_FUNC)();

	template <int inst>/*一级空间配置器*/
	class __MallocAllocTemplate {
	public:
		static void* OOM_Malloc(size_t size)
		{
			void *result;

			for (;;) {
				if (NULL == _handle) {
					throw std::bad_alloc();
				}
				_handle();
				result = malloc(size);
				if (result)
					return result;
			}
		}

		static void* Allocate(size_t size)
		{
			void *result = malloc(size);
			if (NULL == result)
				result = OOM_Malloc(size);
			return result;
		}

		static void Deallocate(void* p, size_t)
		{
			free(p);
		}

		static HANDLE_FUNC SetMallocHandler(HANDLE_FUNC f)
		{
			HANDLE_FUNC old = f;
			_handle = f;
			return old;
		}
	private:
		static HANDLE_FUNC _handle;
	};
	template <int inst>
	HANDLE_FUNC __MallocAllocTemplate<inst>::_handle = 0;

	
	template <bool threads, int inst>
	class __DefaultAllocTemplate
	{

	public:
		/*****************************************
		*		申请大块儿内存，分三种情况
		*	1. 内存池足够，从内存池里取
		*	2. 内存池不够，但能取小于 nobjs 个内存块
		*	3. 内存池连一个都取不出来
		*****************************************/
		static char* ChunkAlloc(size_t size, size_t& nobjs)
		{
			char *result;
			size_t total_bytes = size * nobjs;
			size_t left_bytes = _end_free - _start_free;

			if (left_bytes >= total_bytes) {
				result = _start_free;
				_start_free += total_bytes;
				return (result);
			}
			else if (left_bytes >= size) {
				nobjs = (int)(left_bytes / size);
				total_bytes = nobjs * size;
				result = _start_free + total_bytes;
				_start_free += total_bytes;
				return (result);
			}
			else {
				/*
				 *	从一级空间配置器取，一次性取多一点儿
				 */
				size_t bytes_to_get =
					2 * total_bytes + RroundUp(_heap_size >> 4);

				/*
				 *	内存池可能还剩余一些内存，把这些内存挂载到 free_list 上面
				 *	尽可能挂到大的内存块上面，因为大的内存块可以给小的，这样小的内存也可以使用
				 */
				if (left_bytes > 0) {
					//Obj *volatile *my_free_list = _free_list + FreeListIndex(left_bytes);
					Obj **my_free_list = _free_list + FreeListIndex(left_bytes);
					((Obj*)_start_free)->_free_list_link = *my_free_list;
					*my_free_list = (Obj*)_start_free;
				}
				_start_free = (char*)malloc(bytes_to_get);
				
				/*
				 *	从系统申请内存失败，只能自由链表中挂在更大的内存块上取
				 */
				if (NULL == _start_free)
				{
					size_t i = FreeListIndex(size);
					for (; i < _NFREELISTS; ++i) 
					{
						if (_free_list[i] != NULL) 
						{
							Obj *result = _free_list[i];
							_free_list[i] = result->_free_list_link;
							_start_free = (char*)result;
							_end_free = _start_free + (i + 1) * _ALIGN;
							/*转到第二种情况*/
							return (ChunkAlloc(size, nobjs));
						}
					}
					/* free_list 中也没了,注意要把 */
					_end_free = 0;
					_start_free = (char*)__MallocAllocTemplate<0>::Allocate(bytes_to_get);
				}
				_heap_size += bytes_to_get;
				_end_free = _start_free + bytes_to_get;
				return (ChunkAlloc(size, nobjs));
			}
		}
	
		/*
		 *	往自由链表挂载内存大小为 n 的链表上连续挂载 20 块儿
		 */
		static char* Refill(size_t n)
		{
			size_t nobjs = 20;	
			char *chunk = ChunkAlloc(n, nobjs);
			
			//Obj *volatile*myfreelist = _free_list;
			Obj **my_free_list = _free_list;
			Obj *result;
			Obj *current_obj = 0;
			Obj *next_obj;
			//int index;

			/*		调用ChunkAlloc 返回一的可能
			 *	1. 剩余的内存池可能只够一个 n 大小的内存块儿，属于第二种情况返回
			 *  2. 从系统调用失败，从 free_list 中找到更大块儿的内存补到当前所需内存队列上
			 *		这两种情况都表明此时系统内存吃紧
			 */

			if (1 == nobjs)
				return (chunk);
			/*
			 *	往 free_list 上挂载内存，采用尾插的方法
			 *	ChunkAlloc申请的内存是连续的，采用尾插第一次从free_list 上取可以增加CPU的命中率，只限第一次
			 */
			my_free_list = _free_list + FreeListIndex(n);
			result = (Obj*)chunk;
			*my_free_list = next_obj = (Obj*)(chunk + n);
			for (size_t i = 0; i < nobjs - 2; ++i)
			{
				current_obj = next_obj;
				next_obj = (Obj*)((char*)next_obj + n);
				current_obj->_free_list_link = next_obj;
			}
			current_obj->_free_list_link = NULL;

			return chunk;
		}

		/****************************************************
		 *	对外提供的接口
		 *	如果用户申请空间大于 128 个字节，调一级空间配置器
		 *	不大于，调二级
		 ****************************************************/
		static void* Allocate(size_t n)
		{
			void *ret = 0;
			if (n > _MAX_BYTES)
				 ret = __MallocAllocTemplate<0>::Allocate(n);
			else 
			{
				//Obj *volatile*my_free_list = _free_list + FreeListIndex(n);
				Obj **my_free_list = _free_list + FreeListIndex(n);
				Obj *result = *my_free_list;
				if (result == NULL)
					ret = Refill(RroundUp(n));
				else
				{
					*my_free_list = result->_free_list_link;
					ret = result;
				}
			}
			return ret;
		}

		/*
		 *	释放的内存块如果大于 _MAX_BYTES,表示该内存由一级空间配置器分配和释放
		 *	小块内存由二级空间配置器管理，其中小块内存的释放不是直接返还给系统，而是用头插重新挂载到 free_list 上
		 *	进程结束后才将申请到的内存返还给操作系统
		 */
		static void Deallocate(char* p, size_t n)
		{
			if (n > _MAX_BYTES)
				__MallocAllocTemplate<0>::Deallocate(p, n);
			
			//acquire lock
			size_t index = FreeListIndex(n);
			Obj **my_free_list = _free_list + index;
			Obj* q = (Obj*)p;
			q->_free_list_link = *my_free_list;
			*my_free_list = q;
		}
	private:
		
		//对齐内存块
		static size_t RroundUp(size_t bytes)
		{
			return (((bytes)+(size_t)_ALIGN - 1) & ~((size_t)_ALIGN - 1));
		}

		//找到对应内存大小
		static size_t FreeListIndex(size_t bytes)
		{
			return (((bytes)+(size_t)_ALIGN - 1) / (size_t)_ALIGN - 1);
		}

	private:
		enum { _ALIGN = 8 };	//最小内存块
		enum { _MAX_BYTES = 128 };	//最大内存块
		enum { _NFREELISTS = _MAX_BYTES/_ALIGN };	//挂在内存块数组最大长度

		//指向内存块指针
		union Obj {
			union Obj *_free_list_link;
			char _client_data[1];
		};

		static Obj* _free_list[_NFREELISTS];

		static char *_start_free;
		static char *_end_free;
		static size_t _heap_size;
	};
	
	/*初始化静态成员*/
	template <bool threads, int inst>
	typename __DefaultAllocTemplate<threads, inst>::Obj*
		__DefaultAllocTemplate<threads, inst>::_free_list[_NFREELISTS] = { 0 };

	template <bool threads, int inst>
	char* __DefaultAllocTemplate<threads, inst>::_start_free = 0;

	template <bool threads, int inst>
	char* __DefaultAllocTemplate<threads, inst>::_end_free = 0;

	template <bool threads, int inst>
	size_t __DefaultAllocTemplate<threads, inst>::_heap_size = 0;
}

#endif //__ALLOC_H__
