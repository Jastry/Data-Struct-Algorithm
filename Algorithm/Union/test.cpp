#include "UnionFindSet.h"

int main()
{
	UnionFindSet set(8);
	set.SetUnionFriend(1, 2);
	set.SetUnionFriend(3, 2);

	set.SetUnionFriend(4, 5);
	set.SetUnionFriend(6, 5);

	set.SetUnionFriend(7, 8);

	set.SetUnionFriend(4, 2);
	size_t ret = set.UnionSize();

	set.SetUnionFriend(4, 8);

	ret = set.UnionSize();
	return 0;
}

