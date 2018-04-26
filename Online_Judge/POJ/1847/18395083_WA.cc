#include <stdio.h>  
#include <vector>   
#include <algorithm>
using namespace std;

//POJ 1847 Tram
const int MAX = 0x7fffffff;

int main()
{
	int N, A, B;
	while (~scanf("%d%d%d", &N, &A, &B))
	{
		A--; B--;
		int** matrix = new int*[N];
		for (int i = 0; i < N; ++i)
			matrix[i] = new int[N];
		for (int i = 0; i < N; ++i)
			for (int j = 0; j < N; ++j)
				matrix[i][j] = (i == j) ? 0 : MAX;
		for (int i = 0; i < N; ++i)
		{
			int nums, intersection; scanf("%d", &nums);
			for (int j = 0; j < nums; ++j)
			{
				scanf("%d", &intersection);
				matrix[i][--intersection] = (j == 0) ? 0 : 1;
			}
		}

		//floyd
		for (int k = 0; k < N; ++k)
			for (int i = 0; i < N; ++i)
				for (int j = 0; j < N; ++j)
					matrix[i][j] = min(matrix[i][j], matrix[i][k] + matrix[k][j]);
		if (matrix[A][B] == MAX)printf("-1\n");
		else printf("%d", matrix[A][B]);
		for (int i = 0; i < N; ++i)
			delete[] matrix[i];
		delete[] matrix;
	}

	//system("pause");
	return 0;
}
