#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <stack>
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
            if (_s.empty())
                return;
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
            if (_s.empty())
                return;
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

