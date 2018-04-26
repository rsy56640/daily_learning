#include <stdio.h>
#include <algorithm>
using namespace std;
char m[42][42];
bool visit[42][42];
int path[42][42];
int ansl[20], ansr[20], ans[20];
//left up right down
int dir[4][2] = { { 0,-1 }, { -1, 0 }, { 0, 1 }, { 1, 0 } };
class stack {
public:
	stack(){ last = 0; }
	void push(int x, int y) { stk[last][0] = x; stk[last][1] = y; last++; }
	void pop(int& x, int& y) { last--; x = stk[last][0]; y = stk[last][1]; }
	bool empty() { return last == 0; }
private:
	int stk[100][2];
	int last;
};
int main() {
	for (int i = 0; i < 42; ++i)
		for (int j = 0; j < 42; ++j)
			m[i][j] = '#';
	int n;
	scanf("%d", &n);
	for (int q = 0; q < n; ++q) {
		int w, h;
		int sx, sy, ex, ey;
		ansl[q] = 1, ansr[q] = 1, ans[q] = 0;
		int d;//?????direction
		scanf("%d%d", &w, &h);
		for (int i = 1; i <= h; ++i)
			for (int j = 1; j <= w; ++j)
				visit[h][w] = false;
		for (int i = 1; i <= h + 1; ++i)
			m[i][w + 1] = '#';
		for (int j = 1; j <= w + 1; ++j)
			m[h + 1][j] = '#';
		for (int i = 1; i <= h; ++i)
			for (int j = 1; j <= w; ++j) {
				scanf("%c", &m[h][w]);
				if (m[h][w] == 'S') {
					sx = h;
					sy = w;
					if (sy == h)d = 1;
					if (sy == 1)d = 3;
					if (sx == w)d = 0;
					if (sx == 1)d = 2;
				}
				if (m[h][w] == 'E') {
					ex = h;
					ey = w;
				}
			}
		//ansl
		//d+3%4??????
		int _d = (d + 3) % 4;
		int _sx = sx, _sy = sy;
		while (m[_sx][_sx] != 'E') {
			int x = _sx + dir[_d][0];
			int y = _sx + dir[_d][1];
			if (m[x][y] != '#') {
				_sx = _sx + dir[_d][0];
				_sx = _sx + dir[_d][1];
				_d = (_d + 3) % 4;
				ansl[q]++;
				continue;
			}
			_d = (_d + 1) % 4;
		}
		//ansr
		//
		_d = (d + 1) % 4;
		_sx = sx, _sx = sy;
		while (m[_sx][_sx] != 'E') {
			int x = _sx + dir[_d][0];
			int y = _sx + dir[_d][1];
			if (m[x][y] != '#') {
				_sx = _sx + dir[_d][0];
				_sx = _sx + dir[_d][1];
				_d = (_d + 1) % 4;
				ansr[q]++;
				continue;
			}
			_d = (_d + 3) % 4;
		}
		//ans
		for (int i = 0; i < 42; ++i)
			for (int j = 0; j < 42; ++j)
				path[i][j] = 1 << 30;
		stack stk;
		stk.push(sx, sy);
		path[sx][sy] = 1;
		while (!stk.empty()) {
			int x, y;
			stk.pop(x, y);
			if (m[x][y] == 'E')break;
			for (int i = 0; i < 4; ++i) {
				if (m[x + dir[i][0]][y + dir[i][1]] != '#') {
					stk.push(x + dir[i][0], y + dir[i][1]);
					path[x + dir[i][0]][y + dir[i][1]] =
						std::min(path[x + dir[i][0]][y + dir[i][1]], path[x][y] + 1);
				}
			}
			ans[q] = path[ex][ey];
		}
	}
	for (int i = 0; i < n; ++i)
		printf("%d %d %d\n", ansl[i], ansr[i], ans[i]);
	return 0;
}