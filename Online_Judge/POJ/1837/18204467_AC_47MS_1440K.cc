#include <stdio.h>  
#include <vector>   
using namespace std;

//POJ 1837 Balance
const int MAX = 15001;
int dp[21][MAX];		//dp[i][j]表示放前i个砝码，力矩为j的总方法数
//这里j加了7500
int main() {
	dp[0][7500] = 1;
	int n, m;	//n, m 表示钩子和砝码数
	scanf("%d %d", &n, &m);
	vector<int> hook(n), weight(m);
	for (int i = 0; i < n; ++i)
		scanf("%d", &hook[i]);
	for (int i = 0; i < m; ++i)
		scanf("%d", &weight[i]);
	for (int i = 0; i < m; ++i)
		for (int j = 0; j < MAX; ++j)
			for (int k = 0; k < n; ++k)
				dp[i + 1][j] += dp[i][j - weight[i] * hook[k]];
	printf("%d", dp[m][7500]);
	//system("pause");
	return 0;
}
