#ifndef __HSTABLE_H__
#define __HSTABLE_H__

#include <iostream>
#include <vector>
#include <map>
#include <string>
static const unsigned long sprime_list[28] =
{
	53ul, 97ul, 193ul, 389ul, 769ul,
	1543ul, 3079ul, 6151ul, 12289ul, 24593ul,
	49157ul, 98317ul, 196613ul, 393241ul, 786433ul,
	1572869ul, 3145739ul, 6291469ul, 12582917ul, 25165843ul,
	50331653ul, 100663319ul, 201326611ul, 402653189ul, 805306457ul,
	1610612741ul, 3221225473ul, 4294967291ul
};

inline unsigned long _get_next_prime(unsigned long n)
{

	int i = 0;//素数表一共有28个元素，所以定值为28
	for (; i < 28; ++i){
		if (sprime_list[i] > n && i != 27){
			return sprime_list[i];
		}
	}
	//如果超过了素数表第26个的数量，直接返回最大元素
	return sprime_list[27];
}

namespace OPEN{
	using namespace std;
	//哈希表中每个节点状态
	enum state{
		EXIST,
		DELETE,
		EMPTY
	};

	template <class K, class V>
	struct Hs_node{
		state _st;
		K _key;
		V _value;

		Hs_node()//初始化时，将节点状态设置为空
			:_st(EMPTY)
		{}
	};

	template <class K>
	struct __HashFunc
	{
		size_t operator()(const K& key){
			return key;
		}
	};

	template<>
	struct __HashFunc<string>
	{
		static size_t BKDRHash(const char * str)
		{
			unsigned int seed = 131; // 31 131 1313 13131 131313
			unsigned int hash = 0;
			while (*str)
			{
				hash = hash * seed + (*str++);
			}
			return (hash & 0x7FFFFFFF);
		}

		size_t operator()(const string& key)
		{
			return  BKDRHash(key.c_str());
		}
	};


	template <class K, class V, class Hash_func = __HashFunc<K> >
	class Hs_table{
		typedef Hs_node<K, V> Node;

	public:
		Hs_table()
			:_size(0)
		{}
		~Hs_table()
		{
		}
		bool insert(const K& k, const V& v)
		{
			_checkCapacity();
			size_t index = hash_func(k);
			int i = 1;
			while (_table[index]._st == EXIST){
				if (_table[index]._key == k){
					return false;
				}

				/**************二次探测********************/
				index += i;
				i *= i;
				if (index > _table.size())
				{
					index %= _table.size();
				}
				i++;
				/******************线性探测****************/
				/*index++;
				if (index == _table.size()){
				index = 0;
				}*/
			}
			_table[index]._st = EXIST;
			_table[index]._key = k;
			_table[index]._value = v;
			_size++;
			return true;
		}
		Node* find(const K& k)
		{
			size_t index = hash_func(k);
			int i = 1;
			while (_table[index]._st != EMPTY){
				if (_table[index]._key == k && _table[index]._st == EXIST){
					//注意查找时需要判断是否存在，结束条件
					return &_table[index];
				}
				/***************线性探测*****************/
				//index++;
				//if (index == _table.size()){
				//	index = 0
				//}
				/***************二次探测*****************/
				index += i;
				i *= i;
				if (index > _table.size()){
					index %= _table.size();
				}
				i++;
			}
			return NULL;
		}
		bool erase(const K& k)
		{
			if (!find(k)){
				return false;
			}
			Node* tamp = find(k);
			tamp->_st = DELETE;
			_size--;
			return true;
		}
		size_t size()
		{
			return _size;
		}
		size_t capacity()
		{
			return _table.size();
		}
	protected:
		void _checkCapacity()
		{
			if (_table.size() == 0){	//插入第一个节点
				_table.resize(_get_next_prime(1));
			}
			if (_size * 10 / _table.size() >= 7)//负载因子过大时，降低了 CPU 对数据的命中率。扩容
			{
				Hs_table<K, V> new_table;
				new_table._table.resize(_get_next_prime(_table.size()));
				for (size_t i = 0; i < _table.size(); ++i){
					if (_table[i]._st == EXIST){
						new_table.insert(_table[i]._key, _table[i]._value);
					}
				}
				_table.swap(new_table._table);
			}
		}
		
		size_t hash_func(const K& key){
			Hash_func hash;
			return hash(key) % _table.size();
		}
	protected:
		std::vector<Node> _table;//vector 里放的必须是 Node，需要查看状态。
		size_t _size;
	};
}

namespace BUCK{

	template <class K, class V>
	struct hash_node
	{
		std::pair<K, V> _kv;
		hash_node<K, V>* _next;

		hash_node(const std::pair<K, V> kv)
			:_kv(kv)
			, _next(NULL)
		{}
	};
	
