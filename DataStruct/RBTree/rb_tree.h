#ifndef __RBTREE_H__
#define __RBTREE_H__

#include <iostream>
#include <stack>
#include <utility>

enum COLOUR{
	RED,
	BLACK
};

template <class K, class V>
struct RBTreeNode{
	const K _key;
	V _value;
	enum COLOUR _col;
	RBTreeNode <K, V>* _left;
	RBTreeNode <K, V>* _right;
	RBTreeNode <K, V>* _parent;

	RBTreeNode(const K& key, const V& value)
		:_key(key), _value(value), _left(NULL), _right(NULL), _parent(NULL), _col(RED){}
 };

template <class K, class V>
class RBTree{
public:
	typedef RBTreeNode<K, V> Node;
public:
	RBTree()
		:_root(NULL){}
	~RBTree()
	{
		_rb_tree_delete(_root);
	}
	RBTree(const K* keys, const V* values, size_t nums)
	{
		assert(values);
		assert(keys);
		for (int i = 0; i < nums; ++i){
			insert(keys[i], values[i]);
		}	
	}
	bool insert(const K& key, const V& value)
	{
		if (!_root){
			_root = new Node(key, value);
			_root->_col = BLACK;
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
			parent->_right = cur;
		/* 只有当父节点存在，并且为红色时继续向上调整 */
		/*
		*	分为三种情况
		*	1.0 当 uncle 节点不存在时，直接左旋 / 右旋 / 双旋，改变 parent 和 grandfather 的颜色后 break;

		*	2.0 当 uncle 节点存在
		*		2.1 uncle 节点的颜色是红色，表示 cur 是当前新增节点，改变 uncle parent grandfather 的颜色，继续向上调整
		*		2.2 uncle 节点的颜色是黑色，表示 cur 是前面向上调整后的 grandfather，cur 是之前的 parent
		*			2.2.1 当 parent 是 grandfather 的 left / right && cur 是 parent 的 left / right 时，只需要右旋 / 左旋
		*			2.2.2 当 parent 时 grandfather 的 left / right && cur 是 parent 的 right / left 时，需要 left_right_roattion双旋 / right_left_rotation
		*				注意：双旋之后，需要修复一个 bug，将 parent 与 cur 交换。
		*	最后不论 _root 是什么颜色都要将其置为黑色
		*/

		while (parent && RED == parent->_col){
			Node* grandfather = parent->_parent;
			if (parent == grandfather->_left)
			{
				Node* uncle = grandfather->_right;
				if (uncle && RED == uncle->_col){
					parent->_col =uncle->_col = BLACK;
					grandfather->_col = RED;
					cur = grandfather;
					parent = cur->_parent;
				}
				else {
					/* uncle 不存在 */
					if (cur = parent->_right){
						_rb_tree_rotate_left(parent);
						/* 交换 parent 与 cur */
						_swap(parent, cur);
					}
					_rb_tree_rotate_right(grandfather);
					parent->_col = RED;
					grandfather->_col = BLACK;
				}
			}
			else {
				Node* uncle = grandfather->_left;
				if (uncle && uncle->_col == RED){
					parent->_col = uncle->_col = BLACK;
					grandfather->_col = RED;
					cur = grandfather;
					parent = cur->_parent;
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
	void prevOder()
	{
		_rb_tree_prev_oder(_root);
	}
	void postOder()
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
private:
	void _swap(Node*& left, Node*& right)
	{
		Node* tmp = left;
		left = right;
		right = tmp;
	}
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
		Node* grandfather = parent->_parent;
		subR->_left = parent;
		parent->_parent = subR;
		if (!grandfather){
			_root = subR;
			subR->_parent = NULL;
		}
		else if (grandfather->_right = parent)
			grandfather->_right = subR;
		else
			grandfather->_left = subR;
		parent->_right = subRL;
		if (subRL){
			subRL->_parent = parent;
		}

	}
	void _rb_tree_rotate_right(Node* parent)
	{
		Node* subL = parent->_left;
		Node* subLR = subL->_right;
		Node* grandfather = parent->_parent;
		subL->_right = parent;
		parent->_parent = subL;
		if (!grandfather){
			_root = subL;
			subL->_parent = NULL;
		}
		else if (grandfather->_right == parent)
			grandfather->_right = subL;
		else
			grandfather->_left = subL;
		grandfather->_right = subLR;
		if (subLR){
			subLR->_parent = parent;
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
				std::cout << front->_key << " ";
				s.pop();
				prev = front;
			}
			else {
				cur = front->_right;
			}
		}
		std::cout << std::endl;
	}
	void _rb_tree_prev_oder(Node* node){
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
			std::cout << front->_key << " ";
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
protected:
	Node* _root;
};
#endif // __RBTREE_H__

