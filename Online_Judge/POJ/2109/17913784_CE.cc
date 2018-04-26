#include <stdio.h>
#include <algorithm>
#include <cmath>
using namespace std;

int main()
{
	double n, m;
	while (scanf_s("%lf%lf", &n, &m)){
		n = (double)pow(m, 1 / n);
		printf("%lf", n);
	}
	return 0;
}