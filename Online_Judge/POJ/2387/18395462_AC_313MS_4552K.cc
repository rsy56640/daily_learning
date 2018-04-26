#include <stdio.h>  
#include <vector>   
#include <algorithm>
#include <queue>
using namespace std;

//POJ 2387 Til the Cows Come Home
const int MAX = 0x7fffffff;

typedef struct {
	const bool operator()(const pair<int, int>& lhs, const pair<int, int>& rhs)const
	{
		return lhs.first >= rhs.first;
	}
}Comp;

int main()
{
	int T, N;
	scanf("%d%d", &T, &N);
	int** path = new int*[N];
	int* distance = new int[N];
	distance[0] = 0;
	for (int i = 0; i < N; ++i)
	{
		path[i] = new int[N];
		distance[i] = MAX;
	}
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < N; ++j)
			path[i][j] = MAX;
	for (int i = 0; i < T; ++i)
	{
		int s, t, length;
		scanf("%d%d%d", &s, &t, &length);
		s--; t--;
		if (path[s][t] > length)
			path[s][t] = path[t][s] = length;
	}

	//Dijkstra
	bool* mark = new bool[N];
	for (int i = 0; i < N; ++i)
		mark[i] = false;
	typedef int length;
	typedef int num;
	std::priority_queue<std::pair<length, num>, std::vector<pair<length, num> >, Comp> pq;
	pq.push(std::make_pair(0, 0));
	while (!mark[N - 1])
	{
		const std::pair<length, num> intersection = pq.top(); pq.pop();
		const int length = intersection.first;
		const int num = intersection.second;
		if (mark[num])continue;	//to avoid computing the same point, the reason is as below.
		mark[num] = true;
		distance[num] = length;
		for (int i = 0; i < N; ++i)
		{
			if (mark[i] || path[num][i] == MAX)continue;
			const int dis = distance[num] + path[num][i];
			if (dis < distance[i])
			{
				distance[i] = dis;
				//so the same point "i" might be push more than once
				pq.push(std::make_pair(dis, i));
			}
		}
	}
	printf("%d\n", distance[N - 1]);
	for (int i = 0; i < N; ++i)
		delete[] path[i];
	delete[] path;
	delete[] distance;
	delete[] mark;
	//system("pause");
	return 0;
}
