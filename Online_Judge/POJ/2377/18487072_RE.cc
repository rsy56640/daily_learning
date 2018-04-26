#include <stdio.h>  
#include <vector>   
#include <algorithm>
#include <cmath>
#include <queue>
using namespace std;

//POJ 2560 Freckles

typedef struct Edge_info
{
	int length;
	int in, out;
	Edge_info() {}
	Edge_info(float l, int i, int o)
		:length(l), in(i), out(o) {}
};

typedef struct
{
	bool operator()(const Edge_info& lhs, const Edge_info& rhs)
	{
		return lhs.length < rhs.length;
	}
}Comp;

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
	int N, M;
	while (~scanf("%d%d", &N, &M))
	{
		vector<Edge_info> edge(M);
		int edge_count = 0;
		int A, B, C;
		for (int i = 0; i < M; ++i)
		{
			scanf("%d%d%d", &A, &B, &C);
			A--; B--;
			edge[i] = Edge_info(C, A, B);
		}
		priority_queue<Edge_info, vector<Edge_info>, Comp> pq(Comp(), edge);//maximum heap
		//Kruskal
		int *p = new int[N];
		for (int i = 0; i < N; ++i)
			p[i] = i;
		int total_length = 0;
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