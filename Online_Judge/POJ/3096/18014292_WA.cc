#include <stdio.h>
using namespace std;

//POJ 3096
const int MAXN = 80;
int main() {
	int s[MAXN][MAXN], length[MAXN], line = 0;
	bool stop = false;
	while (true) {
		char c;
		for (int i = 0; i < MAXN; ++i) {
			scanf("%c", &c);
			if (c != '\n'&&c != '*')
				s[line][i] = int(c - 'A');
			if (c == '\n') {
				length[line] = i;
				break;
			}
			if (c == '*') {
				stop = true;
				break;
			}
		}
		if (stop)break;
		line++;
	}
	bool *surprising = new bool[line];
	for (int j = 0; j < line; ++j)
		surprising[j] = true;
	for (int i = 0; i < line; ++i) {
		//???????????
		int len = length[i];
		int *word_pos[26];
		int word_len[26];
		bool *diff = new bool[len];
		for (int j = 0; j < 26; ++j) {
			word_pos[j] = new int[len];
			word_len[j] = 0;
		}
		for (int j = 0; j < len; ++j)
			diff[j] = false;
		//
		bool _continue = true;
		for (int j = 0; j < len; ++j) {
			int word = s[i][j];
			word_pos[word][word_len[word]] = j;
			word_len[word]++;
			if (word_len[word] == 1)continue;
			//???????????
			int k = word_len[word] - 1;
			for (int m = 0; m < k; ++m) {
				int d = word_pos[word][k] - word_pos[word][m];
				if (diff[d]) {
					surprising[i] = false;
					_continue = false;
					break;
				}
				if (!_continue)break;
				if (!diff[d])diff[d] = true;
			}
			if (!_continue)break;
		}
		delete[] diff;
		for (int j = 0; j < 26; ++j)
			delete[] word_pos[j];
	}
	//output
	for (int i = 0; i < line; ++i) {
		int len = length[i];
		for (int j = 0; j < len; ++j)
			printf("%c", char(s[i][j] + 65));
		printf(" is");
		if (surprising[i])printf(" surprsing.\n");
		else printf(" NOT surprising.\n");
	}
	return 0;
}