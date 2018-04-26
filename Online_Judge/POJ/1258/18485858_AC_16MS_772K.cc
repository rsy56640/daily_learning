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
		int dist;
		int min_edge = 999999, in, out;
		for (int i = 0; i < N; ++i)
			for (int j = 0; j < N; ++j)
			{
				scanf("%d", &dist);
				matrix[i][j] = dist;
				if (dist < min_edge && (i != j))
				{
					in = i;
					out = j;
					min_edge = dist;
				}
			}
		bool *mark = new bool[N];
		for (int i = 0; i < N; ++i)
			mark[i] = false;
		mark[in] = true; mark[out] = true;
		int total_length = min_edge;
		priority_queue<Edge_info, vector<Edge_info>, Comp> pq;
		for (int i = 0; i < N; ++i)
		{
			if (mark[i])continue;
			pq.push(Edge_info(matrix[in][i], in, i));
			pq.push(Edge_info(matrix[out][i], out, i));
		}
		//Prim
		for (int i = 0; i < N - 2; ++i)
		{
			bool isValid = false;
			while (!isValid)
			{
				Edge_info e = pq.top(); pq.pop();
				if (mark[e.out])continue;
				isValid = true;
				mark[e.out] = true;
				total_length += e.length;
				for (int j = 0; j < N; ++j)
				{
					if (mark[j])continue;
					pq.push(Edge_info(matrix[e.out][j], e.in, j));
				}
			}
		}
		printf("%d\n", total_length);
	}
	//system("pause");
	return 0;
}