#ifndef __BTTHREAD_H__
#define __BTTHREAD_H__

#include <iostream>
#include <stack>
#include <assert.h>

enum NodeType{LINK, THREAD};

template <class T>
struct __BinaryTreeNode
{
    T _data;
    __BinaryTreeNode<T>* _left;
    __BinaryTreeNode<T>* _right;
    NodeType _lefttype;
    NodeType _righttype;

    __BinaryTreeNode<T>(const T& data)
        :_data(data)
        ,_left(NULL)
        ,_right(NULL)
        ,_lefttype(LINK)
        ,_righttype(LINK)
    {}
};

template <class T>
struct __InOderThreadIterator
{
    typedef __BinaryTreeNode<T> Node;
    typedef __BinaryTreeNode<T>* pNode;
    typedef __InOderThreadIterator<T> Self;
public:
    __InOderThreadIterator(Node* node)
        :_node(node)
    {}
    T& operator*(){
        return _node->_data;
    }
    pNode operator->(){
        return _node;
    }
    bool operator!=(const Self& s){
        return _node != s._node;
    }
    Self& operator++(){
        if (_node->_righttype == THREAD)  
            _node = _node->_right;
        else{
            Node* sub = _node->_right;
            while (sub && sub->_lefttype == LINK){
                sub = sub->_left;
            }
            _node = sub;
        }
        return *this;
    }
protected:
    pNode _node;
};

template <class T>
class __BinaryTreeThread{
    typedef __BinaryTreeNode<T> Node;
    typedef __BinaryTreeNode<T>* pNode;
public:
    typedef __InOderThreadIterator<T> Iterator;
public:
    __BinaryTreeThread()
        :_root(NULL)
    {}
    __BinaryTreeThread(T* a, size_t n, const T& invalid)
    {
        size_t index = 0;
        _root = __CreatTree(a, n, invalid, index);
    }
    ~__BinaryTreeThread()
    {
        _Destroy(_root);
    }

    Iterator begin(){
        Node* sub = _root;
        while (sub && sub->_left){
            sub = sub->_left;
        }
        return sub;
    }
    void InOderNoR()
    {
        if (NULL == _root){return;}

		std::stack<Node*> s;
        Node* cur = _root;

        while (cur || !s.empty())
        {
            while (cur)
            {
                s.push(cur);
                cur = cur->_left;
            }

            Node* top = s.top();
            s.pop();

			std::cout<<top->_data<<" ";
            cur = top->_right;
        }
		std::cout<<std::endl;
    }

   void __InOderThread(Node* root, Node*& prev){
	   if (NULL == root)   return;
       if (root->_lefttype == LINK)
		   __InOderThread(root->_left, prev);
   	   if (NULL == root->_left){
       	  root->_left = prev;
          root->_lefttype = THREAD;
    	}

    	if (prev && NULL == prev->_right){
        	prev->_right = root;
        	prev->_righttype = THREAD;
    	}	

    	prev = root;

   		 if (root->_righttype == LINK)
			 __InOderThread(root->_right, prev);
  	 }	
    void InOderThread(){
        Node* prev = NULL;
        __InOderThread(_root, prev);
    }
	Iterator end(){
		return NULL;
	}
protected:
    Node* __CreatTree(T* a, size_t n, const T& invalid, size_t& index)
    {
       assert(NULL != a && n > 0);
       Node* root = NULL;
       if (index < n && a[index] != invalid)
       {
           root = new Node(a[index]);
           root->_left = __CreatTree(a, n, invalid, ++index);
           root->_right = __CreatTree(a, n, invalid, ++index);
       }
       return root;
    }

   void  _Destroy(Node* root)
   {
       if (NULL == root){return;}
       _Destroy(root->_left);
       _Destroy(root->_right);
       delete root;
   }


protected:
    Node* _root;
};
#endif //__BTTHREAD_H__
