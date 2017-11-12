#include "rb_tree.h"
#include "my_map.h"
#include <utility>
#include <time.h>
#include <iostream>

int main()
{
	my_map<int, int> mp;
	mp.insert(std::pair<int, int>(1,3));
	mp[6] = 7;
	mp[7] = 6;
	my_map<int, int>::Iterator it = mp.Begin();
    while (it != mp.End()){
        std::cout << it->first << ":" << it->second << " ";
        ++it;
    }
    std::cout << std::endl;
    return 0;
}
