#include <iostream>

using namespace std;
struct listNode;

struct listNode{
	int val;
	listNode* next;
	listNode(int x):val(x),next(NULL){}
};

class Solution{
public:
	listNode* addTwoNum(listNode*& l1, listNode*& l2) {
		if (!l1 && !l2)	return l1;
		listNode* node = new listNode(0);
		back(node, l1, l2);
		return node;
	}
protected:
	listNode* back(listNode*& result, listNode*& l1, listNode*& l2) {
		if (!l1 && !l2)
			return result;
		if (l1) result->val += l1->val;
		else l1 = new listNode(0);
		if (l2) result->val += l2->val;
		else l2 = new listNode(0);
		/*c存放进位*/
		listNode* node = new listNode(0);

		if (result->val > 10){
			result->val %= 10;
			node->val = 1;
			result->next = node;
		}
		back(result->next, l1->next, l2->next);
	}
};
int main()
{
	listNode* l1 = new listNode(2);
	l1->next = new listNode(4);
	l1->next->next = new listNode(3);

	listNode* l2 = new listNode(5);
	l2->next = new listNode(6);
	l2->next->next = new listNode(4);
	Solution s;
	listNode* result = s.addTwoNum(l1,l2);
	while (result){
		cout<<result->val<<" ";
		result = result->next;
	}
	cout<<endl;
	return 0;
}
