#include "BinarySearchTree.h"

int main()
{
	BinarySearchTree<int, int> t;
	int arr[] = {7,3,8,10,4,12,5,89,6,34,13,32,14};
	int num = sizeof (arr) / sizeof (arr[0]);
	for (int i = 0; i < num; ++i){
		t.insert(arr[i], i + i);
	}
	t.inOrder();
	for (int i = 0; i < 100; ++i){
		t.erase(i);
	}
	t.inOrder();

	//t.levelOder();
	//t.inOrder();
	//t.prevOder();
	//t.postOder();
	//for (int i = 0; i < 100; ++i){
	//	BinaryTreeNode<int, int>* n = t.find(i);
	//	if (n){
	//		std::cout << i << " ";
	//		//n->_data = 10;	
	//		//n->_right = NULL;
	//	}
	//}
	std::cout << std::endl;
	return 0;
}


