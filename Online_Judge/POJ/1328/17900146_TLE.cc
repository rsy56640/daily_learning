#include <stdio.h>
#include <cmath>
#include <algorithm>
using namespace std;

const int MAXN = 1000 + 5;
struct pos {
	double x;
	double y;
	pos():x(0),y(0){}
	bool operator()(const pos& a, const pos& b) {
		return a.y < b.y;
	}
};
bool cmp(const pos& a, const pos& b) {
	return a.y < b.y;
}

double quickSqrt(double number)
{
    long i;
    double x, y;
    double f = 1.5;
    x  = number * 0.5;
    y  = number;
    i  = * ( long * ) &y;
    i  = 0x5f375a86 - ( i >> 1 );  
    y  = * ( double * ) &i;
    y  = y * ( f - ( x * y * y ) );
    y  = y * ( f - ( x * y * y ) );
    return number * y;
}
int main() {
	pos p[MAXN];
	int counter[MAXN];
	for(int i=0;i<MAXN;++i)
		counter[i]=1;
	int _count = 0;
	int n;
	double d;
	while (true) {
		scanf("%d%d", &n, &d);
		if (n == 0&&d == 0)break;
		double x, y;
		for (int i = 0; i < n; ++i) {
			scanf("%lf%lf", &x, &y);
			double disSquare=d*d - y*y;
			if(disSquare<0){
				counter[_count]=-1;
				++_count;
				getchar();
				continue;
			}
			double delta = quickSqrt(disSquare);
			p[i].x = x - delta;
			p[i].y = x + delta;
		}
		std::sort(p, p + MAXN, cmp);
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