	template <class K>
	struct __HashFunc{
		size_t operator()(const K& key)
		{
			return key;
		}
	};

	template <>
	struct __HashFunc<std::string>
	{
		static size_t BKDRHash(const char * str)
		{
			unsigned int seed = 131; // 31 131 1313 13131 131313
			unsigned int hash = 0;
			while (*str)
			{
				hash = hash * seed + (*str++);
			}
			return (hash & 0x7FFFFFFF);
		}

		size_t operator()(const std::string& key)
		{
			return  BKDRHash(key.c_str());
		}
	};
	
	template<class K, class V, class HashFunc>
	class hash_table;

	template <class K , class V, class HashFunc = __HashFunc<K> >
	struct hsIterator{
		typedef hash_node<K, V> Node;
		typedef hsIterator<K, V, HashFunc> Self;
		Node* _node;
		hash_table<K, V, HashFunc>* _ht;

		hsIterator(Node* node, hash_table<K, V, HashFunc>* ht)
			:_ht(ht)
			, _node(node)
		{}
		
		std::pair<K, V>& operator*()
		{
			return _node->_kv;
		}

		std::pair<K, V>* operator->()
		{
			return &(operator*());
		}

		Self& operator++()
		{
			Node* next = _node->_next;
			if (next){
				_node = next;
			}
			else {
				HashFunc hf;
				size_t index = hf(_node->_kv.first) % _ht->_tables.size();
				_node = NULL;//保证当node为最后一个节点时，可以返回空
				//i 从 index + 1 开始
				for (size_t i = index + 1; i < _ht->_tables.size(); ++i){
					if (_ht->_tables[i]){
						_node = _ht->_tables[i];
						break;
					}
				}
			}
			return *this;
		}
		
		bool operator!=(const Self& it)
		{
			return _node != it._node;
		}
	};
	
	template< class K, class V, class HashFunc = __HashFunc<K> >
	class hash_table{
		typedef hash_node<K, V> Node;
	public:
		typedef hsIterator<K, V, HashFunc> Iterator;
		friend struct hsIterator<K, V, HashFunc>;
	public:
		hash_table()
			:_size(0)
		{}
		
		~hash_table()
		{}

		Iterator End()
		{
			return Iterator(NULL, this);
		}
		
		Iterator Begin()
		{
			for (size_t i = 0; i < _tables.size(); ++i){
				if (_tables[i]){
					return Iterator(_tables[i], this);
				}
			}
			return End();
		}

		std::pair<Iterator, bool> Insert(const std::pair<K, V>& kv)
		{
			_checkCapacity();
			size_t index = _hash_func(kv.first, _tables.size());
			Node* cur = _tables[index];//find ops
			while (cur){
				if (cur->_kv.first == kv.first){//unique_insert
					return std::make_pair(Iterator(cur, this), false);
				}
				cur = cur->_next;
			}
			//头插
			Node* tamp = new Node(kv);
			tamp->_next = _tables[index];
			_tables[index] = tamp;
			++_size;

			return std::make_pair(Iterator(tamp, this), true);
		}
		
		bool Insert(Iterator it){
			std::pair<Iterator, bool> ret = Insert(it->_node->_kv);
			return ret->second;
		}
		
		V& operator[](const K& key)
		{
			std::pair<Iterator, bool> ret = Insert(make_pair(key, V()));
			return ret.first->second;
		}

		void Resize(size_t size)
		{
			Iterator it = Begin();
			std::vector<Node*> new_tables;
			new_tables.resize(size);
			new_tables.swap(_tables);
			while (it != End()){
				Insert(it);
				++it;
				delete it->_node;
			}
		}

	private:
		size_t _hash_func(const K& key, size_t size)
		{
			HashFunc hf;
			return hf(key) % size;
		}

		void _checkCapacity()
		{
			if (0 == _tables.size())
			{
				_tables.resize(_get_next_prime(1));
				return;
			}
			//平衡因子改为 1
			if (_size / _tables.size() >= 1){
				std::vector<Node*> new_tables(_get_next_prime(_tables.size()), NULL);
				//开新空间，重新插入
				//HashFunc hf;
				for (size_t i = 0; i < _tables.size(); ++i){
					while (_tables[i]){
						Node* cur = _tables[i];
						Node* next = cur->_next;
						//size_t index = hf(cur->_kv.first % new_tables.size());
						size_t index = _hash_func(cur->_kv.first, new_tables.size());

						Node* head = new_tables[index];
						new_tables[index] = cur;
						cur->_next = head;
						_tables[i] = next;
					}
				}
				_tables.swap(new_tables);
			}
		}
	protected:
		std::vector<Node*> _tables;
		size_t _size;
	};
}

#endif //__HSTABLE_H__
