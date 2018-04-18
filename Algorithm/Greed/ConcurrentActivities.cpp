#include "fun.h"

/* 计算最多并发活动数量 */
typedef struct ActiveTime {
	//精确到分0.00 ~ 24.59
	float m_start;
	float m_finsh;
}Time;


struct CmpFinsh {
	bool operator()(const Time& left, const Time& right) {
		return left.m_finsh < right.m_finsh;
	}

	bool operator()(float left, float right) {
		return left < right;
	}
};

struct CmpStart {
	bool operator()(const Time& left, const Time& right) {
		return left.m_start < right.m_start;
	}
};

class Solution {
	typedef struct ActiveTime Time;
public:
	int CommonAcitive(std::vector< Time >& t_v) {
		if (t_v.size() <= 1)	return t_v.size();
		std::sort(t_v.begin(), t_v.end(), CmpStart());
		std::vector<int> cnts(t_v.size(), 1);

		std::vector<float> finsh;
		finsh.push_back(t_v[0].m_finsh);
		int max = 1;
		for (int i = 1; i < t_v.size(); ++i) {
			int cur_max = 1;
			for (int j = 0; j < i; ++j) {
				if (t_v[i].m_start < finsh[j])	cur_max++;
				else break;
			}
			if (cur_max > max) max = cur_max;
			finsh.push_back(t_v[i].m_finsh);
			std::make_heap(finsh.begin(), finsh.end(), CmpFinsh());
		}
		return max;
	}
};

int main() {
	std::vector<Time> v;
	float v_start[] = { 1.30, 4.20, 6.55, 7.15, 8.10, 9.45, 10.30, 11.00 };
	float v_finsh[] = { 7.30, 6.50, 16.10, 9.55, 11.20, 20.00, 11.45, 22.30};
	std::vector<Time> v_t;
	v_t.resize(sizeof(v_start) / sizeof(v_start[0]));
	for (int i = 0; i < v_t.size(); ++i) {
		v_t[i].m_finsh = v_finsh[i];
		v_t[i].m_start = v_start[i];
	}
	Solution so;
	int res = so.CommonAcitive(v_t);
	return 0;
}

