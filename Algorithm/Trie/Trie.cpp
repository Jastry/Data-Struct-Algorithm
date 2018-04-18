#include <iostream>

class DictionaryTree {
public:
	DictionaryTree() : m_root(NULL){}
	void ParsingPath(std::string url, std::vector<std::string> & path) {
		if (url.size() <= 0)	return;
		int pos = 0;
		std::vector< int > idx;
		for (int i = 0; i < url.size(); ++i) {
			if (url[i] == '/') {
				idx.push_back(i);
			}
		}
		if (idx.empty())
			return;
		idx.push_back(url.size());

		for (int i = 0; i < idx.size() - 1 ; ++i) {
			path.push_back(url.substr(idx[i], idx[i + 1] - idx[i]));
		}
	}

	bool Insert(const std::string & url, bool Insert = true) {
		std::vector<std::string> path;

		ParsingPath(url, path);
		if (m_root == NULL) {
			m_root = new Node("www.kujiale.com");
		}

		Node * tmp = m_root;

		int idx = 0;
		for (int i = 1; i < tmp->m_next.size();) {
			if (idx < path.size() && path[idx] == tmp->m_next[i]->m_file) {
				tmp = tmp->m_next[i];
				idx++;
				i = 0;
			}
			else
				++i;
		}

		if (!Insert) {
			if (idx <= path.size())
				return true;
			else
				return false;
		}

		while (idx < path.size()) {
			Node * node = new Node(path[idx++]);
			tmp->m_next.push_back(node);
			tmp = tmp->m_next[tmp->m_next.size()-1];
		}
		return true;
	}

	bool FindUrl(std::string & url) {
		return Insert(url, false);
	}

private:
	Node * m_root;
};

int main()
{
	std::string url = "www.kujiale.com/a/b/c/d";
	DictionaryTree tree;
	tree.Insert(url);

	tree.Insert("www.kujiale.com/aa/bb/cc/dd");
	tree.Insert("www.kujiale.com/aa/bbb/ccc/ddd");
	tree.Insert("www.kujiale.com/aa/bbbb/cccc/dddd");
	tree.Insert("www.kujiale.com/aa/bbbb/cccc/dddd");
	std::string str = "www.kujiale.com/aa/bbbb/c";

	bool res = tree.FindUrl(str);

	return 0;
}

