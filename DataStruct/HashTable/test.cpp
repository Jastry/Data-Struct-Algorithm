#include "hash_table.h"

using namespace std;
using namespace BUCK;

void TestHashTable()
{
	/*hash_table<int, int> ht;
	for (size_t i = 0; i < 53; ++i)
	{
		ht.Insert(make_pair(rand() % 100000, i));
	}
	ht.Insert(make_pair(3,4));*/
	/*ht.Insert(make_pair(4,5));
	ht.Insert(make_pair(5,6));*/

	hash_table<string, string> dict;
	dict.Insert(make_pair("left", "左边"));
	dict.Insert(make_pair("right", "右边"));
	dict.Insert(make_pair("right1", "右边"));
	dict.Insert(make_pair("right2", "右边"));
	dict.Insert(make_pair("right3", "右边"));
	dict.Insert(make_pair("right4", "右边"));
	dict.Insert(make_pair("left", "剩余"));
	dict["left"] = "剩余";

	hash_table<string, string>::Iterator it = dict.Begin();
	while (it != dict.End())
	{
		cout<<it->first<<":"<<it->second<<" "<<endl;
		++it;
	}
	//cout<<endl;

}

int main()
{
	TestHashTable();
	return 0;
}
