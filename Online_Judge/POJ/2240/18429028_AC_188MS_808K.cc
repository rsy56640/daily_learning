#include <stdio.h>  
#include <vector>   
#include <algorithm>
#include <map>
#include <string>
using namespace std;

//POJ 2240 Arbitrage

int main()
{
	int n, _case = 0;
	while (~scanf("%d", &n))
	{
		if (n == 0)break;
		_case++;
		char buf[64];
		double **rate = new double*[n];
		for (int i = 0; i < n; ++i)
			rate[i] = new double[n];
		for (int i = 0; i < n; ++i)
			for (int j = 0; j < n; ++j)
				rate[i][j] = (i == j) ? 1.0 : 0.0;
		map<string, int> id;
		for (int i = 0; i < n; ++i)
		{
			scanf("%s", buf);
			id[buf] = i;
		}
		int m;
		scanf("%d", &m);
		for (int i = 0; i < m; ++i)
		{
			scanf("%s", buf);
			int src = id[buf];
			double _rate;
			scanf("%lf", &_rate);
			scanf("%s", buf);
			int dest = id[buf];
			rate[src][dest] = _rate;
		}

		//floyd
		bool flag = false;
		for (int k = 0; k < n; ++k)
			for (int i = 0; i < n; ++i)
				for (int j = 0; j < n; ++j)
					rate[i][j] = max(rate[i][j], rate[i][k] * rate[k][j]);
		for (int i = 0; i < n; ++i)
			if (rate[i][i] > 1.0)
			{
				flag = true;
				break;
			}
		if (flag)
			printf("Case %d: Yes\n", _case);
		else printf("Case %d: No\n", _case);
	}
	//system("pause");
	return 0;
}
