#include <stdio.h>  
#include <vector>   
#include <algorithm>
//#include <iostream>
using namespace std;

//POJ 1276 Cash Machine

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
		bool **dp = new bool*[n + 1];
		for (int i = 0; i <= n; ++i)
			dp[i] = new bool[cash + 1];
		//dp[i][j]表示在只有前i个的情况下，容量为j的包能否装满
		for (int i = 0; i < n + 1; ++i)
		{
			dp[i][0] = true;
			for (int j = 1; j < cash + 1; ++j)
				dp[i][j] = false;
		}
		for (int i = 0; i < n; ++i)
			for (int j = 0; j < cash + 1; ++j)
			{
				int _amount = amount[i];
				int _denomination = denomination[i];
				int temp = std::min(_amount, j / _denomination);
				for (int k = 0; k <= temp; ++k)
				{
					dp[i + 1][j] = dp[i + 1][j] || dp[i][j - k*_denomination];
					if (dp[i + 1][j])break;
					//cout << i + 1 << "\t" << j << "\t" << dp[i + 1][j] << endl;
				}
			}
		int cur_cash = cash;
		for (;; cur_cash--)
			if (dp[n][cur_cash])break;
		printf("%d", cur_cash);
		for (int i = 0; i <= n; ++i)
			delete[] dp[i];
		delete[] dp;
	}
	//system("pause");
	return 0;
}
