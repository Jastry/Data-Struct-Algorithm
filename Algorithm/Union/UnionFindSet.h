#ifndef __UNIONFINDSET_H__
#define __UNIONFINDSET_H__

#include <iostream>
#include <vector>
#include <cassert>

class UnionFindSet{
public:
	UnionFindSet(size_t n)
	{
		_set.resize(n + 1, -1);
	}

	void SetUnionFriend(size_t left, size_t right);

	size_t UnionSize();
private:
	size_t _FindParent(size_t index);
protected:
	std::vector<int> _set; 
};

#endif	//__UNIONFINDSET_H__
