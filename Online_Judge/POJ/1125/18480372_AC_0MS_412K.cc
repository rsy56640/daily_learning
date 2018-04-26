#include <stdio.h>
#include <algorithm>
using namespace std;
const int N = 105 + 3;
const int MAXN = 10000;
int matrix[N][N][N];

//POJ 1125 Stockbroker Grapevine

int main()
{
	int n;
	while (~scanf("%d", &n))
	{
		if (n == 0)break;
		for (int k = 0; k <= n; ++k)
			for (int i = 0; i < n; ++i)
				for (int j = 0; j < n; ++j)
					matrix[k][i][j] = MAXN;
		for (int i = 0; i < n; ++i)
		{
			int m;
			scanf("%d", &m);
			matrix[0][i][i] = 0;
			for (int j = 0; j < m; ++j)
			{
				int out, time;
				scanf("%d%d", &out, &time);
				matrix[0][i][out - 1] = time;
			}
		}
		//floyd
		for (int k = 1; k < n; ++k)
			for (int i = 0; i < n; ++i)
				for (int j = 0; j < n; ++j)
					matrix[k][i][j] = min(matrix[k - 1][i][j], matrix[k - 1][i][k] + matrix[k - 1][k][j]);
		int cur_min = MAXN;
		int cur_num = -1;
		for (int i = 0; i < n; ++i)
		{
			int temp_max = 0x80000000;
			for (int j = 0; j < n; ++j)
			{
				if (i == j)continue;
				matrix[n][i][j] = min(matrix[n - 1][i][j], matrix[n - 1][i][n - 1] + matrix[n - 1][n - 1][j]);
				if (matrix[n][i][j] > temp_max)
					temp_max = matrix[n][i][j];
			}
			if (temp_max < cur_min)
			{
				cur_min = temp_max;
				cur_num = i;
			}
		}
		printf("%d %d\n", cur_num + 1, cur_min);
	}
	//system("pause");
	return 0;
}
