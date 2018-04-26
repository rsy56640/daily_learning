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
	vector<int>decs(n);
	acs[0] = decs[0] = 1;
	int _max = 0;
	for (int i = 1; i < n; ++i)
	{
		int m1 = 0, m2 = 0;
		for (int j = 0; j < i; ++j)
		{
			if (height[i] > height[j])m1 = std::max(m1, acs[j]);
			if (height[i] < height[j])m2 = std::max(m2, decs[j]);
		}
		acs[i] = m1 + 1;
		decs[i] = m2 + 1;
		_max = std::max(_max, std::max(acs[i], decs[i]));
	}
	printf("%d", n - _max);

	//system("pause");
	return 0;
}
