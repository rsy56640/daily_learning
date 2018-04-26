#include <stdio.h>  
#include <vector>   
#include <algorithm>
#include <map>
using namespace std;

//POJ 1611 The Suspects

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
	int n, m;
	while (~scanf("%d%d", &n, &m))
	{
		if (n == 0)break;
		int *p = new int[n];
		for (int i = 0; i < n; ++i)
			p[i] = i;
		for (int i = 0; i < m; ++i)
		{
			int k; scanf("%d", &k);
			int s; scanf("%d", &s);
			int a;
			for (int i = 1; i < k; ++i)
			{
				scanf("%d", &a);
				if (!judge(p, s, a))
					un(p, s, a);
			}
		}
		int count = 0;
		int tmp = getFather(p, 0);
		for (int i = 0; i < n; ++i)
		{
			if (getFather(p, i) == tmp)count++;
		}
		printf("%d\n", count);
		delete[] p;
	}

	//system("pause");
	return 0;
}