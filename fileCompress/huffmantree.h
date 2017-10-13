
#include "heap.h"

template <class T>
struct HuffmanNode{
	T _wt;
	HuffmanNode<T>* _left;
	HuffmanNode<T>* _right;
	HuffmanNode<T>* _prev;


	HuffmanNode(const T& w)
		:_wt(w)
		, _left(NULL)
		, _right(NULL)
		, _prev(NULL)
	{}
};


template <class T>
struct SmallHeap{
	bool operator()(const T& left, const T& right){
		return left->_wt < right->_wt;
	}
};


template <class T>
class HuffmanTree
{
public:
	typedef HuffmanNode<T> Node;
public:
	HuffmanTree(){}
	HuffmanTree(const T* a, int size, const T& invalue = T()){
		assert(a && size > 0);
		Heap<Node*, SmallHeap> huffman_tree;
		for (int i = 0; i < size; ++i){
			if (a[i] != invalue){
				Node* huffman_node = new Node(a[i]);
				huffman_tree.Push(huffman_node);
			}
		}

		while (huffman_tree.Size() > 1){
			Node* huffman_node_left = huffman_tree.Top();	huffman_tree.Pop();
			Node* huffman_node_right = huffman_tree.Top();	huffman_tree.Pop();

			Node* parent = new Node(huffman_node_left->_wt + huffman_node_right->_wt);
			parent->_left = huffman_node_left;
			huffman_node_left->_prev = parent;

			parent->_right = huffman_node_right;
			huffman_node_right->_prev = parent;

			huffman_tree.Push(parent);
		}
		_root = huffman_tree.Top();
	}

	Node* get_root(){
		return _root;
	}
	~HuffmanTree(){
		_Destory(_root);
	}
protected:
	void _Destory(Node* root){
		if (NULL == root)
			return;
		_Destory(root->_left);
		_Destory(root->_right);
		delete root;
		root = NULL;
	}
protected:
	Node* _root;
};
