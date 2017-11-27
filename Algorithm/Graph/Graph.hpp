/*
 * vs 2013
 * 实现邻接矩阵GraphMatrix
 * 实现邻接表  GraphLinkTable
 * 利用 UnionFindSet (并查集) 实现 Prim 算法，求最小生成树
 * 联系邮箱 blbagony@163.com
 *
 */


#ifndef __GRAPH_H__
#define __GRAPH_H__

#include "../Union/UnionFindSet.h"
#include <queue>
#include <stack>
#include <map>
#include <string>
#include <queue>

template <class V, class W, bool IsDirected = false>	//默认为无向图
class GraphMartix{
public:
	GraphMartix(const V* vertexs, size_t size)
	{
		assert(vertexs);
		_vertexs.resize(size);

		for (size_t i = 0; i < size; ++i) {
			_vertexs[i] = vertexs[i];
			_index_map[vertexs[i]] = i;
		}
		_martix.resize(size);
		for (size_t i = 0; i < size; ++i) {
			_martix[i].resize(size, W());
		}
	}
	

	void AddEdge(const std::string& src, const std::string& dst, const W& weight)
	{
		size_t src_index = _index_map[src];
		size_t dst_index = _index_map[dst];
		_martix[src_index][dst_index] == weight;
		if (!IsDirected) {
			_martix[dst_index][src_index] = weight;
		}
	}

	size_t GetVertexsIndex(const V& v)
	{
		assert(_index_map.count(v));
		return _index_map[v];
	}


protected:

	std::vector<V> _vertexs;

	std::vector< std::vector<W> > _martix;

	std::map<V, size_t> _index_map;
};

template <class W>
struct EdgeNode{
	W _w;
	EdgeNode<W> *_next;
	int _src;
	int _dst;

	EdgeNode(int src, int dst, const W& w)
		:_src(src), _dst(dst), _w(w), _next(NULL)
	{}
};

template <class V, class W, bool IsDirected = false>
class GraphLinkTable{
	typedef EdgeNode<W> Edge;
public:
	GraphLinkTable(){}
	GraphLinkTable(const V* vertexs, size_t size)
	{
		assert(vertexs);
		_vertexs.resize(size);
		_link_tables.resize(size, NULL);
		for (size_t i = 0; i < size; ++i) {
			_vertexs[i] = vertexs[i];
			_index_map[vertexs[i]] = i;
		}
	}

	void _AddEdge(size_t src_index, size_t dst_index, const W& w)
	{
		assert(src_index < _vertexs.size() && dst_index < _vertexs.size());
		Edge *edge = new Edge(src_index, dst_index, w);
		//头插
		edge->_next = _link_tables[src_index];
		_link_tables[src_index] = edge;
	}

	void AddEdge(const V& src, const V& dst, const W& w)
	{
		assert(_index_map.count(src));
		assert(_index_map.count(dst));

		size_t src_index = _index_map[src];
		size_t dst_index = _index_map[dst];

		_AddEdge(src_index, dst_index, w);
		if (!IsDirected) {
			_AddEdge(dst_index, src_index, w);
		}
	}

	void DFS(size_t head = 0)
	{
		std::vector<bool> visited(_vertexs.size(), false);
		_DFS(head, visited);
		std::cout << std::endl;
	}

	void BFS(size_t head = 0)
	{
		std::vector<bool> visited(_vertexs.size(), false);
		std::queue<int> q;
		q.push(head);
		while (!q.empty()) {
			int front = q.front();
			q.pop();
			if (!visited[front]) {
				std::cout << _vertexs[front] << "->";
				visited[front] = true;
				Edge *cur = _link_tables[front];
				while (cur) {
					if (!visited[cur->_dst]) {
						q.push(cur->_dst);
					}
					cur = cur->_next;
				}
			}
		}
		std::cout << std::endl;
	}

	bool Kruskal(GraphLinkTable<V, W>& min_tree)
	{
		assert(!IsDirected);
		min_tree._vertexs = _vertexs;
		min_tree._link_tables.resize(_vertexs.size(), NULL);
		min_tree._index_map = _index_map;

		struct EdgeComparaLess
		{
			bool operator()(const Edge* left, const Edge* right)
			{
				return left->_w > right->_w;
			}
		};

		std::priority_queue< Edge*, std::vector<Edge*>, EdgeComparaLess > pq; 
		for (size_t i = 0; i < _link_tables.size(); ++i) {
			Edge *edge = _link_tables[i];
			while (edge) {
				if (edge->_src < edge->_dst) {	//必须是 
				//if (edge->_src > edge->_dst) {
					pq.push(edge);
				}
				edge = edge->_next;
			}
		}
		UnionFindSet union_set(_vertexs.size());
		size_t N = _vertexs.size() - 1;
		for (;;) {
			Edge *edge = pq.top();
			pq.pop();

			if (union_set._FindParent(edge->_src) != union_set._FindParent(edge->_dst)) {
				union_set.SetUnionFriend(edge->_src, edge->_dst);
				
				min_tree._AddEdge(edge->_src, edge->_dst, edge->_w);
				if (!--N)
					return true;
			}
		}
		return false;
	}
private:
	void _DFS(size_t src, std::vector<bool>& visited)
	{
		std::cout << _vertexs[src] << "->";
		visited[src] = true;
		Edge *cur = _link_tables[src];
		while (cur) {
			if (!visited[cur->_dst]) {
				_DFS(cur->_dst, visited);
			}
			cur = cur->_next;
		}
	}
		
protected:
	std::vector<V> _vertexs;
	std::vector<Edge*> _link_tables;
	std::map<V, size_t> _index_map;
};

#endif // !__GRAPH_H__

