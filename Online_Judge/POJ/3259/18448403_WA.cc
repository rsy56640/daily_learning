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
	int F;
	scanf("%d", &F);
	for (int f = 0; f < F; ++f)
	{
		int N, M, W;
		scanf("%d%d%d", &N, &M, &W);
		const int edge_num = 2 * M + W;
		vector<Edge> edges(edge_num);
		int edge_count = 0;
		//path
		int S, E, T;
		for (int i = 0; i < M; ++i)
		{
			scanf("%d%d%d", &S, &E, &T);
			S--; E--;
			edges[edge_count++] = Edge(S, E, T);
			edges[edge_count++] = Edge(E, S, T);
		}
		//wormhole
		for (int i = 0; i < W; ++i)
		{
			scanf("%d%d%d", &S, &E, &T);
			S--; E--;
			edges[edge_count++] = Edge(S, E, -T);
		}
		//Bellan-Ford
		vector<int> distance(N);
		for (int i = 0; i < N; ++i)
			distance[i] = MAX;
		distance[0] = 0;
		for (int i = 1; i < N; ++i)
			for (int j = 0; j < edge_num; ++j)
			{
				const Edge& edge = edges[j];
				int in = edge.in, out = edge.out, val = edge.val;
				if (distance[in] + val < distance[out])
					distance[out] = distance[in] + val;
			}
		bool flag = false;
		for (int j = 0; j < edge_num; ++j)
		{
			const Edge& edge = edges[j];
			int in = edge.in, out = edge.out, val = edge.val;
			if (distance[in] + val < distance[out])
			{
				flag = true;
				break;
			}
		}
		if (!flag)printf("NO\n");
		else printf("YES\n");
	}
	//system("pause");
	return 0;
}
