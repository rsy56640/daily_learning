#include <stdio.h>
#include <vector>
#include <queue>
using namespace std;

const int inf = 0x7fffffff;
char temp[20];		//buffer

struct Edge {
	int from, to, cap, flow;
	void set(int u, int v, int c, int f)
	{
		from = u;
		to = v;
		cap = c;
		flow = f;
	}
	Edge(int u, int v, int c, int f) :from(u), to(v), cap(c), flow(f) {}
};

int main() {
	int n;
	while (~scanf("%d", &n))
	{
		int  ps, c, l;
		scanf("%d %d %d", &ps, &c, &l);
		int S = n, T = n + 1;
		vector<vector<std::pair<int, bool> > >	//node[i]数组 int表示和i相连的边序号
			node(n + 2);						//bool表示 true 出边; false 入边

		vector<Edge> edges;
		int edge_num = 0;				//边序号
		//读边
		for (int i = 0; i < l; ++i)
		{
			int from, to, cap;
			scanf("%s", temp); sscanf(temp, "(%d,%d)%d", &from, &to, &cap);
			edges.push_back(Edge(from, to, cap, 0));
			node[from].push_back(make_pair(edge_num, 1));
			node[to].push_back(make_pair(edge_num, 0));
			edge_num++;
		}

		//读S相关的边
		for (int i = 0; i < ps; ++i)
		{
			int to, cap;
			scanf("%s", temp); sscanf(temp, "(%d)%d", &to, &cap);
			edges.push_back(Edge(S, to, cap, 0));
			node[S].push_back(make_pair(edge_num, 1));
			node[to].push_back(make_pair(edge_num, 0));
			edge_num++;
		}

		//读T相关的边
		for (int i = 0; i < c; ++i)
		{
			int from, cap;
			scanf("%s", temp); sscanf(temp, "(%d)%d", &from, &cap);
			edges.push_back(Edge(from, T, cap, 0));
			node[from].push_back(make_pair(edge_num, 1));
			node[T].push_back(make_pair(edge_num, 0));
			edge_num++;
		}

		int maxFlow = 0;
		vector<int> aug(n + 2);					//aug[i] 表示S到i的可改进量
		vector<pair<int, bool> > path(n + 2);	//p[i] 表示增广路径上的 i的进入边序号和方向
		//BFS查找增广路径
		while (true)
		{
			for (int i = 0; i < n + 2; ++i)aug[i] = 0;
			queue<int> q;
			q.push(S);
			aug[S] = inf;
			while (!q.empty())
			{
				int x = q.front();
				q.pop();
				int size = node[x].size();
				for (int i = 0; i < size; ++i)
				{
					//注：因为本题平行边的原因，
					//有可能会出现四次 x-t
					int num = node[x][i].first;
					bool out = node[x][i].second;
					Edge& e = edges[num];
					int t = out ? e.to : e.from;
					if (aug[t])continue;		//访问过的就跳过，当然也可以不跳。。。

					if (out&&e.cap > e.flow)					//x->t 还能进一些
					{
						path[t] = make_pair(num, 0);			//记录进入边序号和方向
						aug[t] = min(aug[x], e.cap - e.flow);	//更新残存容量
						q.push(t);
					}

					if (!out&&e.flow > 0)						//x<-t 退回去一些
					{
						path[t] = make_pair(num, 1);			//记录进入边序号和方向
						aug[t] = min(aug[x], e.flow);			//更新残存容量
						q.push(t);
					}
				}
				if (aug[T])break;			//已经找到一条增广路径
			}
			if (!aug[T])break;				//没有增广路径，结束，返回结果
			//有增广路径，从path[]倒着更新
			//这条路径上最小值就是aug[T]
			int min_aug = aug[T];
			for (int u = T; u != S;)
			{
				int num = path[u].first;
				bool out = path[u].second;
				if (!out)			//原来的流：p_u-->u
					edges[num].flow += min_aug;
				else				//原来的流：p_u<--u
					edges[num].flow -= min_aug;
				u = out ? edges[num].to : edges[num].from;
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
