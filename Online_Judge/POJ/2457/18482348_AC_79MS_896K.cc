#include <stdio.h>  
#include <vector>   
#include <algorithm>
#include <queue>
using namespace std;

//POJ 2457 Part Acquisition

struct Edge
{
	int in, out;
	Edge() {}
	Edge(int _in, int _out) :
		in(_in), out(_out) {}
};

void print(int* precursor, int K)
{
	int *tmp = new int[K];
	int count = 0;
	int cur = K - 1;
	while (cur >= 0)
	{
		tmp[count] = cur;
		cur = precursor[cur];
		count++;
	}
	printf("%d\n", count);
	for (int i = count - 1; i >= 0; --i)
		printf("%d\n", tmp[i] + 1);
	delete[] tmp;
}


int main()
{
	int N, K;
	while (~scanf("%d%d", &N, &K))
	{

		vector<vector<int> > nodes(K);
		vector<Edge> edges(N);
		for (int i = 0; i < N; ++i)
		{
			int in, out;
			scanf("%d%d", &in, &out);
			in--; out--;
			edges[i] = Edge(in, out);
			nodes[in].push_back(i);
		}
		//BFS
		bool *mark = new bool[K];
		int *precursor = new int[K];
		for (int i = 0; i < K; ++i)
		{
			mark[i] = false;
			precursor[i] = -1;
		}
		mark[0] = true;

		queue<int> q;
		q.push(0);
		while (!q.empty())
		{
			const int node_id = q.front(); q.pop();
			const int size = nodes[node_id].size();
			for (int i = 0; i < size; ++i)
			{
				const int edge_num = nodes[node_id][i];
				const int out = edges[edge_num].out;
				if (mark[out])continue;
				q.push(out); mark[out] = true;
				precursor[out] = node_id;
			}
		}
		if (mark[K - 1])print(precursor, K);
		else printf("-1\n");
		delete[] mark;
	}

	//system("pause");
	return 0;
}