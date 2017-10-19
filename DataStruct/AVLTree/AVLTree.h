#ifndef __AVLTREE_H__
#define __AVLTREE_H__

#include <iostream>
#include <stack>
#include <stdlib.h>
template <class key, class value>
struct AVLTreeNode{
	key _key;
	value _value;
	AVLTreeNode<key, value>* _left;
	AVLTreeNode<key, value>* _right;
	AVLTreeNode<key, value>* _parent;
	int _bf;	//平衡因子

	AVLTreeNode(const key& k, const value& v)
		: _key(k)
		, _value(v)
		, _left(NULL)
		, _right(NULL)
		, _parent(NULL)
		, _bf(0)	/* 新节点平衡因子为零，新节点每次插入都作为叶子结点 */
	{}
};
template <class k, class v>
class AVLTree
{
	typedef AVLTreeNode<k, v> Node;
public:
	AVLTree()
		:_root(NULL)
	{}

	~AVLTree()
	{
		_destroy(_root);
	}

	bool insert(const k& key, const v& value)
	{
		if (!_root){
			_root = new Node(key, value);
			return true;
		}
		Node* cur = _root;
		Node* parent = NULL;
		while (cur){
			parent = cur;
			if (key > cur->_key)
				cur = cur->_right;
			else if (key < cur->_key)
				cur = cur->_left;
			else
				return false;
		}
		cur = new Node(key, value);
		cur->_parent = parent;
		if (key > parent->_key)
			parent->_right = cur;
		else
			parent->_left = cur;

		/* 更新平衡因子 */
		while (parent)
		{
			if (parent->_left == cur)
				parent->_bf--;
			else if (parent->_right == cur)
				parent->_bf++;

			if (parent->_bf == 0)
				break;
			else if (1 == abs(parent->_bf)){
				cur = parent;
				parent = parent->_parent;
			}
			else
			{
				if (2 == parent->_bf){
					Node* subR = parent->_right;
					if (1 == subR->_bf)	/* 左单旋 */
						_left_left_rotation(parent);
					else if (-1 == subR->_bf)
						_right_left_rotation(parent);
					
				}
				else if (-2 == parent->_bf)
				{
					Node* subL = parent->_left;
					if (-1 == subL->_bf)
						_right_right_rotation(parent);
					else if (1 == subL->_bf)
						_left_right_rotation(parent);

				}
				else
					return false;
			}
		}
		return true;
	}

	bool isBalance()
	{
		int height = 0;
		return _isBalance(_root, height);
	}

	void inOder(){
		if (!_root)
			return;
		Node* cur = _root;
		std::stack<Node*> s;
		while (cur || !s.empty()){
			while (cur){
				s.push(cur);
				cur = cur->_left;
			}
			Node* front = s.top();
			s.pop();
			std::cout << front->_key << " : " << front->_value << std::endl;
			cur = front->_right;
		}
		std::cout << std::endl;
	}

protected:
	void _destroy(Node* root)
	{
		if (!root)
			return;
		_destroy(root->_left);
		_destroy(root->_right);
		delete root;
		root = NULL;
	}

	void _left_left_rotation(Node* parent)
	{
		Node* subR = parent->_right;
		Node* subRL = subR->_left;
		Node* grandfather = parent->_parent;
		parent->_parent = subR;
		subR->_left = parent;
		if (!grandfather){
			_root = subR;
			subR->_parent = NULL;
		}
		else {
			if (parent->_key > grandfather->_key)
				grandfather->_right = subR;
			else
				grandfather->_left = subR;
			subR->_parent = grandfather;
		}
		parent->_right = subRL;
		if (subRL)
			subRL->_parent = parent;
		parent->_bf = subR->_bf = 0;
	}

	void _right_right_rotation(Node* parent)
	{
		Node* subL = parent->_left;
		Node* subLR = subL->_right;
		Node* grandfather = parent->_parent;
		parent->_parent = subL;
		subL->_right = parent;
		if (!grandfather){
			_root = subL;
			subL->_parent = NULL;
		}
		else {
			if (parent->_key > grandfather->_key)
				grandfather->_right = subL;
			else
				grandfather->_left = subL;
			subL->_parent = grandfather;

		}
		parent->_left = subLR;
		if (subLR)
			subLR->_parent = parent;
		parent->_bf = subL->_bf = 0;

	}

	void _left_right_rotation(Node* parent)
	{
		Node* subL = parent->_left;
		Node* subLR = subL->_right;
		int bf = subLR->_bf;
		_left_left_rotation(parent->_left);
		_right_right_rotation(parent);
		if (bf == -1){
			parent->_bf = 1;
			subL->_bf = 0;
		}
		else if (bf == 1){
			parent->_bf = 0;
			subL->_bf = -1;
		}
		else {
			parent->_bf = subL->_bf = 0;
		}
		subLR->_bf = 0;
	}

	void _right_left_rotation(Node*& parent)
	{
		Node* subR = parent->_right;
		Node* subRL = subR->_left;
		int bf = subRL->_bf;
		_right_right_rotation(subR);
		_left_left_rotation(parent);
		if (1 == bf){
			parent->_bf = -1;
			subRL->_bf = 0;
		}
		else if (-1 == bf){
			parent->_bf = 0;
			subR->_bf = 1;
		}
		else
		{
			subR->_bf = parent->_bf = 0;
		}
		subRL->_bf = 0;
	}

	bool _isBalance(const Node* root, int& height)
	{
		if (!root){
			height = 0;
			return true;
		}
		int leftHeight = 0;
		if (!_isBalance(root->_left, leftHeight))
			return false;

		int rightHeight = 0;
		if (!_isBalance(root->_right, rightHeight))
			return false;

		if (root->_bf != rightHeight - leftHeight)
		{
			std::cout << root->_key << " : " << root->_value << std::endl;

			return false;
		}
		height = leftHeight > rightHeight ? leftHeight + 1 : rightHeight + 1;
		if (abs(rightHeight - leftHeight) > 1)
			return false;
		return true;
	}

private:
	Node* _root;
};

#endif //__AVLTREE_H__

