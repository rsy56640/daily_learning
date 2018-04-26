#include <stdio.h>
using namespace std;
const int prime = 1000000007;
char m[42][42];
bool visit[42][42];
int path[42][42];
int ansl[20], ansr[20], ans[20];
char c;
//left up right down
int dir[4][2] = { { 0,-1 }, { -1, 0 }, { 0, 1 }, { 1, 0 } };
class queue {
public:
	queue() :first(0), last(0) {}
	void push(int x, int y) { q[last][0] = x; q[last][1] = y; last++; }
	void pop(int& x, int& y) { x = q[first][0]; y = q[first][1]; first++; }
	bool empty() { return (last - first + 100) % 100 == 0; }
private:
	int q[100][2];
	int first, last;
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
		int d;
		scanf("%d%d", &w, &h);
		scanf("%c", &c);
		for (int i = 1; i <= h; ++i)
			for (int j = 1; j <= w; ++j)
				visit[h][w] = false;
		for (int i = 1; i <= h + 1; ++i)
			m[i][w + 1] = '#';
		for (int j = 1; j <= w + 1; ++j)
			m[h + 1][j] = '#';
		for (int i = 1; i <= h; ++i) {
			for (int j = 1; j <= w; ++j) {
				scanf("%c", &m[i][j]);
				if (m[i][j] == 'S') {
					sx = i;
					sy = j;
					if (sx == h)d = 1;
					if (sx == 1)d = 3;
					if (sy == w)d = 0;
					if (sy == 1)d = 2;
				}
				if (m[i][j] == 'E') {
					ex = i;
					ey = j;
				}
			}
			scanf("%c", &c);
		}
		//ansl
		//d+3%4??????
		int _d = (d + 3) % 4;
		int _sx = sx, _sy = sy;
		while (m[_sx][_sy] != 'E') {
			int x = _sx + dir[_d][0];
			int y = _sy + dir[_d][1];
			if (m[x][y] != '#') {
				_sx = x;
				_sy = y;
				_d = (_d + 3) % 4;
				ansl[q]++;
				continue;
			}
			_d = (_d + 1) % 4;
		}
		//ansr
		//
		_d = (d + 1) % 4;
		_sx = sx, _sy = sy;
		while (m[_sx][_sy] != 'E') {
			int x = _sx + dir[_d][0];
			int y = _sy + dir[_d][1];
			if (m[x][y] != '#') {
				_sx = x;
				_sy = y;
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
		queue que;
		que.push(sx, sy);
		path[sx][sy] = 1;
		while (!que.empty()) {
			int x, y;
			que.pop(x, y);
			if (m[x][y] == 'E')break;
			for (int i = 0; i < 4; ++i) {
				if (m[x + dir[i][0]][y + dir[i][1]] != '#'
					&&path[x + dir[i][0]][y + dir[i][1]] == (1 << 30)) {
					que.push(x + dir[i][0], y + dir[i][1]);
					path[x + dir[i][0]][y + dir[i][1]] = path[x][y] + 1;
				}
			}
		}
		ans[q] = path[ex][ey];
	}
	for (int i = 0; i < n; ++i)
		printf("%d %d %d\n", ansl[i], ansr[i], ans[i]);
	return 0;
}