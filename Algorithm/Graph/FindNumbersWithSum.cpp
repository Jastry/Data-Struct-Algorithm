#include <iostream>

/* 一个递增数组，他们的和恰好是 s, 输出两个数乘积最小的 */
#if 1
class Solution {
public:
	std::vector<int> FindNumbersWithSum(std::vector<int> array, int sum) {
		std::vector<int> res;
		if (sum == 0 || array.size() < 2)
			return array;

		int left = 0, right = array.size() - 1;
		while (left < right) {
			int tamp = array[left] + array[right];
			if (tamp == sum) {
				res.push_back(array[left]);
				res.push_back(array[right]);
				return res;
			}
			else if (tamp > sum)
				right--;
			else
				left++;
		}
		return res;
	}
};

int main()
{
	std::vector<int> a = {1, 2, 3, 4, 5, 6, 7, 8,9};
	Solution so;
	std::vector<int> res = so.FindNumbersWithSum(a, 11);
	return 0;
}

#endif

