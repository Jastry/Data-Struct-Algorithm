#include "UnionFindSet.h"


void UnionFindSet::SetUnionFriend(size_t left, size_t right)
{
	assert(left < _set.size());
	assert(right < _set.size());
	size_t left_parent = _FindParent(left);
	size_t right_parent = _FindParent(right);

	if (left != right) {
		_set[left_parent] += _set[right_parent];
		_set[right_parent] = left_parent;
	}
}

size_t UnionFindSet::UnionSize()
{
	size_t cnt = 0;
	for (size_t i = 1; i < _set.size(); ++i) {
		if (_set[i] < 0) {
			++cnt;
		}
	}
	return cnt;
}

size_t UnionFindSet::_FindParent(size_t index)
{
	assert(index < _set.size());
	while (_set[index] > 0) {
		index = _set[index];
	}
	return index;
}
