#include <stdio.h>  
#include <vector>   
#include <algorithm>
using namespace std;

//POJ 2524 Ubiquitous Religions

typedef struct Edge_info
{
	int length;
	int in, out;
	Edge_info() {}
	Edge_info(int i, int o, int l)
		: in(i), out(o), length(l) {}
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
	int count = 0;
	int n, m;
	while (~scanf("%d%d", &n, &m))
	{
		count++;
		if (n == 0)break;
		int *p = new int[n];
		for (int i = 0; i < n; ++i)
			p[i] = i;
		int A, B;
		for (int i = 0; i < m; ++i)
		{
			scanf("%d%d", &A, &B);
			A--; B--;
			un(p, A, B);
		}
		vector<int> diff_religion;
		for (int i = 0; i < n; ++i)
			diff_religion.push_back(getFather(p, i));
		std::sort(diff_religion.begin(), diff_religion.end());
		vector<int>::iterator it = std::unique(diff_religion.begin(), diff_religion.end());
		printf("Case %d: %d\n", count, it - diff_religion.begin());
	}
	//system("pause");
	return 0;
}