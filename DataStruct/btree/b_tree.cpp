#include "b_tree.h"

int main()
{
    BTree<int, int> t;
    t.insert(std::make_pair(1,2));
    long k = 1;
    t.search(k);
    return 0;
}
