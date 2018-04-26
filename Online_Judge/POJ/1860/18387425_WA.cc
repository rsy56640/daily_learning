#include <stdio.h>  
#include <vector>   
#include <algorithm>
using namespace std;

//POJ 1860 Currency Exchange

struct ExPoint
{
	double rate, commision;
	ExPoint() {}
	ExPoint(double r, double c) :rate(r), commision(c) {}
};

int main() {
	int N, M, S;
	double V;
	while (~scanf("%d%d%d%lf", &N, &M, &S, &V))
	{
		ExPoint** matrix = new ExPoint*[N];
		double* currency = new double[N];
		for (int i = 0; i < N; ++i)
		{
			matrix[i] = new ExPoint[N];
			currency[i] = 0;
		}
		S--;
		currency[S] = V;
		for (int i = 0; i < M; ++i)
		{
			int A, B;
			double R_AB, C_AB, R_BA, C_BA;
			scanf("%d%d%lf%lf%lf%lf", &A, &B, &R_AB, &C_AB, &R_BA, &C_BA);
			A--; B--;
			matrix[A][B] = ExPoint(R_AB, C_AB);
			matrix[B][A] = ExPoint(R_BA, C_BA);
		}
		for (int i = 0; i < N; ++i)
		{
			//loose j -> k
			for (int j = 0; j < N; ++j)
			{
				if (currency[j] < 1e-7)continue;
				for (int k = 0; k < N; ++k)
				{
					if (matrix[j][k].commision < 1e-7)continue;
					double exchange = (currency[j] - matrix[j][k].commision)*matrix[j][k].rate;
					if (exchange > currency[k])currency[k] = exchange;
				}
			}
		}
		if (currency[S] > V)
			printf("YES\n");
		else printf("NO\n");
		for (int i = 0; i < N; ++i)
			delete[] matrix[i];
		delete[] matrix;
		delete[] currency;
	}
	//system("pause");
	return 0;
}
