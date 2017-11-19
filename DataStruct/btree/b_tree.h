#ifndef __BTREE_H__
#define __BTREE_H__

#include <iostream>
#include <string>
#include <vector>
#include <string.h>

const size_t MAX_KEY = 4;



template <class Key, class Value, size_t M = MAX_KEY>
struct BTreeNode{

	std::pair<Key, Value> _kvs[M];
	BTreeNode<Key, Value, M>* _subs[M + 1];
	BTreeNode<Key, Value, M>* _parent;

	size_t _size;
	
	BTreeNode()
	{
		memset(this, 0, sizeof (BTreeNode));
	}
	BTreeNode(const std::pair<Key, Value>& kv)
	{
		memset(this, 0, sizeof (BTreeNode));
		_kvs[0] = kv;
		_size++;
	}
};


template <class Key, class Value, size_t M = MAX_KEY>
class BTree{
	//typename BTreeNode;

public:
	typedef BTreeNode<Key, Value, M> Node;
public:
	BTree()
		:_root(NULL)
	{}
	~BTree()
	{
		_Destroy(_root);
	}


	//return node* & index,index type is int because not search not found return -1;
	std::pair<Node*, int> Find(const Key &key)
	{
		Node* parent = NULL;
		Node* cur = _root;
		while (cur != NULL) {
			size_t i = 0;
			while (i < cur->_size) {
				if (cur->_kvs[i].first > key) {//key 位于跟自己下表相同的孩子数组中
					break;
				}
				else if (cur->_kvs[i].first < key) {
					++i;
				}
				else {
					return std::make_pair(cur, i);
				}
			}
			parent = cur;
			cur = cur->_subs[i];
		}
		//如果没找到，返回指向插入位置的节点
		return std::make_pair(parent, -1);
	}
	Node* _DivideNode(Node* cur)
	{
		Node* new_node = new Node;
		int n = cur->_size;
		int mid = n >> 1;
		int i = mid + 1;
		int j = 0;
		
		for (; i < n; ++i) {
			new_node->_kvs[j] = cur->_kvs[i];
			new_node->_size++;
			cur->_size--;
			new_node->_subs[j] = cur->_subs[i];
			if (new_node->_subs[j]) {
				new_node->_subs[j]->_parent = new_node;
			}
			j++;
		}

		new_node->_subs[j] = cur->_subs[i];
		if (new_node->_subs[j]) {
			new_node->_subs[j]->_parent = new_node;
		}
		return new_node;
	}

	bool Insert(const std::pair<Key, Value> &kv)
	{
		if (!_root) {
			_root = new Node(kv);
			return true;
		}
		std::pair<Node*, int> ret = Find(kv.first);
		if (ret.second > 0) {
			return false;	//已经存在
		}
		Node* cur = ret.first;
		Node* sub = NULL;
		std::pair<Key, Value> new_kv = kv;
		
		//insert cur  -> new_kv & sub
		for (;;) {
			_BaseInsert(cur, sub, new_kv);

			if (cur->_size < M)
				return true;
			else {
				//分裂
				Node* new_node = _DivideNode(cur);
				std::pair<Key, Value> mid_kv = cur->_kvs[cur->_size >> 1];
				if (cur == _root) {
					_root = new Node(mid_kv);
					cur->_size--;// _root 拿走 cur 的 mid_kv;
					_root->_subs[0] = cur;
					_root->_subs[1] = new_node;
					cur->_parent = _root;
					new_node->_parent = _root;
					return true;
				}
				else
				{
					//子问题，把 mid_kv 插入 cur->_parent 
					sub = new_node;
					new_kv = mid_kv;
					cur = cur->_parent;
				}
			}
		}
	}

	void Show()
	{
		_InOder(_root);
		std::cout << std::endl;
	}
private:
	void _BaseInsert(Node* node, Node* sub, const std::pair<Key, Value>& kv)
	{
		int end = node->_size - 1;
		while (end >= 0) {//insert
			if (node->_kvs[end].first > kv.first) {
				node->_kvs[end + 1] = node->_kvs[end];
				node->_subs[end + 2] = node->_subs[end + 1];
				--end;
			}
			else
				break;
		}
		node->_kvs[end + 1] = kv;
		node->_subs[end + 2] = sub;
		if (sub) {
			sub->_parent = node;
		}
		node->_size++;
	}

	void _InOder(Node* root)
	{
		if (!root){
			return;
		}
		Node* cur = root;
		while (cur){
			size_t i = 0;
			size_t ks = cur->_size;
			for (; i < ks; ++i){
				_InOder(cur->_subs[i]);
				std::cout << cur->_kvs[i].first << " ";
			}
			cur = cur->_subs[i];
		}
	}

	void _Destroy(Node *node)
	{
		if (!node)
			return;
		int sz = node->_size;
		for (int i = 0; i < sz; ++i) {
			_Destroy(node->_subs[i]);
		}
		delete node;
	}

protected:
	Node* _root;
};

void TestBTree()
{
	BTree<int, int, 3> t;
	int a[] = { 53, 75, 139, 49, 145, 36, 101 };
	for (size_t i = 0; i < sizeof(a) / sizeof(a[0]); ++i)
	{
		t.Insert(std::make_pair(a[i], i));
	}

	t.Show();
}
#endif

