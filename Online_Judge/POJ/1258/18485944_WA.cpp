#include <stdio.h>  
#include <vector>   
#include <algorithm>
#include <queue>
using namespace std;

//POJ 1258 Agri-Net

typedef struct Edge_info
{
	int length;
	int in, out;
	Edge_info() {}
	Edge_info(int l, int i, int o)
		:length(l), in(i), out(o) {}
};
typedef struct
{
	bool operator()(const Edge_info& lhs, const Edge_info& rhs)
	{
		return lhs.length > rhs.length;
	}
}Comp;

const int MAXN = 100 + 5;
int matrix[MAXN][MAXN];

int main()
{
	int N;
	while (~scanf("%d", &N))
	{
		bool *mark = new bool[N];
		for (int i = 0; i < N; ++i)
			mark[i] = false;
		int total_length = 0;
		vector<Edge_info> edge(N*(N - 1));
		int edge_count = 0;
		int dist;
		for (int i = 0; i < N; ++i)
			for (int j = 0; j < N; ++j)
			{
				scanf("%d", &dist);
				if (i == j)continue;
				edge[edge_count++] = Edge_info(dist, i, j);
			}
		priority_queue<Edge_info, vector<Edge_info>, Comp> pq(Comp(), edge);
		mark[pq.top().in] = true;
		//Kruskal
		for (int i = 0; i < N - 1; ++i)
		{
			bool isValid = false;
			while (!isValid)
			{
				Edge_info e = pq.top(); pq.pop();
				if (mark[e.out])continue;
				isValid = true;
				mark[e.out] = true;
				total_length += e.length;
			}
		}
		printf("%d\n", total_length);
	}
	//system("pause");
	return 0;
}