#ifndef _BINARYSERACHTREE_H_
#define _BINARYSERACHTREE_H_

#include <iostream>
#include <vector>
#include <stack>

template <class T, class V>
struct BinaryTreeNode{
	/*const*/ T _data;
	V _value;
	BinaryTreeNode<T, V>* _right;
	BinaryTreeNode<T, V>* _left;

	BinaryTreeNode(const T& d, const V& v)
		:_data(d)
		, _value(v)
		, _right(NULL)
		, _left(NULL)
	{}
};

template <class T, class V>
class BinarySearchTree{
	typedef BinaryTreeNode<T, V> Node;
public:
	BinarySearchTree()
		:_root(NULL)			
	{}

	BinarySearchTree(const T* array_k, const V* array_v ,int num)
		:_root(NULL)
	{
		assert(array_k);
		assert(array_v);
		for (int i = 0; i < num; ++i){
			T& data = array_k[i];
			V& value = array_v[i];
			insert(data, value);
		}
	}

	~BinarySearchTree()
	{
		_destory(_root);
	}
public:
	static void _destory(Node* root)
	{
		if (root == NULL)
			return;
		_destory(root->_left);
		_destory(root->_right);
		delete root;
	}
	
	void inOrder(){
		if (NULL == _root)
			return;
		Node* cur = _root;
		std::stack<Node*> s;
		while (cur || !s.empty()){
			while (cur){
				s.push(cur);
				cur = cur->_left;
			}
			cur = s.top();
			s.pop();
			std::cout << cur->_data << " ";
			cur = cur->_right;
		}
		std::cout << std::endl;
	}	

	void prevOder()
	{
		if (!_root)
			return;
		Node* cur = _root;
		std::stack<Node*> s;
		while (cur || !s.empty()){
			while (cur){
				std::cout << cur->_data << " ";
				s.push(cur);
				cur = cur->_left;
			}
			cur = s.top();
			s.pop();
			cur = cur->_right;
		}
		std::cout << std::endl;
	}
	
	void postOder()
	{
		if (!_root)
			return;
		Node* cur = _root;
		Node* prev = cur;
		std::stack<Node*> s;
		while (cur || !s.empty()){
			while (cur){
				s.push(cur);
				cur = cur->_left;
			}
			Node* front = s.top();
			if ( !front->_right || front->_right == prev ){
				std::cout << front->_data << " ";
				prev = front;
				s.pop();
			}
			else 
				cur = front->_right;
		}
		std::cout << std::endl;
	}
	
	void levelOder()
	{
		if (!_root)
			return;
		Node* cur = _root;
		std::stack<Node*> s;
		s.push(cur);
		while ( !s.empty() ){
			Node* front = s.top();
			s.pop();
			std::cout << front->_data << " ";
			if (front->_left)
				s.push(front->_left);
			if (front->_right)
				s.push(front->_right);
		}
		std::cout << std::endl;
	}

	void erase(const T& d)
	{
		_delete(_root, d);
	}

	Node* find(const T& d)
	{
		if (!_root)
			return NULL;
		Node* cur = _root;
		while (cur){
			T& data = cur->_data;
			if (data > d)
				cur = cur->_left;
			else if (data < d)
				cur = cur->_right;
			else
				return cur;
		}
		return NULL;
	}

	Node* findMin(Node*& pos){
		if (!pos)
			return NULL;
		Node* min = pos;
		while (min->_left != NULL){
			min = min->_left;
		}
		return min;
	}

	bool insert(const T& d, const V& v)
	{
		if (!_root){
			_root = new Node(d, v);
			return true;
		}
		Node* cur = _root;
		Node* prev = NULL;
		while (cur){
			prev = cur;
			if (cur->_data > d)
				cur = cur->_left;
			else if (cur->_data < d)
				cur = cur->_right;
			else
				return false;
		}
		if (prev->_data > d)
			prev->_left = new Node(d, v);
		else
			prev->_right = new Node(d, v);
		return true;
	}
private:
	static void _destroy(Node*& root)
	{
		if (!root)
			return;
		_destroy(root->_left);
		_destroy(root->_right);
		delete root;
	}

	Node* _delete(Node*& cur, const T& d)
	{
		if (!cur)
			return cur;
		if (cur->_data < d)
			return _delete(cur->_right, d);
		if (cur->_data > d)
			return _delete(cur->_left, d);
		else {
			Node* del = cur;
			if (!cur->_left)
				cur = cur->_right;
			else if (!cur->_right)
				cur = cur->_left;
			else 
			{
				Node* prev = findMin(cur->_right);
				cur->_data = prev->_data;
				return _delete(cur->_right, prev->_data);
			}
			delete del;
		}
		return cur;
	}

protected:
	Node* _root;
};
#endif //_BINARYSERACHTREE_H_
