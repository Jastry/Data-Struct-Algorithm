#pragma once


 /*
  *			实现功能:
  *			创建日期:
  *			联系方式:blbagony@163.com
  *			修改记录:
  *			备注:
  */
#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cassert>
#include <set>
#include <bitset>
#include <unordered_set>
#include <stack>
#include <unordered_map>
#include <cassert>
#include <math.h>

#if 0
/* 创建链表 */
struct ListNode {
	int val;
	ListNode * next;
	ListNode(int x) : val(x), next(NULL){}
};

ListNode * CreateList(int a[], int n) {
	assert(a);
	ListNode *head = NULL, *p = NULL;
	for (auto i = 0; i < n; ++i) {
		if (head == NULL) {
			head = p = new ListNode(a[i]);
		}
		else {
			p->next = new ListNode(a[i]);
			p = p->next;
		}
	}
	return head;
}

/* 销毁链表 */
void Destroy(ListNode * head) {
	if (head == NULL)
		return;
	ListNode * del = head;
	while (head) {
		head = head->next;
		delete del;
		del = head;
	}
}
#endif

#if 0

/*************************************
*	用两个栈实现队列;用两个队列实现栈
**************************************/

namespace QueueAndstack{
	template <class T>
	class QueueByTwoStack{
	public:
		QueueByTwoStack(){}
		void Push(const T& x)
		{
			_in.push(x);
		}

		void Pop()
		{
			if (!_out.empty())
				_out.pop();
			else
			{
				if (_in.empty())
					return;
				while (!_in.empty())
				{
					_out.push(_in.top());
					_in.pop();
				}
				_out.pop();
			}
		}

		T& Front()
		{
			if (!_out.empty())
				return _out.top();
			else
			{
				while (!_in.empty())
				{
					_out.push(_in.top());
					_in.pop();
				}
				return _out.top();
			}
		}

		size_t Size()
		{
			return _out.size() + _in.size();
		}

		bool Empty()
		{
			return _out.empty() && _in.empty();
		}

	protected:
		std::stack<T> _in;
		std::stack<T> _out;
	};

	template <class T>
	class StackByTwoQueue{
	public:
		StackByTwoQueue(){}
		void Push(const T& x)
		{
			if (_q1.empty())
				_q2.push(x);
			else
				_q1.push(x);
		}

		void Pop()
		{
			if (_q1.empty())
			{
				while (_q2.size() > 1)
				{
					_q1.push(_q2.front());
					_q2.pop();
				}
				_q2.pop();
			}
			else
			{
				while (_q1.size() > 1)
				{
					_q2.push(_q1.front());
					_q1.pop();
				}
				_q1.pop();
			}
		}

		T& Top()
		{
			if (_q1.empty())
				return _q2.back();
			else
				return _q1.back();
		}

		size_t Size()
		{
			return _q1.size() + _q2.size();
		}

		bool Empty()
		{
			return _q1.empty() && _q2.empty();
		}

	protected:
		std::queue<T> _q1;
		std::queue<T> _q2;
	};
}


/*********************************************
*	实现一个栈 push pop min 时间复杂度为 0(1)
*********************************************/

namespace method1{
	template <class T>
	class MinStack{
	public:
		MinStack(){}
		void Push(const T& x)
		{
			_s.push(x);
			if (_min.empty() || _min.top() > x)
			{
				_min.push(x);
			}
			else
				_min.push(_min.top());
		}
		void Pop()
		{
			_s.pop();
			_min.pop();
		}
		T& top()
		{
			return _s.top();
		}
		T Min()
		{
			return _min.top();
		}
	protected:
		std::stack<T> _s;
		std::stack<T> _min;
	};
}

namespace method2{
	template <class T>
	class MinStack{
	public:
		MinStack(){}
		void Push(const T& x)
		{
			_s.push(x);
			if (_min.empty() || _min.top().first > x)
			{
				_min.push(std::make_pair(x, 1));
			}
			else
			{
				//++(_min[_min.size() - 1]).second;
				++_min.top().second;
			}
		}
		void Pop()
		{
			_s.pop();
			if (--_min.top().second == 0){
				_min.pop();
			}
		}
		T& Top()
		{
			return _s.top();
		}
		T Min()
		{
			return _min.top();
		}
	protected:
		std::stack<T> _s;
		std::stack< std::pair<T, int> > _min;
	};
}

