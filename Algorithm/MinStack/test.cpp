#include "MinStack.hpp"


int main()
{
	/*method1::MinStack<int> s;
	s.Push(5);
	s.Push(4);
	s.Push(2);
	s.Push(6);
	s.Push(1);
	s.Push(0);*/

	/*method2::MinStack<int> s;
	s.Push(5);
	s.Push(4);
	s.Push(2);
	s.Push(6);
	s.Push(1);
	s.Push(0); 
	s.Pop();
	s.Pop();
	s.Pop();
	s.Pop();
	s.Pop();
	s.Pop();
	s.Pop();*/

	method3::MinStack<int> s;
	s.Push(5);
	s.Push(4);
	s.Push(2);
	s.Push(6);
	s.Push(1);
	s.Push(0);
	s.Pop();
	int ret = s.Min();
	s.Pop();
	ret = s.Min();
	s.Pop();
	ret = s.Min();
	s.Pop();
	ret = s.Min();
	s.Pop();
	ret = s.Min();
	s.Pop();
	//ret = s.Min();
	s.Pop();
	//ret = s.Min();

	//const char *str = "hello world";
	//int ret = Strlen(str);
	/*Virtual::child ch;
	Virtual::base* ba = &ch;
	ba->print();
	ba->base::print();*/
	//ba->mid1::print();
	//ba->mid2::print();
	/*ch.print();
	ch.base::print();
	ch.mid1::print();
	ch.mid2::print();*/
	return 0;
}

