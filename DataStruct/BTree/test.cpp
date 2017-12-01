#include "BinaryTree.hpp"

void TestBinaryTree()
{
	int a[] = { 1, 2, 3, '#', '#', 4, '#', '#', 5, 6 };
	int b[] = { 1, 2, 4, '#', '#', 5, 8, '#', 9, 10, '#', '#', '#', 
		'#', 3, 6, '#', 11, '#', 12, 13, '#', 14, '#', '#', '#', 7 };

	//BinaryTree<int> tree(a,sizeof(a)/sizeof(a[0]),'#');
	BinaryNode<int>* node = NULL;
	BinaryTree<int> t(b, sizeof(b) / sizeof(b[0]), '#');
	/*node = t.FindCommFather2(t.Find(10), t.Find(9));
	node = t.FindCommFather2(t.Find(10), t.Find(14));
	node = t.FindCommFather2(t.Find(14), t.Find(1));
	node = t.FindCommFather2(t.Find(4), t.Find(9));
	node = t.FindCommFather2(t.Find(10), t.Find(1));*/
	int maxlen = -1;
	t.GetMaxLen2(maxlen);
}

int main()
{
	TestBinaryTree();
	return 0;
}
