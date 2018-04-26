#include <stdio.h>
#include <algorithm>
using namespace std;

inline void flip(bool f[][6], int x, int y) {
	f[x][y - 1] ^= 1;
	f[x - 1][y] ^= 1;
	f[x][y] ^= 1;
	f[x + 1][y] ^= 1;
	f[x][y + 1] ^= 1;
}
inline void fresh(bool f[][6], bool g[][6]) {
	for (int i = 1; i < 5; ++i)
		for (int j = 0; j < 5; ++j)
			g[i][j] = f[i][j];
}

int main()
{
	int min_flip = 16;
	bool f[6][6], g[6][6];
	for (int i = 1; i < 5; ++i) {
		for (int j = 1; j < 5; ++j) {
			char c;
			scanf("%c", &c);
			if (c == 'b')f[i][j] = 1;
			else f[i][j] = 0;
		}
		getchar();
	}
	//f turn to 1
	for (int m = 0; m < 16; ++m) {
		fresh(f, g);
		//flip 1st line
		int counter = 0;
		for (int i = 0; i < 4; ++i)
			if (m&(1 << i)) {
				flip(g, 1, i + 1);
				++counter;
			}
		//flip other lines according to 1st line
		for (int i = 2; i < 5; ++i)
			//for any line, check 1-4 col
			for (int j = 1; j < 5; ++j)
				if (!g[i - 1][j]) {
					flip(g, i, j);
					++counter;
				}
		//check the last line
		bool success = true;
		for (int j = 1; j < 5; ++j)
			if (!g[4][j]) {
				success = false;
				break;
			}
		//upgrade min
		if (success)min_flip = std::min(min_flip, counter);
	}

	//g turn to 0
	for (int m = 0; m < 16; ++m) {
		fresh(f, g);
		//flip 1st line
		int counter = 0;
		for (int i = 0; i < 4; ++i)
			if (m&(1 << i)) {
				flip(g, 1, i + 1);
				++counter;
			}
		//flip other lines according to 1st line
		for (int i = 2; i < 5; ++i)
			//for any line, check 1-4 col
			for (int j = 1; j < 5; ++j)
				if (g[i - 1][j]) {
					flip(g, i, j);
					++counter;
				}
		//check the last line
		bool success = true;
		for (int j = 1; j < 5; ++j)
			if (g[4][j]) {
				success = false;
				break;
			}
		//upgrade min
		if (success)min_flip = std::min(min_flip, counter);
	}
	printf("%d", min_flip);
	return 0;
}