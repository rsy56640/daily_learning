#include <stdio.h>
#include <algorithm>
using namespace std;

int main() {
	int n;
	scanf_s("%d", &n);
	int *length = new int[n];
	for (int i = 0; i < n; ++i)
		scanf("%d", &length[i]);
	sort(length, length + n);
	int cost = 0;
	for (int i = 0; i < n; ++i) {
		cost += length[i] * (n - i);
	}
	cost -= length[0];
	printf("%d", cost);
	return 0;
}