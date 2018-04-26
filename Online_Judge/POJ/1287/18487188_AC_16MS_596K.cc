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
	Edge_info(int i, int o, int l)
		: in(i), out(o), length(l) {}
};

typedef struct
{
	bool operator()(const Edge_info& lhs, const Edge_info& rhs)
	{
		return lhs.length > rhs.length;
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
	int P, R;
	while (~scanf("%d%d", &P, &R))
	{
		if (P == 0)break;
		if (P == 1)
		{
			printf("0\n");
			getchar();
			continue;
		}
		vector<Edge_info> edge(R);
		int edge_count = 0;
		int A, B, C;
		for (int i = 0; i < R; ++i)
		{
			scanf("%d%d%d", &A, &B, &C);
			A--; B--;
			edge[i] = Edge_info(A, B, C);
		}
		priority_queue<Edge_info, vector<Edge_info>, Comp> pq(Comp(), edge);//maximum heap
		//Kruskal
		int *p = new int[P];
		for (int i = 0; i < P; ++i)
			p[i] = i;
		int total_length = 0;
		for (int i = 0; i < P - 1; ++i)
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
		getchar();
	}
	//system("pause");
	return 0;
}