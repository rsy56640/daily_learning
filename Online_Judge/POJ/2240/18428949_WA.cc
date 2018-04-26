#include <stdio.h>  
#include <vector>   
#include <algorithm>
#include <map>
#include <string>
using namespace std;

//POJ 2240 Arbitrage

typedef struct
{
	int src, dest;
	double rate;
}Edge;

int main()
{
	int n, _case = 0;
	while (~scanf("%d", &n))
	{
		if (n == 0)break;
		_case++;
		char buf[64];
		map<string, int> id;
		for (int i = 0; i < n; ++i)
		{
			scanf("%s", buf);
			id[buf] = i;
		}
		int m;
		scanf("%d", &m);
		vector<Edge> edges(m);
		for (int i = 0; i < m; ++i)
		{
			scanf("%s", buf);
			edges[i].src = id[buf];
			scanf("%lf", &(edges[i].rate));
			scanf("%s", buf);
			edges[i].dest = id[buf];
		}

		//bellman-ford
		bool flag = false;
		for (int k = 0; k < n; ++k)
		{
			//suppose k is source 
			vector<double> currency(n);
			currency[k] = 1.0;

			//for each edge in edges[m],
			//loose the src->dest
			for (int i = 0; i < m; ++i)
			{
				for (int j = 0; j < n; ++j)
				{
					int src = edges[i].src;
					int dest = edges[i].dest;
					double rate = edges[i].rate;
					//loose and update
					if (currency[dest] < currency[src] * rate)
						currency[dest] = currency[src] * rate;
				}
			}
			//judge whether there is a positive circle
			if (currency[k] > 1.0)
			{
				flag = true;
				break;
			}
		}
		if (flag)
			printf("Case %d: Yes\n", _case);
		else printf("Case %d: No\n", _case);
	}
	//system("pause");
	return 0;
}
