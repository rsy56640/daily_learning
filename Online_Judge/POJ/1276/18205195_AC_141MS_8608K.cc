#include <stdio.h>  
#include <vector>   
#include <algorithm>
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
		vector<int>zero_one_bug;
		vector<int> amount(n), denomination(n);
		for (int i = 0; i < n; ++i)
		{
			int amount, denomination;
			scanf("%d %d", &amount, &denomination);
			int k = 1;
			while (amount > 0)
			{
				if (amount < k)
				{
					zero_one_bug.push_back(amount*denomination);
					break;
				}
				amount -= k;
				zero_one_bug.push_back(k*denomination);
				k <<= 1;
			}
		}
		int size = zero_one_bug.size();
		bool **dp = new bool*[size + 1];
		for (int i = 0; i <= size; ++i)
			dp[i] = new bool[cash + 1];
		//dp[i][j]表示在只有前i个的情况下，容量为j的包能否装满
		for (int i = 0; i < size + 1; ++i)
		{
			dp[i][0] = true;
			for (int j = 1; j < cash + 1; ++j)
				dp[i][j] = false;
		}
		for (int i = 0; i < size; ++i)
		{
			int temp = std::min(zero_one_bug[i], cash);
			for (int j = 0; j <= temp; ++j)
			{
				dp[i + 1][j] |= dp[i][j];
				if (dp[i + 1][j])continue;
			}
			temp = zero_one_bug[i];
			for (int j = temp; j <= cash; ++j)
			{
				dp[i + 1][j] = dp[i][j] | dp[i][j - temp];
				if (dp[i + 1][j])continue;
			}
		}
		int cur_cash = cash;
		for (;; cur_cash--)
			if (dp[size][cur_cash])break;
		printf("%d\n", cur_cash);
		for (int i = 0; i <= size; ++i)
			delete[] dp[i];
		delete[] dp;
	}
	//system("pause");
	return 0;
}
