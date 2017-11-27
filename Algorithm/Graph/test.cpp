#include "../Union/UnionFindSet.h"
#include "Graph.hpp"

int main()
{
#if 0	
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
#endif	

#if 0
	std::string strs[] = {"北京", "上海", "南京", "西安", "成都"};
	GraphMartix<std::string, int> gm(strs, sizeof (strs) / sizeof (std::string));
	gm.AddEdge("北京", "上海", 10);
	gm.AddEdge("北京", "南京", 20);
	gm.AddEdge("北京", "西安", 25);
	gm.AddEdge("西安", "上海", 15);

#endif
#if 1
	std::string strs[] = { "北京", "上海", "南京", "西安", "成都" };
	GraphLinkTable<std::string, int> gm(strs, sizeof (strs) / sizeof (std::string));
	gm.AddEdge("北京", "上海", 10);
	gm.AddEdge("北京", "南京", 20);
	gm.AddEdge("北京", "西安", 25);
	gm.AddEdge("西安", "上海", 15);
	gm.AddEdge("成都", "上海", 30);

	gm.DFS();
	gm.BFS();

	GraphLinkTable<std::string, int> ggm;
	bool ret = gm.Kruskal(ggm);
#endif
	return 0;
}

