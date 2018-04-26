#include <stdio.h>  
#include <vector>   
#include <algorithm>
#include <queue>
using namespace std;

//POJ 1847 Tram
const int MAX = 0x7fffffff;

struct Edge
{
	int in, out, val;
	Edge() {}
	Edge(int _in, int _out, int _val) :
		in(_in), out(_out), val(_val) {}
};

typedef struct
{
	const bool operator()(pair<int, int>& lhs, pair<int, int>& rhs)
	{
		return lhs.first >= rhs.first;
	}
}Comp;

int main()
{
	int N, A, B;
	while (~scanf("%d%d%d", &N, &A, &B))
	{
		A--; B--;
		vector<vector<int> > nodes(N);	//node[i] represents the adjency edge_No of i
		vector<Edge> edges;
		int edge_count = 0;
		for (int i = 0; i < N; ++i)
		{
			int nums, intersection; scanf("%d", &nums);
			nodes[i].resize(nums);
			for (int j = 0; j < nums; ++j)
			{
				scanf("%d", &intersection); intersection--;
				if (j == 0)
					edges.push_back(Edge(i, intersection, 0));
				else
					edges.push_back(Edge(i, intersection, 1));
				nodes[i][j] = edge_count;
				edge_count++;
			}
		}
		//Dijkstra
		bool* mark = new bool[N];
		vector<int> distance(N);
		for (int i = 0; i < N; ++i)
		{
			distance[i] = MAX;
			mark[i] = false;
		}
		distance[A] = 0;
		typedef int dist;
		typedef int node_id;
		std::priority_queue<std::pair<dist, node_id>, vector<pair<dist, node_id> >, Comp> pq;
		pq.push(make_pair(0, A));
		while ((!mark[B]) && (!pq.empty()))
		{
			const std::pair<dist, node_id> node = pq.top(); pq.pop();
			const int dist = node.first;
			const int node_id = node.second;
			if (mark[node_id])continue;			//avoid the computing the same point
			mark[node_id] = true;
			for (vector<int>::iterator it = nodes[node_id].begin(); it != nodes[node_id].end(); ++it)
			{
				const Edge& edge = edges[*it];
				int out = edge.out;
				if (mark[out])continue;
				int val = edge.val;
				int new_dist = dist + val;
				if (new_dist < distance[out])
				{
					distance[out] = new_dist;
					pq.push(std::make_pair(new_dist, out));
				}
			}
		}
		if (mark[B])printf("%d\n", distance[B]);
		else printf("-1\n");
		delete[] mark;
	}
	//system("pause");
	return 0;
}
