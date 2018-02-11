#ifndef __HEAP_H__
#define __HEAP_H__

#include <iostream>
#include <vector>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

template<class T>
struct Less
{
	bool operator()(const T& left, const T& right){
		return left < right; 
	}
};

template<class T>
struct Greater{
	bool operator()(const T& left, const T& right){
		return left > right; 
	}
};


template <class T, template <class> class Compara = Less>
class Heap{
public:
	Heap(){}
	Heap(const T* arr, int size){
		assert(arr);
		assert(size > 0);
		_heap.resize(size);

		for (int i = 0; i < size; ++i){
			_heap[i] = arr[i];
		}
		for (int i = (size - 2) / 2; i >= 0; --i){
			_AdjustDown(i);
		}
	} 
	bool IsEmpty(){
		return _heap.empty();
	}

	size_t Size() const
	{
		return _heap.size();
	}


	const T& Top()
	{
		assert(!_heap.empty());
		return _heap[0];
	}
	
	void Push(const T& d)
	{
		_heap.push_back(d);
		_AdjustUp(_heap.size() - 1);
	}

	void Pop()
	{
		int sz = _heap.size();
		if (sz > 1){
			std::swap(_heap[0], _heap[sz - 1]);
			_heap.pop_back();
			_AdjustDown(0);
		}else{
			_heap.pop_back();
		}
	}
protected:
	void _AdjustUp(size_t child){
		Compara<T> cmp;
		assert(child < _heap.size());

		while (child > 0){
			int parent = (child - 1) >> 1;
			if (cmp(_heap[child], _heap[parent])){
				std::swap(_heap[child], _heap[parent]);
				child = parent;
			}else
				break;
		}
	}
#if 0
	void _AdjustDown(size_t parent){
		size_t child = parent * 2 + 1;
		size_t sz = _heap.size();
		Compara<T> cmp;

		while (child < sz){
			if (child + 1 < sz && cmp(_heap[child + 1], _heap[child])){
				child += 1;
			}
			if (cmp(_heap[child] , _heap[parent])){
				std::swap(_heap[child], _heap[parent]);
				parent = child;
				child = parent * 2 + 1;
			}else
				break;
		}
	
	}
#endif 
    void _AdjustDown(size_t parent)
    {
        size_t child = parent * 2 + 1;
        size_t sz = _heap.size();
        
        while (child < sz) {
            if (child+1< sz && heap[child] < heap[child+1]) 
                child++;
            if (_heap[child] > _heap[parent]) {

            
                swap(_heap[child], _heap[parent]);
                parent = child;
                child = parent * 2 + 1;
            }
            else 
                break;
        }
    }
protected:
	 std::vector<T> _heap;
};

template<class T, template <class> class Compara = Greater>
class PriorityQueue{
public:
	PriorityQueue(){}
	PriorityQueue(const T* a, size_t size)
		:_pri(a, size)
	{
	}

	void Push(const T& data){
		_pri.Push(data);
	}
	void Pop(){
		_pri.Pop();
	}

	size_t Size() const
	{
		return _pri.Size();
	}
	const T& Top(){
		return _pri.Top();
	}
protected:
	Heap<T, Compara> _pri;

};
#endif //__HEAP_H__
