#include <stdio.h>  
#include <vector>   
#include <algorithm>
using namespace std;

//POJ 1860 Currency Exchange

struct Edge
{
	int in, out;
	double rate, commision;
	Edge() {}
	Edge(double r, double c) :rate(r), commision(c) {}
};

int main()
{
	int N, M, S;
	double V;
	while (~scanf("%d%d%d%lf", &N, &M, &S, &V))
	{
		vector<Edge> edges(2 * M);
		vector<double> currency(N);
		S--;
		currency[S] = V;
		for (int i = 0; i < 2 * M;)
		{
			int A, B;
			double R_AB, C_AB, R_BA, C_BA;
			scanf("%d%d%lf%lf%lf%lf", &A, &B, &R_AB, &C_AB, &R_BA, &C_BA);
			A--; B--;
			edges[i].in = A;
			edges[i].out = B;
			edges[i].rate = R_AB;
			edges[i].commision = C_AB;
			i++;
			edges[i].in = B;
			edges[i].out = A;
			edges[i].rate = R_BA;
			edges[i].commision = C_BA;
			i++;
		}
		bool flag = false;
		for (int k = 1; k < N; ++k)
			for (int i = 0; i < 2 * M; ++i)
			{
				const Edge& edge = edges[i];
				int in = edge.in, out = edge.out;
				double rate = edge.rate, commision = edge.commision;
				double exchange = (currency[in] - commision)*rate;
				if (exchange > currency[out])
					currency[out] = exchange;
			}
		for (int i = 0; i < 2 * M; ++i)
		{
			const Edge& edge = edges[i];
			int in = edge.in, out = edge.out;
			double rate = edge.rate, commision = edge.commision;
			double exchange = (currency[in] - commision)*rate;
			if (exchange > currency[out])
			{
				flag = true;
				break;
			}
		}
		if (flag)
			printf("YES\n");
		else printf("NO\n");

	}
	//system("pause");
	return 0;
}
