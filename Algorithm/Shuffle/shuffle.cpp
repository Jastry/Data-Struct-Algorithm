#include <iostream>
#include <cstdio>
#include <ctime>
#include <vector>


/* 随机洗牌算法 */
#if 1

#define BUFFSIZE 54

class Solution {
public:
	Solution( void ) {
		// 1 ~ 55
		m_v.resize(BUFFSIZE + 1);
		for (int i = 1; i < m_v.size(); ++i) {
			m_v[i] = i;
		}
	}

	const std::vector<int>& getResult(void) {
		return m_v;
	}

	void shuffle(void) {
		std::srand(std::time(nullptr));
		int rand_idx = 0;
		for (int i = 1; i < m_v.size(); ++i) {
			rand_idx = 1 + std::rand() / ((RAND_MAX + 1u) / BUFFSIZE);
			//rand_idx = 1 + (std::rand() % BUFFSIZE);
			std::swap(m_v[i], m_v[rand_idx]);
		}
	}

	void shuffle_pro(void) {
		std::srand(std::time(nullptr));
		int rand_idx = 0;
		for (int i = 1; i < m_v.size(); ++i) {
			rand_idx = i + rand() % (BUFFSIZE - i + 1);
			//rand_idx = 1 + (std::rand() % BUFFSIZE);
			std::swap(m_v[i], m_v[rand_idx]);
		}
	}

	bool isLegal(void) {
		int legal[55] = {0};
		for (int i = 1; i < m_v.size(); ++i) {
			legal[m_v[i]]++;
		}
		for (int i = 1; i < m_v.size(); ++i) {
			if (legal[i] == 0)	
				return false;
		}
		return true;
	}

	void display(void) {
		printf("%s\n", "After the shuffle: ");
		for (int i = 0; i < m_v.size(); ++i) {
			printf("%d\t", m_v[i]);
		}
		printf("\n");
	}

private:
	std::vector<int> m_v;
};

int main() {
	Solution so;
	for (int i = 0; i < BUFFSIZE; i++) {
		const std::vector<int> res = so.getResult();
		so.shuffle_pro();
		if (!so.isLegal()) {
			printf("%s\n", "there is something wrong");
		}
		//so.display();
	}
	return 0;

}
#endif
