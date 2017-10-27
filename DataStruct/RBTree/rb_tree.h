/*
	windows 
	VS2013
	blbagony@163.com
*/
#ifndef __RBTREE_H__
#define __RBTREE_H__

#include <iostream>
#include <stack>
#include <utility>
#include <time.h>
#include <assert.h>

enum COLOUR{
	RED,
	BLACK
};


template <class Valuetype>
struct _rb_tree_node{
	Valuetype _value;
	enum COLOUR _col;
	_rb_tree_node <Valuetype>* _left;
	_rb_tree_node <Valuetype>* _right;
	_rb_tree_node <Valuetype>* _parent;

	_rb_tree_node(const Valuetype& value)
		:_value(value), _left(NULL), _right(NULL), _parent(NULL), _col(RED){}
};


template <class Valuetype>
struct _rb_tree_iterator{
	typedef _rb_tree_node<Valuetype> _rb_node;
	typedef _rb_tree_iterator<Valuetype> Self;
	
	_rb_node* _node;
	
	_rb_tree_iterator(_rb_node* node)
		:_node(node)
	{}

	Self& operator++()
	{
		if (!_node)
			return *this;
		_rb_node* cur = _node;
		_rb_node* parent = cur->_parent;
		if (cur->_right){
			cur = cur->_right;
			while (cur->_left)
				cur = cur->_left;
			_node = cur;
		}
		else if (parent && parent->_left == cur){
			_node = parent;
		}
		else {
			while (parent && parent->_right == cur){
				cur = parent;
				parent = cur->_parent;
			}
			_node = parent;
		}
		return *this;
	}

	
	Valuetype& operator*() const
	{
		return _node->_value;
	}
	
	Valuetype* operator->() const
	{
		return &(this->operator*());
	}
	
	bool operator!=(const Self it) const
	{
		return _node != it._node;
	}
};

template <class Valuetype>
struct Less{
	bool operator()(const Valuetype& left, const Valuetype& right){
		return left < right;
	}
};

template <class Key, class Value, class Compara = Less< Value > >
class rb_tree{
public:
	typedef _rb_tree_iterator<Value> Iterator;
	typedef _rb_tree_node<Value> Node;
public:
	rb_tree()
		:_root(NULL){}
	~rb_tree()
	{
		_rb_tree_delete(_root);
	}
bool insert(const Value& value)
{
	if (!_root){
		_root = new Node(value);
		_root->_col = BLACK;
		return true;
	}

	Node* cur = _root;
	Node* parent = NULL;
	while (cur){
		parent = cur;
		if (value > cur->_value)
			cur = cur->_right;
		else if (value < cur->_value)
			cur = cur->_left;
		else
			return false;
	}
	cur = new Node(value);
	cur->_parent = parent;
	
	if (value > parent->_value)
		parent->_right = cur;
	else
		parent->_left = cur;
	if (parent->_col == BLACK)
		return true;

	while (parent && RED == parent->_col){
		//此时grandfather 一定存在
		Node* grandfather = parent->_parent;
		if (parent == grandfather->_left)
		{
			Node* uncle = grandfather->_right;
			if (uncle && RED == uncle->_col){
				parent->_col = uncle->_col = BLACK;
				grandfather->_col = RED;
				if (grandfather->_parent && grandfather->_parent->_col == RED){
					cur = grandfather;
					parent = cur->_parent;
					grandfather = parent->_parent;
				}
				else
					break;
			}
			else {
				/* uncle 不存在 */
				if (cur == parent->_right){
					_rb_tree_rotate_left(parent);
					/* 交换 parent 与 cur */
					_swap(parent, cur);
				}
				_rb_tree_rotate_right(grandfather);
				parent->_col = BLACK;
				grandfather->_col = RED;
			}
		}
		else {
			Node* uncle = grandfather->_left;
			if (uncle && uncle->_col == RED){
				parent->_col = BLACK;
				uncle->_col = BLACK;
				grandfather->_col = RED;
				if (grandfather->_parent && grandfather->_parent->_col == RED){
					cur = grandfather;
					parent = cur->_parent;
					grandfather = parent->_parent;
				}
				else
					break;
			}
			else
			{
				if (cur == parent->_left){
					_rb_tree_rotate_right(parent);
					_swap(parent, cur);
				}
				_rb_tree_rotate_left(grandfather);
				parent->_col = BLACK;
				grandfather->_col = RED;
			}
		}
	}
	_root->_col = BLACK;
	return true;
}



