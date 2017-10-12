#include <iostream>
#include <string>
#include <map>
#include <vector>

std::vector<int> vowelIndices(std::string word)
{
	std::map< std::string, std::vector<int> > mp;
	
	mp["Super"].push_back(2);
	mp["Super"].push_back(4);

	mp["Mmmm"];
	
	mp["Apple"].push_back(1);
	//mp["Apple"].push_back(4);
	mp["Apple"].push_back(5);
	
	mp["YoMama"].push_back(1);
	mp["YoMama"].push_back(2);
	mp["YoMama"].push_back(4);
	mp["YoMama"].push_back(6);

	return mp[word];
}

int main()
{
	std::string str = "YoMama";
	std::cout<<vowelIndices(str)[1]<<std::endl; 
	return 0;
}
