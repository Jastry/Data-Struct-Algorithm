#include "rb_tree.h"
#include <time.h>
#include <stdlib.h>


#if 0
int main()
{
	std::cout << "hello world" << std::endl;
	return 0;
}
#endif


int main()
{

	rb_tree<int, int> t;
	srand((unsigned)time(NULL));
	for (int i = 0; i < 100; i++){
		int n = rand() % 50000;
		t.insert(n);
	}
	
	rb_tree<int, int>::Iterator it = t.Begin();
	while (it != t.End()){
		std::cout << *it <<" ";
		++it;
	}
	std::cout << std::endl;

	it = t.rBegin();
	while (it != t.rEnd()){
		std::cout << *it <<" ";
		--it;
	}
	std::cout << std::endl;
	//t.inOder();
	//t.postOder();
	//std::cout << t.is_rb_tree() << std::endl;
	return 0;
}
