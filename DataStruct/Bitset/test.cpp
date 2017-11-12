#include "bitset.h"
#include "bloomFilter.h"
int main()
{
	Bitset set(-1);
	set.Set(4);
	set.Set(16);
	set.Set(24);
	bool ret = set.Test(4);
	ret = set.Test(16);
	ret = set.Test(24);
	ret = set.Test(20);
	set.reSet(16);
	set.reSet(24);
	set.reSet(20);
	set.reSet(15);
	ret = set.Test(16);
	ret = set.Test(24);
	ret = set.Test(20);
	ret = set.Test(15);
	return 0;
}
