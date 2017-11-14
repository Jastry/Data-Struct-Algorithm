#ifndef __BTREE_H__
#define __BTREE_H__

#include <iostream>
#include <string>
#include <vector>
#include <string.h>

const size_t MAX_KEY = 4;

template <class Key, class Value, size_t M = MAX_KEY>
struct BTreeNode{
    size_t _keys; //num of kv
    std::pair<Key, Value> _kvs[M];
    BTreeNode<Key, Value, M>* _subs[M+1];
    
    BTreeNode()
    {
        bzero(this, sizeof (BTreeNode));
    }
   //BTreeNode(const std::pair<Key, Value>& kv)
   // BTreeNode()
   // {
   //     _subs[0] = NULL;
   //     for (size_t i =0; i < M; ++i){
   //         _subs[i + 1] = NULL;
   //         _kvs[i] = std::pair<Key, Value>();
   //     }
   // }
};


template <class Key, class Value, size_t M=MAX_KEY>
class BTree{
public:
    typedef BTreeNode<Key, Value, M> Node;
    
public:
    BTree()
        :_root(NULL)
        ,_size(0){}
    ~BTree()
    {
       // clear(_root);
    }

    //void _clear(Node* node);
    
    //return node* & index,index type is int because not search not found return -1;
    std::pair<Node *, int> search(const Key& key)
    {
        if (!_root){
            return std::make_pair(NULL, -1);
        }

        Node* cur = _root;
        while (cur) {
            size_t sz = cur->_keys;
            size_t i = 0;
            for (; i < sz; ++i) {
                if (cur->_kvs[i].first > key) {
                    i++;
                } 
                else if (cur->_kvs[i].first < key) {
                    break;
                }
                else {
                    return std::make_pair(cur, (int)i); 
                }  
                cur = cur->_subs[i];
            }
            return std::make_pair(NULL, -1);
        }
        return std::make_pair(NULL, -1);
    }

    void insert(const std::pair<Key, Value>& kv)
    {
        if (!_root){
            _root = new Node;
            _root->_kvs[0] = kv;
            _root->_keys++;
            _size++;
        }
    }

protected:
    Node* _root;
    size_t _size;
};
#endif
