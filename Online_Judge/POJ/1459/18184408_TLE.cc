#include <stdio.h>  
#include <vector>  
#include <queue>  
using namespace std;

//POJ 1459 Power Network  

const int MAXN = 100 + 2;
const int inf = 0x7fffffff;
char temp[20];      //buffer  
int main() {
	int n;
	while (~scanf("%d", &n))
	{
		int flow[MAXN][MAXN], cap[MAXN][MAXN];
		for (int i = 0; i < MAXN; ++i)
			for (int j = 0; j < MAXN; ++j)
			{
				flow[i][j] = 0;
				cap[i][j] = 0;
			}
		int  ps, c, l;
		scanf("%d %d %d", &ps, &c, &l);
		int S = n, T = n + 1;
		vector<vector<int> >node(n + 2);                    //node[i]数组 表示和i相连的点  

															//读边  
		for (int i = 0; i < l; ++i)
		{
			int from, to, ca;
			scanf("%s", temp); sscanf(temp, "(%d,%d)%d", &from, &to, &ca);
			cap[from][to] = ca;
			if (cap[to][from])continue;         //平行边重复记录  
			node[from].push_back(to);
			node[to].push_back(from);
		}

		//读S相关的边  
		for (int i = 0; i < ps; ++i)
		{
			int to, ca;
			scanf("%s", temp); sscanf(temp, "(%d)%d", &to, &ca);
			cap[S][to] = ca;
			if (cap[to][S])continue;            //平行边重复记录  
			node[S].push_back(to);
			node[to].push_back(S);
		}

		//读T相关的边  
		for (int i = 0; i < c; ++i)
		{
			int from, ca;
			scanf("%s", temp); sscanf(temp, "(%d)%d", &from, &ca);
			cap[from][T] = ca;
			if (cap[T][from])continue;          //平行边重复记录  
			node[from].push_back(T);
			node[T].push_back(from);
		}

		int maxFlow = 0;
		vector<int> aug(n + 2);                   //aug[i] 表示S到i的可改进量  
		vector<int> path(n + 2);              //p[i] 表示增广路径上的 i的进入点  
											  //BFS查找增广路径  
		while (true)
		{
			for (int i = 0; i < n + 2; ++i)aug[i] = 0;
			queue<int>q;
			q.push(S);
			aug[S] = inf;
			while (!q.empty())
			{
				int x = q.front();
				q.pop();
				int size = node[x].size();
				for (int i = 0; i < size; ++i)
				{
					int t = node[x][i];
					if (aug[t])continue;				//访问过的就跳过，当然也可以不跳。。。

														//x->t
					if (cap[x][t] > flow[x][t])
					{
						path[t] = x;				//记录进入点
						aug[t] = min(aug[x], cap[x][t] - flow[x][t]);	//更新残存容量
						q.push(t);
					}

					//t->x
					else
					{
						path[t] = x;				//记录进入点
						aug[t] = min(aug[x], flow[t][x]);				//更新残存容量
						q.push(t);
					}

				}
				if (aug[T])break;           //已经找到一条增广路径  
			}
			if (!aug[T])break;              //没有增广路径，结束，返回结果  
											//有增广路径，从path[]倒着更新  
											//这条路径上最小值就是aug[T]  
			int min_aug = aug[T];
			for (int u = T; u != S; u = path[u])
			{
				if (flow[path[u]][u] < cap[path[u]][u])
					flow[path[u]][u] += min_aug;
				else
					flow[u][path[u]] -= min_aug;
			}
			//流加上这个值  
			maxFlow += min_aug;
		}
		//没有增广路径，结束，返回结果  
		printf("%d\n", maxFlow);
	}
	//system("pause");
	return 0;
}
