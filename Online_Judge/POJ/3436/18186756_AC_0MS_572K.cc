#include <stdio.h>  
#include <vector>  
#include <queue>  
#include <stack>
using namespace std;

//POJ 3436 ACM Computer Factory
const int inf = 0x7fffffff;

struct Edge {
	int from, to, flow;
	void set(int u, int v, int f)
	{
		from = u;
		to = v;
		flow = f;
	}
	Edge(int u, int v, int f) :from(u), to(v), flow(f) {}
};

struct Info
{
	vector<int> v;
	Info() {}
	Info(vector<int>& other) :v(other) {}
	bool isNext(const Info& other)					//*this points to other
	{
		const vector<int>& ov = other.v;
		int size = ov.size();
		for (int i = 0; i < size; ++i)
			if ((ov[i] != 2) && (ov[i] != v[i]))
				return false;
		return true;
	}
};

bool next_to_S(vector<int> &v)
{
	int size = v.size();
	for (int i = 0; i < size; ++i)
		if (v[i] == 1)
			return false;
	return true;
}

bool next_to_T(vector<int> &v)
{
	int size = v.size();
	for (int i = 0; i < size; ++i)
		if (v[i] != 1)
			return false;
	return true;
}

struct Factory {
	int cap, flow;
	Info in, out;
};

int main() {
	int n, p;						//点数，部件数 
	while (~scanf("%d %d", &p, &n))
	{
		int S = 0, T = n + 1;			//Source, Terminal  
		vector<vector<std::pair<int, bool> > >		//node[i]数组 int表示和i相连的边序号  
			node(n + 2);							//bool表示 true 出边; false 入边  

		vector<Factory> node_info(n + 2);			//存储点的信息
		node_info[S].cap = inf;
		node_info[T].cap = inf;

		vector<Edge> edges;
		int edge_num = 0;				//边序号

		//读点，并生成边
		for (int i = 1; i <= n; ++i)
		{
			int cap;
			vector<int> in(p), out(p);
			scanf("%d", &cap);
			for (int i = 0; i < p; ++i)scanf("%d", &in[i]);
			for (int i = 0; i < p; ++i)scanf("%d", &out[i]);
			node_info[i].cap = cap;
			node_info[i].flow = 0;
			node_info[i].in = Info(in);
			node_info[i].out = Info(out);
			//S相关点
			if (next_to_S(node_info[i].in.v))
			{
				node[0].push_back(make_pair(edge_num, 1));
				node[i].push_back(make_pair(edge_num, 0));
				edges.push_back(Edge(S, i, 0));
				edge_num++;
			}
			//T相关点
			if (next_to_T(node_info[i].out.v))
			{
				node[T].push_back(make_pair(edge_num, 0));
				node[i].push_back(make_pair(edge_num, 1));
				edges.push_back(Edge(i, T, 0));
				edge_num++;
			}
			//检查是否有普通边
			for (int j = 1; j < i; ++j)
			{
				//j->i
				if (node_info[j].out.isNext(node_info[i].in))
				{
					node[j].push_back(make_pair(edge_num, 1));
					node[i].push_back(make_pair(edge_num, 0));
					edges.push_back(Edge(j, i, 0));
					edge_num++;
				}
				//j<-i
				if (node_info[i].out.isNext(node_info[j].in))
				{
					node[j].push_back(make_pair(edge_num, 0));
					node[i].push_back(make_pair(edge_num, 1));
					edges.push_back(Edge(i, j, 0));
					edge_num++;
				}
			}
		}

		int maxFlow = 0;
		vector<int> aug(n + 2);					//aug[i] 表示S到i的可改进量  
		vector<pair<int, bool> > path(n + 2);	//p[i] 表示增广路径上的 i的进入边序号和方向  

		//BFS查找增广路径  
		while (true)
		{
			for (int i = 0; i < n + 2; ++i)aug[i] = 0;
			stack<int> q;
			q.push(S);
			aug[S] = inf;
			while (!q.empty())
			{
				int x = q.top();
				q.pop();
				int size = node[x].size();
				for (int i = 0; i < size; ++i)
				{
					int num = node[x][i].first;
					bool out = node[x][i].second;
					Edge& e = edges[num];
					int t = out ? e.to : e.from;
					if (aug[t])continue;			//访问过的就跳过，当然也可以不跳。。。  

					if (out&&node_info[t].flow < node_info[t].cap)		//x->t 还能进一些  
					{
						path[t] = make_pair(num, 0);			//记录进入边序号和方向  
						aug[t] = min(aug[x],
							min(node_info[t].cap - node_info[t].flow
								, node_info[x].cap - node_info[x].flow));	//更新残存容量  
						q.push(t);
					}

					else if (!out&&e.flow > 0)				//x<-t 退回去一些  
					{
						path[t] = make_pair(num, 1);            //记录进入边序号和方向  
						aug[t] = min(aug[x], e.flow);           //更新残存容量  
						q.push(t);
					}
				}
				if (aug[T])break;           //已经找到一条增广路径  
			}
			if (!aug[T])break;              //没有增广路径，结束，返回结果  

			//有增广路径，从path[]倒着更新  
			//这条路径上最小值就是aug[T]  
			int min_aug = aug[T];
			for (int u = T; u != S;)
			{
				int num = path[u].first;
				bool out = path[u].second;
				if (!out)           //原来的流：p_u-->u  
				{
					edges[num].flow += min_aug;
					node_info[u].flow += min_aug;
				}
				else                //原来的流：p_u<--u  
				{
					edges[num].flow -= min_aug;
					node_info[u].flow -= min_aug;
				}
				u = out ? edges[num].to : edges[num].from;
			}
			//流加上这个值
			node_info[S].flow += min_aug;
			maxFlow += min_aug;
		}
		//没有增广路径，结束，返回结果
		printf("%d", maxFlow);
		queue<int> q;
		int size = edges.size();
		for (int i = 0; i < size; ++i)
			if (edges[i].flow > 0)
			{
				if (edges[i].from == S || edges[i].to == T)continue;
				q.push(i);
			}
		printf(" %d\n", q.size());
		while (!q.empty())
		{
			int m = q.front(); q.pop();
			printf("%d %d %d\n", edges[m].from, edges[m].to, edges[m].flow);
		}
	}
	//system("pause");
	return 0;
}
