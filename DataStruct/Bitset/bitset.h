#ifndef __BITSET_H__
#define __BITSET_H__

#include <iostream>
#include <vector>
#include <string>
//#include <>

class Bitset
{
public:
	Bitset(size_t range)
		:_size(range)
		, _count(0)
	{
		_bmp.resize((range >> 3) + 1);
	}
	Bitset()
		:_size(0xffffffff)
		, _count(0)
	{
		_bmp.resize((size_t)-1 >> 3);
	}
	bool Test(size_t pos)
	{
		size_t index = pos >> 3;
		size_t num = pos % 8;
		return _bmp[index] & (1 << num);
	}
	void Set(size_t pos)
	{
		size_t index = pos >> 3;
		size_t num = pos % 8;
		_bmp[index] |= (1 << num);
		_count++;
		return;
	}
	void reSet(size_t pos)
	{
		size_t index = pos >> 3;
		size_t num = pos % 8;
		_bmp[index] &= (~(1 << num));
	}
	size_t Count()
	{
		return _count;
	}
	size_t Size()
	{
		return _size;
	}

	void clear()
	{
		size_t size = _bmp.size();
		_bmp.clear();
		_bmp.resize(size);
	}
protected:
	size_t _count;//num of one
	size_t _size; //num of zero
	std::vector<char> _bmp;
};
#endif
