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
    BinaryNode<T>* _Lchild;
    BinaryNode<T>* _Rchild;

    BinaryNode<T>(const T& data)
        :_data(data)
        ,_Lchild(NULL)
        ,_Rchild(NULL)
    {}
};
template <class T>
struct Iterator
{
  BinaryNode<T>* _prt;
  Iterator()
  :_prt(NULL){}
  Iterator(const BinaryNode<T>* prt){
    _prt = prt;
  }
  T& operator*(Iterator<T>* it){
    return &it->_data;
  }
  BinaryNode<T>* operator->(){
    return this;
  }
};
template <class T>
class BinaryTree
{
    typedef BinaryNode<T> Node;
    typedef Iterator<T>* It;
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
    /*BinaryTree<T> operator=(const BinaryTree<T> tree)
    {
        _root = _OperatorCopy(tree._root);
    }*/
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
        cout<<endl;
    }
    void PreOderNoR()
    {
        if (NULL == _root){return;}

        Node* cur = _root;
        stack<Node*> s;
        
        while (cur || !s.empty())
        {
            while(cur)
            {
                s.push(cur);
                cout<<cur->_data<<" ";
                cur = cur->_Lchild;
            }
            //ÓÒ×ÓÊ÷×ßÍêÁË
            Node*top = s.top(); //½«×îÓÒ¶ËµÄ½ÚµãÄÃ³öÀ´
            s.pop();

            cur = top->_Rchild; //½«×îÓÒ¶Ë½ÚµãµÄ×ó½Úµãµ±³É×ÓÎÊÌâ
        }
        cout<<endl;
    }
    void InOder()
    {
        _InOder(_root);
        cout<<endl;
    }

    void InOderNoR()
    {
        if (NULL == _root){return;}

        stack<Node*> s;
        Node* cur = _root;

        while (cur || !s.empty())
        {
            while (cur)
            {
                s.push(cur);
                cur = cur->_Lchild;
            }
            //ÓÒ×ÓÊ÷×ßÍê
            Node* top = s.top();
            s.pop();

            cout<<top->_data<<" ";
            cur = top->_Rchild;
        }
        cout<<endl;
    }
    void LevelOder()
   {
       queue<Node*> q;
       if (NULL != _root){q.push(_root);}   //ÏÈ½«¸ù½Úµã·ÅÈë¶ÓÁÐÖÐ

       while (!q.empty())
       {
           Node* front = q.front();//°ÑÉÏÒ»¸ö½Úµã½ÚµãÄÃ³öÀ´·ÃÎÊ

           cout<<front->_data<<" ";
           if (front->_Lchild)      //·ÃÎÊÓÒ×ÓÊ÷
           {
               q.push(front->_Lchild);
           }

           if (front->_Rchild)  //·ÃÎÊ×ó×ÓÊ÷
           {
               q.push(front->_Rchild);
           }

           q.pop(); //½«ÉÏÒ»¸ö¸ù½Úµãpopµô
       }
       cout<<endl;
   }

    void PostOder()
    {
        _PostOder(_root);
        cout<<endl;
    }

    void PostOderNoR()
    {
        if (NULL == _root){return;}

        stack<Node*> s;
        Node* cur = _root;
        Node* prev = NULL;

        while (cur || !s.empty())
        {
            while (cur)
            {
                s.push(cur);
                cur = cur->_Lchild;
            }

            Node* front = s.top();
            if (NULL == front->_Rchild || prev == front->_Rchild)
            {
                cout<<front->_data<<" ";
                prev = front;
                s.pop();
            }
            else
            {
                cur = front->_Rchild;
            }
        }    
        cout<<endl;
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
protected:
   Node* _CopyBinaryTree(Node* root)
   {
       if (NULL == root){return NULL;}
       Node* NewNode = new Node(root->_data);//¿½±´µ±Ç°¸ù½Úµã
       NewNode->_Lchild = (_CopyBinaryTree(root->_Lchild));//¿½±´×ó×ÓÊ÷
       NewNode->_Rchild = (_CopyBinaryTree(root->_Rchild));//¿½±´ÓÒ×ÓÊ÷
       return NewNode;
   }

   Node* _CreatTree(T* a, size_t n, const T& invalid, size_t& index)
   {
       assert(NULL != a && n > 0);
       Node* root = NULL;
       if (index < n && a[index] != invalid)
       {
           root = new Node(a[index]);
           root->_Lchild = _CreatTree(a, n, invalid, ++index);
           root->_Rchild = _CreatTree(a, n, invalid, ++index);
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
       if (NULL == root){return NULL;}

       if (data == root->_data){return root;}

       Node* Left = _Find(root->_Lchild,data);
       return Left != NULL ? Left:_Find(root->_Rchild,data);
   }
   void  _Destroy(Node* root)
   {
       if (NULL == root){return;}
       _Destroy(root->_Lchild);
       _Destroy(root->_Rchild);
       //cout<<root->_data<<" ";
       delete root;
   }
   void  _PreOder(Node* root)
   {
       if (NULL == root){return;}
       cout<<root->_data<<" ";
       _PreOder(root->_Lchild);
       _PreOder(root->_Rchild);
   }

   void  _PostOder(Node* root)
   {
       if (NULL == root){return;}
       _PostOder(root->_Lchild);
       _PostOder(root->_Rchild);
       cout<<root->_data<<" ";
   }

   void  _InOder(Node* root)
   {
       if (NULL == root){return;}
       _InOder(root->_Lchild);
       cout<<root->_data<<" ";
       _InOder(root->_Rchild);
   }
   size_t _Size(Node* root)
   {
       if (NULL == root){return NULL;}
       return _Size(root->_Lchild) + _Size(root->_Rchild) + 1; 
   }
   size_t _LeafSize(Node* root)
   {
       if (NULL == root){return NULL;}
       
       if (NULL == root->_Lchild && NULL == root->_Rchild){return 1;}   //ÕÒµ½Ò»¸öÒ¶×Ó½Úµã
        
       return  _LeafSize(root->_Lchild) + _LeafSize(root->_Rchild);
   }
   size_t _GetKLevelSize(Node* root,size_t K)
   {
       if (NULL == root){return NULL;}

       if (K == 1){return 1;} //µ½´ïµÚK²ã
                
                /*ÓÒ×ÓÊ÷µÄµÚK-1²ã½×½ÚµãÊý   +   ×ó×ÓÊ÷µÄµÚK-1²ã½×½ÚµãÊý*/
       return _GetKLevelSize(root->_Lchild,K-1) + _GetKLevelSize(root->_Rchild,K-1);
   }
   size_t _Depth(Node* root)
   {
       if (NULL == root){return 0;}

       if (NULL == root->_Lchild && NULL == root->_Rchild){return 1;}

       size_t Left = _Depth(root->_Lchild);
       size_t Right = _Depth(root->_Rchild);
       return Left > Right? Left + 1:Right+1;
   }
   size_t g_d(Node* r)
   {
       if (NULL == r)
           return 0;
       if (NULL == r->_Lchild && NULL == r->_Rchild)
           return 1;
       size_t ll = g_d(r->_Lchild);
       size_t rr = g_d(r->_Rchild);
       return ll > rr ? ll+1 : rr+1;    
   }
protected:
    It iterator;
    Node* _root;
};
void TestBinaryTree()
{
    int a[] = {1,2,3,'#','#',4,'#','#',5,6};
    int b[] = {1,2,4,'#','#',5,8,'#',9,10,'#','#','#','#',3,6,'#',11,'#',12,13,'#',14,'#','#','#',7};
    
    //BinaryTree<int> tree(a,sizeof(a)/sizeof(a[0]),'#');
    BinaryTree<int> tree(b,sizeof(b)/sizeof(b[0]),'#');
    size_t ret = tree.Depth();
    /*tree.PreOderNoR();
    tree.InOderNoR();
    tree.PostOderNoR();*/

    /*BinaryTree<int> tree2(tree);
    BinaryTree<int> tree3 = tree2;

    BinaryNode<int>* cur = tree.Find(1);
    cur = tree.Find(2);
    cur = tree.Find(3);
    cur = tree.Find(4);
    cur = tree.Find(5);
    cur = tree.Find(6);
    cur = tree.Find(7);*/

    /*tree.PreOder();
    tree.PostOder();
    tree.InOder();
    tree.LevelOder();
    
//    cout<<tree.Depth()<<endl;
//    cout<<tree.Size()<<endl;
//    cout<<tree.LeafSize()<<endl;*/
//
//    /*cout<<tree.GetKLevelSize(0)<<endl;
//    cout<<tree.GetKLevelSize(1)<<endl;
//    cout<<tree.GetKLevelSize(2)<<endl;
//    cout<<tree.GetKLevelSize(3)<<endl;
//    cout<<tree.GetKLevelSize(4)<<endl;*/
}
#endif //__BINARYTREE_H__ 

