#include "rb_tree.h"
#include <time.h>
#include <stdlib.h>

typedef RBTreeNode<int, int> Node;


int main(int argc, char** argv)
{

	RBTree<int, int> t;
	srand((unsigned)time(NULL));
	for (int i = 0; i < 100; i++){
		int n = rand() % 50000;
		t.insert(i,i);
	}

	
	t.prevOder();
	t.postOder();
	std::cout << t.is_rb_tree() << std::endl;
	return 0;
}
