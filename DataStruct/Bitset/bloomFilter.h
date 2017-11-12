#ifndef __BLOOMFILTER_H__
#define __BLOOMFILTER_H__

#include "bitset.h"
#include "hashFunc.h"

struct __HashFunc1{
	size_t operator()(const std::string str)
	{
		return BKDRHash(str.c_str());
	}
};

struct __HashFunc2{
	size_t operator()(const std::string str)
	{
		return JSHash(str.c_str());
	}
};

struct __HashFunc3{
	size_t operator()(const std::string str)
	{
		return SDBMHash(str.c_str());
	}
};

struct __HashFunc4{
	size_t operator()(const std::string str)
	{
		return RSHash(str.c_str());
	}
};

struct __HashFunc5{
	size_t operator()(const std::string str)
	{
		return APHash(str.c_str());
	}
};

template <class K = std::string, 
	class HashFunc1 = __HashFunc1,
	class HashFunc2 = __HashFunc2,
	class HashFunc3 = __HashFunc3,
	class HashFunc4 = __HashFunc4,
	class HashFunc5 = __HashFunc5>
struct BloomFilter{
public:
	BloomFilter(size_t size)
		:_range(size*10)
		,_bitset(size*10)
	{}
	void Set(const K& key)
	{
		size_t hash1 = HashFunc1()(key) % _range;
		size_t hash2 = HashFunc2()(key) % _range;
		size_t hash3 = HashFunc3()(key) % _range;
		size_t hash4 = HashFunc4()(key) % _range;
		size_t hash5 = HashFunc5()(key) % _range;

		_bitset.Set(hash1);
		_bitset.Set(hash2);
		_bitset.Set(hash3);
		_bitset.Set(hash4);
		_bitset.Set(hash5);
	}

	bool Test(const K& key)
	{
		if (!_bitset.Test(HashFunc1()(key) % _range))
			return false;
		if (!_bitset.Test(HashFunc2()(key) % _range))
			return false;
		if (!_bitset.Test(HashFunc3()(key) % _range))
			return false;
		if (!_bitset.Test(HashFunc4()(key) % _range))
			return false;
		if (!_bitset.Test(HashFunc5()(key) % _range))
			return false;
		return true;
	}

	void Reset(const K& key);
protected:
	//一开始就开到最大值
	Bitset _bitset;
	size_t _range;
};

#endif //__BLOOMFILTER_H__
