#include <stdio.h>  
#include <vector>   
#include <algorithm>
#include <iostream>
using namespace std;

//POJ 1276 Cash Machine
const int _min = 0x80000000;

int main() {
	int cash, n;
	while (~scanf("%d %d", &cash, &n))
	{
		if (n == 0)
		{
			printf("%d\n", 0);
			continue;
		}
		vector<int> amount(n), denomination(n);
		for (int i = 0; i < n; ++i)
			scanf("%d %d", &amount[i], &denomination[i]);
		int **dp = new int*[n + 1];
		for (int i = 0; i <= n; ++i)
			dp[i] = new int[cash + 1];
		//dp[i][j]表示在只有前i个的情况下，容量最大为j的最大值
		for (int i = 0; i < n + 1; ++i)
			for (int j = 0; j < cash + 1; ++j)
				dp[i][j] = 0;
		for (int i = 0; i < n; ++i)
			for (int j = 0; j < cash + 1; ++j)
			{
				int _amount = amount[i];
				int _denomination = denomination[i];
				dp[i + 1][j] = _min;
				int temp = std::min(_amount, j / _denomination);
				for (int k = 0; k <= temp; ++k)
					//{
					dp[i + 1][j] = std::max(dp[i + 1][j],
						dp[i][j - k*_denomination] + k*_denomination);
				//cout << i + 1 << "\t" << j << "\t" << dp[i + 1][j] << endl;
			//}
			}
		printf("%d", dp[n][cash]);
		for (int i = 0; i <= n; ++i)
			delete[] dp[i];
		delete[] dp;
	}
	//system("pause");
	return 0;
}
