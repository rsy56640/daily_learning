#include <stdio.h>  
#include <vector>   
#include <algorithm>
#include <cmath>
#include <queue>
using namespace std;

//POJ 2560 Freckles

typedef struct Edge_info
{
	float length;
	int in, out;
	Edge_info() {}
	Edge_info(float l, int i, int o)
		:length(l), in(i), out(o) {}
};

typedef struct Node
{
	float x, y;
	Node() {}
	Node(int _x, int _y) :x(_x), y(_y) {}
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
	int n;
	while (~scanf("%d", &n))
	{
		if (n == 0)break;
		vector<Node> nodes(n);
		float x, y;
		for (int i = 0; i < n; ++i)
		{
			scanf("%f%f", &x, &y);
			nodes[i] = Node(x, y);
		}
		vector<Edge_info> edge(n*(n - 1));
		int edge_count = 0;
		float dist;
		for (int i = 0; i < n; ++i)
			for (int j = 0; j < n; ++j)
			{
				if (i == j)continue;
				x = nodes[i].x - nodes[j].x;
				y = nodes[i].y - nodes[j].y;
				dist = sqrt(x*x + y*y);
				edge[edge_count++] = Edge_info(dist, i, j);
			}
		priority_queue<Edge_info, vector<Edge_info>, Comp> pq(Comp(), edge);
		//Kruskal
		int *p = new int[n];
		for (int i = 0; i < n; ++i)
			p[i] = i;
		float total_length = 0.0f;
		for (int i = 0; i < n - 1; ++i)
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
		printf("%.2f\n", total_length);
		delete[] p;
	}
	//system("pause");
	return 0;
}