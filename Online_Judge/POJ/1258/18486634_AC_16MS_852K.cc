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

inline int getFather(int *p, int x)
{
	if (p[x] == x)return x;
	p[x] = getFather(p, p[x]);
	return p[x];
}

inline void un(int *p, int x, int y)
{
	int fx = getFather(p, x);
	int fy = getFather(p, y);
	p[fx] = fy;
}

inline bool judge(int *p, int x, int y)
{
	return getFather(p, x) == getFather(p, y);
}

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
		//Kruskal
		int *p = new int[N];
		for (int i = 0; i < N; ++i)
			p[i] = i;
		for (int i = 0; i < N - 1; ++i)
		{
			bool isValid = false;
			while (!isValid)
			{
				Edge_info e = pq.top(); pq.pop();
				if (judge(p, e.in, e.out))continue;
				isValid = true;
				un(p, e.in, e.out);
				total_length += e.length;
			}
		}
		printf("%d\n", total_length);
		delete[] p;
	}
	//system("pause");
	return 0;
}