	void in_oder()
	{
		_rb_tree_in_oder(_root);
	}
	void post_oder()
	{
		_rb_tree_post_oder(_root);
	}
	bool is_rb_tree(){
		if (!_root)
			return true;
		int k = 0;
		Node* cur = _root;
		while (cur){
			if (cur->_col == BLACK)
				++k;
			cur = cur->_left;
		}
		return _is_rb_tree(_root, 0, k);
	}
	Iterator& Begin()
	{
		if (!_root)
			return Iterator(NULL);
		Node* cur = _root;
		while (cur->_left)
			cur = cur->_left;
		
		return Iterator(cur);
	}
	Iterator& End(){
		return Iterator(NULL);
	}


private:

	void _rb_tree_delete(Node* node)
	{
		if (!node)	
			return;
		_rb_tree_delete(node->_left);
		_rb_tree_delete(node->_right);
		delete node;
	}
	void _rb_tree_rotate_left(Node* parent)
	{
		Node* subR = parent->_right;
		Node* subRL = subR->_left;
		subR->_left = parent;
		Node* grandfather = parent->_parent;
		parent->_parent = subR;
		parent->_right = subRL;
		if (subRL){
			subRL->_parent = parent;
		}
		if (!grandfather){
			_root = subR;
			_root->_parent = NULL;
		}
		else{
			if (grandfather->_left == parent)
				grandfather->_left = subR;
			else
				grandfather->_right = subR;
			subR->_parent = grandfather;
		}
	}
	void _rb_tree_rotate_right(Node* parent)
	{
		Node* subL = parent->_left;
		Node* subLR = subL->_right;
		subL->_right = parent;
		if (subLR){
			subLR->_parent = parent;
		}
		subL->_right = parent;
		Node* grandfather = parent->_parent;
		parent->_parent = subL;
		parent->_left = subLR;
		if (NULL == grandfather){
			_root = subL;
			_root->_parent = NULL;
		}
		else{
			if (grandfather->_left == parent)
				grandfather->_left = subL;
			else
				grandfather->_right = subL;
			subL->_parent = grandfather;
		}
	}
	void _rb_tree_post_oder(Node* node)
	{
		if (!node)
			return;
		Node* cur = node;
		std::stack<Node*> s;
		Node* prev = NULL;
		while (cur || !s.empty()){
			while (cur){
				s.push(cur);
				cur = cur->_left;
			}
			Node* front = s.top();
			if (!front->_right || front->_right == prev){
				std::cout << front->_value<< " ";
				s.pop();
				prev = front;
			}
			else {
				cur = front->_right;
			}
		}
		std::cout << std::endl;
	}
	void _rb_tree_in_oder(Node* node){
		if (!node)
			return;
		Node* cur = node;
		std::stack<Node*> s;
		while (cur || !s.empty()){
			while (cur){
				s.push(cur);
				cur = cur->_left;
			}
			Node* front = s.top();
			s.pop();
			std::cout << front->_value << " ";
			cur = front->_right;
		}
		std::cout << std::endl;
	}
	bool _is_rb_tree(Node* node, int black_node_number, int k)
	{
		if (!node){
			if (k == black_node_number){
				return true;
			}
			std::cout << "black_node_number is " << black_node_number << std::endl;
			std::cout << "k is " << k << std::endl;
			return false;
		}
		if (node->_col == RED && node->_parent && node->_parent->_col == RED){
			std::cout << "有连续红节点" << std::endl;
			return false;
		}
		if (node->_col == BLACK){
			black_node_number++;
		}
		if (!_is_rb_tree(node->_left, black_node_number, k))
			return false;
		if (!_is_rb_tree(node->_right, black_node_number, k))
			return false;
		return true;
	}
	void _swap(Node*& left, Node*& right)
	{
		assert(!left && !right);
		Node* tamp = left;
		left = right;
		right = tamp;
	}
protected:
	Node* _root;
};
#endif // __RBTREE_H__
