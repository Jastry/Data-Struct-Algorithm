#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <iostream>
#include <stack>
#include <queue>
#include <assert.h>

using namespace std;

template <class T>
struct BinaryNode
{
	T _data;
	BinaryNode<T>* _left;
	BinaryNode<T>* _right;

	BinaryNode<T>(const T& data)
		: _data(data)
		, _left(NULL)
		, _right(NULL)
	{}
};
template <class T>
class BinaryTree
{
	typedef BinaryNode<T> Node;
public:
	BinaryTree()
		:_root(NULL)
	{}

	BinaryTree(T* a, size_t n, const T& invalid)
	{
		size_t index = 0;
		_root = _CreatTree(a, n, invalid, index);
	}

	BinaryTree(const BinaryTree<T>& t)
	{
		_root = _CopyBinaryTree(t._root);
	}

	BinaryTree<T> operator=(BinaryTree<T> tree)
	{
		swap(_root, tree._root);
		return *this;
	}

	~BinaryTree()
	{
		_Destroy(_root);
	}

	void PreOder()
	{
		_PreOder(_root);
		cout << endl;
	}

	void PreOderNoR()
	{
		if (NULL == _root){ return; }

		Node* cur = _root;
		stack<Node*> s;

		while (cur || !s.empty())
		{
			while (cur)
			{
				s.push(cur);
				cout << cur->_data << " ";
				cur = cur->_left;
			}
			//右子树走完了
			Node*top = s.top(); //将最右端的节点拿出来
			s.pop();

			cur = top->_right; //将最右端节点的左节点当成子问题
		}
		cout << endl;
	}

	void InOder()
	{
		_InOder(_root);
		cout << endl;
	}

	void InOderNoR()
	{
		if (NULL == _root){ return; }

		stack<Node*> s;
		Node* cur = _root;

		while (cur || !s.empty())
		{
			while (cur)
			{
				s.push(cur);
				cur = cur->_left;
			}
			//右子树走完
			Node* top = s.top();
			s.pop();

			cout << top->_data << " ";
			cur = top->_right;
		}
		cout << endl;
	}

	void LevelOder()
	{
		queue<Node*> q;
		if (NULL != _root){ q.push(_root); }   //先将根节点放入队列中

		while (!q.empty())
		{
			Node* front = q.front();//把上一个节点节点拿出来访问

			cout << front->_data << " ";
			if (front->_left)      //访问右子树
			{
				q.push(front->_left);
			}

			if (front->_right)  //访问左子树
			{
				q.push(front->_right);
			}

			q.pop(); //将上一个根节点pop掉
		}
		cout << endl;
	}

	void PostOder()
	{
		_PostOder(_root);
		cout << endl;
	}

	void PostOderNoR()
	{
		if (NULL == _root){ return; }

		stack<Node*> s;
		Node* cur = _root;
		Node* prev = NULL;

		while (cur || !s.empty())
		{
			while (cur)
			{
				s.push(cur);
				cur = cur->_left;
			}

			Node* front = s.top();
			if (NULL == front->_right || prev == front->_right)
			{
				cout << front->_data << " ";
				prev = front;
				s.pop();
			}
			else
			{
				cur = front->_right;
			}
		}
		cout << endl;
	}

	size_t Size()
	{
		return _Size(_root);
	}

	size_t LeafSize()
	{
		return _LeafSize(_root);
	}

	size_t Depth()
	{
		return _Depth(_root);
	}

	size_t GetKLevelSize(size_t K)
	{
		return _GetKLevelSize(_root, K);
	}

	Node* Find(const T& data)
	{
		return _Find(_root, data);
	}

	size_t noRDepth(Node* r)
	{
		queue<Node*> q;
		if (r == NULL)
			return 0;
		q.push(r);
		int depth = 0;
		while (!q.empty()){
			int len = q.size();
			depth++;
			while (len--){
				Node* t = q.front();
				q.pop();
				if (t->_left)
					q.push(t->_left);
				if (t->_right)
					q.push(t->_right);
			}
		}
		return depth;
	}

	Node* Find(Node* root, Node* data)
	{
		return _Find(root, data);
	}

	Node* FindCommFather(Node* x1, Node* x2)
	{
		assert(x1 && x2);
		return _FindCommFather(_root, x1, x2);
	}

	Node* FindCommFather2(Node* x1, Node* x2)
	{
		std::stack<Node*> s_x1;
		std::stack<Node*> s_x2;
		_GetPath(_root, x1, s_x1);
		_GetPath(_root, x2, s_x2);

		while (s_x1.size() != s_x2.size())
		{
			s_x1.size() > s_x2.size() ? s_x1.pop() : s_x2.pop();
		}
		while (s_x1.size() > 0)
		{
			if (s_x1.top() == s_x2.top())
			{
				return s_x1.top();
			}
			s_x1.pop();
			s_x2.pop();
		}
		return NULL;
	}

	void GetMaxLen()
	{
		int maxlen = -1;
		_GetMaxLen(_root, maxlen);
		return maxlen;
	}

	int GetMaxLen2(int& maxlen)
	{
		return _GetMaxLen2(_root, maxlen);
	}
protected:
	int _GetMaxLen2(Node* root, int& maxlen)
	{
		if (root == NULL)
			return 0;
		int left_depth = _GetMaxLen2(root->_left, maxlen);
		int right_depth = _GetMaxLen2(root->_right, maxlen);
		
		maxlen = maxlen > left_depth + right_depth ? maxlen : left_depth + right_depth;
		return left_depth > right_depth ? left_depth + 1: right_depth + 1;
	}
	
