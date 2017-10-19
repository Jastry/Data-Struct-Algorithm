#include "AVLTree.h"
int main()
{
	AVLTree<int, int> t;
	//int arr[] = { 4, 2, 6, 1, 3, 5, 15, 8, 16, 7 };
	int arr[] = { 5, 3, 4, 1, 7, 8, 2, 6, 0, 9 };
	int num = sizeof (arr) / sizeof (int);
	for (int i = 0; i < num; ++i){
		t.insert(arr[i], i);
	}
	t.inOder();
	std::cout << "is balance : " << t.isBalance() << std:: endl;
}
