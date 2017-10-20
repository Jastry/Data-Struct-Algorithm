#include <iostream>
#include <vector>
#include <map>
#include <>
//#include <bits/unordered_map.h>

using namespace std;

class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
		int num = nums.size();
		vector< vector<int> > v;
		v.resize(num);
		for (int i = 0; i < num; ++i){
			v[i].resize(num);
			v[0][i] = v[i][0] = nums[i];
		}
		nums.clear();
		for (int i = 0; i < num; ++i){
			for (int j = 0; j < num; ++j){
				if (i != j && v[0][j] + v[i][0] == target){
					nums.resize(2);
					nums[0] = v[i][0];nums[1] = v[0][j];
					return nums;
				}
			}
		}
    }
};

vector<int> twoSum(vector<int> &numbers, int target)
{
    //Key is the number and value is its index in the vector.
	unordered_map<int, int> hash;
	vector<int> result;
	for (int i = 0; i < numbers.size(); i++) {
		int numberToFind = target - numbers[i];

            //if numberToFind is found in map, return them
		if (hash.find(numberToFind) != hash.end()) {
                    //+1 because indices are NOT zero based
			result.push_back(hash[numberToFind] + 1);
			result.push_back(i + 1);			
			return result;
		}

            //number was not found. Put it in the map.
		hash[numbers[i]] = i;
	}
	return result;
}

/*
class Solution1 {
public:
	vector<int> twoSum(vector<int>& v, int target) {
		vector<int> result;
		unordered_map<int, int> mp;
		int number = v.size();
		for (int i = 0; i < number; ++i){
			int numberToFind = target - v[i];
			//find 失败返回？？？ 
			if ( hash.find(numberToFind) != hash.end()){
				// +1 是因为下标不是从零开始的 
				result.push_back(hash[numberToFind] + 1);
				result.push_back(i + 1);
				return result;
			}
			hash[v[i]] = i;
		}
		return result;
	}
};
*/
int main()
{
	Solution s;
	vector<int> v;
	v.resize(4);
	v[0] = 2;
	v[1] = 7;
	v[2] = 11;
	v[3] = 15;

	//v = s.twoSum(v,9);
	vector<int> rv = twoSum(v, 9);
	//cout<<v.size()<<endl;
	for (int i = 0; i < v.size(); ++i){
		cout<<rv[i]<<endl;
	}
	return 0;
}
