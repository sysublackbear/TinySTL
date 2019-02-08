#ifndef _UF_SET_H_
#define _UF_SET_H_

#include <cstring>

// 并查集uf_set
// 主要应用：求图的连通分量
namespace TinySTL{
	template <size_t N>
	class uf_set{
	public:
		uf_set();

		int Find(int index);
		void Union(int index1, int index2);

		void Clear();
	private:
		int parent[N];//parent[i] = -n 表示节点i是根节点且以i为根的树中共有n个节点
	};

	template<size_t N>
	uf_set<N>::uf_set(){
		Clear();
	}
	template<size_t N>
	int uf_set<N>::Find(int index){
		auto root = index;
		// 一直递归找出index节点的根节点
		// 记为root
		for (; parent[root] >= 0; root = parent[root]){}
		while (root != index){//路径压缩
			auto t = parent[index];  // 在find操作进行时，使find查找路径中的顶点(的父亲)都直接指向为树根（这很明显地改变了子树的高度）
			parent[index] = root;
			index = t;
		}
		return root;
	}
	// parent[index]代表元素index的父亲是parent[index]
	template<size_t N>
	void uf_set<N>::Union(int index1, int index2){
		auto root1 = Find(index1), root2 = Find(index2);
		auto total_nodes = parent[root1] + parent[root2];//total nodes
		// 判断root1和root2哪个树高。较高的子树去合并较低的子树
		if (parent[root1] > parent[root2]){//加权合并
			parent[root1] = root2;
			parent[root2] = total_nodes;
		}else{
			parent[root2] = root1;
			parent[root1] = total_nodes;
		}
	}

	// 初始化，将所有的元素都初始化为-1，表示彼此之间并不相交
	template<size_t N>
	void uf_set<N>::Clear(){
		memset(parent, -1, sizeof(int) * N);
	}
}

#endif
