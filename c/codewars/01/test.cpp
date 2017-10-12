#include <iostream>
#include <vector>
#include <string>
#include <queue>

using namespace std; 

struct Compara{
	bool operator ()(const string& left, const string& right){
		int left_size = left.size();
		int right_size = right.size();
		int i = 0; int j = 0;
		while (i < left_size && j < right_size){
			if ((int)left[i] <  (int)right[j]){
				return true;
			}
			else if ((int)left[i] > (int)right[j]){
				return false;
			}
			else{
				++i; ++j;
			}
		}
		
		if (left_size <= right_size)	return true;
		else if(left_size > right_size) return false;
	}	
};

class Kata
{
	typedef Compara cmp;
public:
    std::vector<std::string> sortByLength(std::vector<std::string> array)
    {
    	std::priority_queue<string,vector<string>,cmp> q;
    	int sz = array.size();
    	for (int i = 0; i < sz; ++i)
    		q.push(array[i]);
    	for (int j = 0; j < sz; ++j){
    		array[j] = q.top();
			q.pop();
		}
        return array;
    }
};


int main(int argc, char* argv[])
{
	Kata k;
	std::vector<string> array;
	array.resize(5);
	for (int i = 0; i < 5; ++i){
		string str;
		std::cin>>str;
		array[i] = str;
	}
	k.sortByLength(array);
	std::vector<string>::iterator it = array.begin();
	for (; it != array.end(); ++it){
		cout<<*it<<" ";
	}
	std::cout<<std::endl;
	return 0;
}