namespace method3{
	template <class T>
	class MinStack{
	public:
		MinStack(){}
		void Push(const T& x)
		{
			if (_s.empty() || _s.top() > x)
			{
				_s.push(x);
				_s.push(x);
			}
			else
			{
				T tamp = _s.top();
				_s.push(x);
				_s.push(tamp);
			}
		}
		void Pop()
		{
			_s.pop();
			_s.pop();
		}

		T Min()
		{
			return _s.top();
		}
	protected:
		std::stack<T> _s;
	};
}

/********************
*	一些函数
*******************/
namespace fun{
	/*******************************************************************************
	 * 模拟出栈顺序，将出栈划分为几次，原来栈只要入栈一次就打破了连续出栈的顺序，\
	 * 让现在对比栈匹配出栈元素如果匹配到，\
	 * 就判断出栈顺序（需要判断的出栈顺序）和当前对比栈顺序是否相同，\
	 * 如果相同清理对比栈元素，后面存放下一次连续的 pop。每一次清理\
	 * 都会将当前对比栈一次清空，如果没有则表明出栈顺序有问题。
	 ********************************************************************************/
	bool StackPopOderIsLegal(char* in, char* out)
	{
		assert(in && out && strlen(in) == strlen(out));
		std::stack<char> tamp;
		for (; *in != '\0'; in++){
			tamp.push(*in);
			while (!tamp.empty() && *out == tamp.top())
			{
				out++;
				tamp.pop();
			}
		}
		return tamp.empty() ? true : false;
	}


	size_t Strlen(const char* str)
	{
		assert(str);
		return (*str != '\0') ? (1 + Strlen(str + 1)) : 0;
	}

	void* Memcpy(void* dst, const void* src, size_t size){
		assert(dst);
		assert(src);
		const char* s = (const char*)src;
		char* d = (char*)dst;

		//该种情况不存在重叠问题
		if (d > (s + size) || d < s){
			while (size--){
				*d++ = *s++;
			}
		}
		else
		{
			d += (size - 1);
			s += (size - 1);
			for (size_t i = 0; i < size; ++i){
				*d-- = *s--;
			}
		}
		return (void*)dst;
	}


	void swap(int *a, int *b)
	{
		int temp;

		temp = *a;
		*a = *b;
		*b = temp;


		return;
	}

	void quicksort(int array[], int maxlen, int begin, int end)
	{
		int i, j;

		if (begin < end)
		{
			i = begin + 1;  // 将array[begin]作为基准数，因此从array[begin+1]开始与基准数比较！
			j = end;        // array[end]是数组的最后一位

			while (i < j)
			{
				if (array[i] > array[begin])  // 如果比较的数组元素大于基准数，则交换位置。
				{
					swap(&array[i], &array[j]);  // 交换两个数
					j--;
				}
				else
				{
					i++;  // 将数组向后移一位，继续与基准数比较。
				}
			}

			/* 跳出while循环后，i = j。
			* 此时数组被分割成两个部分  -->  array[begin+1] ~ array[i-1] < array[begin]
			*                           -->  array[i+1] ~ array[end] > array[begin]
			* 这个时候将数组array分成两个部分，再将array[i]与array[begin]进行比较，决定array[i]的位置。
			* 最后将array[i]与array[begin]交换，进行两个分割部分的排序！以此类推，直到最后i = j不满足条件就退出！
			*/

			if (array[i] >= array[begin])  // 这里必须要取等“>=”，否则数组元素由相同的值时，会出现错误！
			{
				i--;
			}

			swap(&array[begin], &array[i]);  // 交换array[i]与array[begin]

			quicksort(array, maxlen, begin, i);
			quicksort(array, maxlen, j, end);
		}
	}
}
namespace Virtual{
	class base{
	public:
		base(){
			std::cout << "base called" << std::endl;
		}
		virtual void print(){
			std::cout << "base print" << std::endl;
		}
	};

	class mid1 :virtual public base{
	public:
		mid1(){
			std::cout << "mid1 called" << std::endl;
		}
		void print(){
			std::cout << "mid1 print" << std::endl;
		}
	};

	class mid2 :virtual public base{
	public:
		mid2(){
			std::cout << "mid2 called" << std::endl;
		}
		void print(){
			std::cout << "mid2 print" << std::endl;
		}
	};

	class child :virtual public mid1, virtual public mid2{
	public:
		child(){
			std::cout << "child called" << std::endl;
		}
		virtual void print(){
			std::cout << "child print" << std::endl;
		}
	};

}

#endif
