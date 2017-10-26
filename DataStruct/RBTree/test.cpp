#include "rb_tree.h"
#include <time.h>
#include <stdlib.h>



int main()
{

	RBTree<int, int> t;
	srand((unsigned)time(NULL));
	for (int i = 0; i < 100; i++){
		int n = rand() % 50000;
		t.Insert(n);
	}
	
	RBTree<int,int>::Iterator it = t.Begin();
	while (it != t.End()){
		std::cout << *it <<" "; 
	}
	std::cout << std::endl;
	//t.inOder();
	//t.postOder();
	//std::cout << t.is_rb_tree() << std::endl;
	return 0;
}
