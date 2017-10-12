#include <iostream>
#include <vector>
#include <string>
#include <fstream>

/*
template<class TYPE>
int getLengthOfMissingArray(std::vector< std::vector< TYPE > > arrayOfArrays)
{
 	int sz = arrayOfArrays.size();

	std::vector<int> v;
 	v.resize(sz);

 	for (int i = 0; i < sz; ++i){
 		v[arrayOfArrays[i].size()] = 1;
 	}

 	for (int i = 0; i < sz; ++i){
 		if (v[i] == 0)
 			return i;
 	}
}


int main()
{
	std::vector< std::vector<int> > v;
	v.resize(5);
	v[0].resize(2);
	v[1].resize(3);
	v[2].resize(4);
	v[4].resize(5);
	std::cout<<getLengthOfMissingArray(v)<<std::endl;
	return 0;
}
*/

void UnCompress(std::string& filename){
	int pos = filename.find(".");
	std::string substr = filename.substr(pos, 4);
	std::cout<<substr<<std::endl;
	//assert(filename);
}

int main()
{
	std::string str = "log.hzp";
	UnCompress(str);
	return 0;
}
