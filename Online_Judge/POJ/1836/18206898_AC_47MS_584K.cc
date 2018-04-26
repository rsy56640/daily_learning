#include <stdio.h>  
#include <vector>   
#include <algorithm>
using namespace std;

//POJ 1836 Alignment

int main() {
	int n;
	scanf("%d", &n);
	vector<double> height(n);
	for (int i = 0; i < n; ++i)
		scanf("%lf", &height[i]);
	vector<int>acs(n);
	vector<int>decs(n);			//倒序递增
	acs[0] = decs[n - 1] = 1;
	for (int i = 1; i < n; ++i)
	{
		int m1 = 0, m2 = 0;
		double h1 = height[i], h2 = height[n - 1 - i];
		for (int j = 0; j < i; ++j)
		{
			if (h1 > height[j])m1 = std::max(m1, acs[j]);
			if (h2 > height[n - 1 - j])m2 = std::max(m2, decs[n - 1 - j]);
		}
		acs[i] = m1 + 1;
		decs[n - 1 - i] = m2 + 1;
	}
	int _max = std::max(acs[n - 1], decs[0]);
	for (int i = 0; i < n - 1; ++i)
		for (int j = i + 1; j < n; ++j)
			_max = std::max(_max, acs[i] + decs[j]);
	printf("%d", n - _max);

	//system("pause");
	return 0;
}
