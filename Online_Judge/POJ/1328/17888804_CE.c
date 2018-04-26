#include <stdio.h>
#include <cmath>
#include <algorithm>
using namespace std;

const int MAXN = 1000 + 5;
struct pos {
	double x = 0;
	double y = 0;
	bool operator()(const pos& a, const pos& b) {
		return a.y < b.y;
	}
};
bool cmp(const pos& a, const pos& b) {
	return a.y < b.y;
}


int main() {
	pos p[MAXN];
	int counter[MAXN];
	//for (int &i : counter)
	//	i = 1;
	for(int i=0;i<MAXN;++i)
		counter[i]=1;
	int _count = 0;
	int n, d;
	while (true) {
		scanf("%d%d", &n, &d);
		if (n == 0)break;
		double x, y;
		for (int i = 0; i < n; ++i) {
			scanf("%lf%lf", &x, &y);
			double delta = sqrt(d*d - y*y);
			p[i].x = x - delta;
			p[i].y = x + delta;
		}
		std::sort(p, p + MAXN, cmp);
		//sort(p, p + MAXN, [](pos a, pos b)->bool {return a.y < b.y; });
		//sort(p, p + MAXN, pos());
		int cur = 1;
		double last = p[0].y;
		while (cur < n) {
			if (p[cur].x > last) {
				++counter[_count];
				last = p[cur].y;
			}
			++cur;
		}
		++_count;
		getchar();
	}
	for (int i = 0; i < _count; ++i)
		printf("Case%d: %d\n", i + 1, counter[i]);
	return 0;
}