	void _GetMaxLen(Node* root, int& maxlen)
	{
		if (root == NULL)
			return;
		int leftDepth = _Depth(root->_left);
		int rightDepth = _Depth(root->_right);
		maxlen = maxlen > leftDepth + rightDepth ? maxlen : leftDepth + rightDepth;
		_GetMaxLen(root->_left, maxlen);
		_GetMaxLen(root->_right, maxlen);
	}

	Node* _FindCommFather(Node* root, Node* x1, Node* x2)
	{
		if (NULL == root)
			return NULL;
		if (x1 == root || x2 == root)
			return x1 == _root ? x1 : x2;
		bool x1_inleft = false, x1_inright = false;
		bool x2_inleft = false, x2_inright = false;

		if (Find(root->_left, x1))
			x1_inleft = true;
		else if (Find(root->_right, x1))
			x1_inright = true;
		else
			return NULL;
		if (Find(root->_left, x2))
			x2_inleft = true;
		else if (Find(root->_right, x2))
			x2_inright = true;
		else
			return NULL;

		if ((x1_inleft && x2_inright) || (x1_inright && x2_inleft))
			return root;
		else if (x1_inleft && x2_inleft)
			return _FindCommFather(root->_left, x1, x2);
		else if (x1_inright && x2_inright)
			return _FindCommFather(root->_right, x1, x2);
		else
			return NULL;
	}

	Node* _CopyBinaryTree(Node* root)
	{
		if (NULL == root){ return NULL; }
		Node* NewNode = new Node(root->_data);//拷贝当前根节点
		NewNode->_left = (_CopyBinaryTree(root->_left));//拷贝左子树
		NewNode->_right = (_CopyBinaryTree(root->_right));//拷贝右子树
		return NewNode;
	}

	Node* _CreatTree(T* a, size_t n, const T& invalid, size_t& index)
	{
		assert(NULL != a && n > 0);
		Node* root = NULL;
		if (index < n && a[index] != invalid)
		{
			root = new Node(a[index]);
			root->_left = _CreatTree(a, n, invalid, ++index);
			root->_right = _CreatTree(a, n, invalid, ++index);
		}
		return root;
	}

	Node* _OperatorCopy(Node* root)
	{
		if (_root != root)
		{
			~BinaryTree();
		}
		return _CopyBinaryTree(root);
	}

	Node* _Find(Node* root, const T& data)
	{
		if (NULL == root){ return NULL; }

		if (data == root->_data){ return root; }

		Node* find = _Find(root->_left, data);
		return find != NULL ? find : _Find(root->_right, data);
	}
	
	Node* _Find(Node* root, Node* data)
	{
		if (NULL == root)
			return NULL;
		if (root == data)
			return root;
		Node* find = _Find(root->_left, data);
		return find != NULL ? find : _Find(root->_right, data);
	}
	
	bool _GetPath(Node* root, Node* data, std::stack<Node*>& s)
	{
		if (NULL == root)
			return false;
		s.push(root);
		if (s.top() == data)
			return true;
		if (root->_left){
			if (_GetPath(root->_left, data, s))
				return true;
		}
		if (root->_right){
			if (_GetPath(root->_right, data, s))
				return true;
		}
		s.pop();
		return false;
	}

	void  _Destroy(Node* root)
	{
		if (NULL == root){ return; }
		_Destroy(root->_left);
		_Destroy(root->_right);
		//cout<<root->_data<<" ";
		delete root;
	}

	void  _PreOder(Node* root)
	{
		if (NULL == root){ return; }
		cout << root->_data << " ";
		_PreOder(root->_left);
		_PreOder(root->_right);
	}

	void  _PostOder(Node* root)
	{
		if (NULL == root){ return; }
		_PostOder(root->_left);
		_PostOder(root->_right);
		cout << root->_data << " ";
	}

	void  _InOder(Node* root)
	{
		if (NULL == root){ return; }
		_InOder(root->_left);
		cout << root->_data << " ";
		_InOder(root->_right);
	}

	size_t _Size(Node* root)
	{
		if (NULL == root){ return NULL; }
		return _Size(root->_left) + _Size(root->_right) + 1;
	}

	size_t _LeafSize(Node* root)
	{
		if (NULL == root){ return NULL; }

		if (NULL == root->_left && NULL == root->_right){ return 1; }   //找到一个叶子节点

		return  _LeafSize(root->_left) + _LeafSize(root->_right);
	}

	size_t _GetKLevelSize(Node* root, size_t K)
	{
		if (NULL == root){ return NULL; }

		if (K == 1){ return 1; } //到达第K层

		/*右子树的第K-1层阶节点数   +   左子树的第K-1层阶节点数*/
		return _GetKLevelSize(root->_left, K - 1) + _GetKLevelSize(root->_right, K - 1);
	}

	size_t _Depth(Node* root)
	{
		if (NULL == root){ return NULL; }

		if (NULL == root->_left && NULL == root->_right){ return 1; }

		size_t Left = _Depth(root->_left);
		size_t Right = _Depth(root->_right);
		return Left > Right ? Left + 1 : Right + 1;
	}

protected:
	Node* _root;
};

#endif //__BINARYTREE_H__ 
