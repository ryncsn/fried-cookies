/*
 * Author: Kairui Song
 * Date: 2018-02-20
 */
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstring>
#include <limits>
#include <map>
#include <unordered_map>
#include <bitset>

using namespace::std;

#define N_MAX 100
#define INF numeric_limits<int>::max()


vector<int> to[N_MAX];
int match[N_MAX] = {-1};
bool visited[N_MAX] = {false};
bool skip[N_MAX] = {0};


bool bi_match_dfs(int num) {
	/* Do DFS bi match in place */
	for (int next : to[num]) {
		if (visited[next] == false) {
			visited[next] = true;
			if (match[next] == -1 || bi_match_dfs(match[next])) {
				match[num] = next;
				match[next] = num;
				return true;
			}
		}
	}
	return false;
}

void hacks() {
	cout.sync_with_stdio(false);
}

void clean() {
	cin >> ws;
}

void link(int start, int end) {
	to[start - 1].push_back(end - 1);
	to[end - 1].push_back(start - 1);
}

int main(int argc, char *argv[]) {
	hacks();
	int n = 8;
	link(1, 7);
	link(1, 5);
	link(5, 2);
	link(5, 3);
	link(3, 6);
	link(7, 4);
	link(4, 8);

	for (int i = 0; i < n; ++i) {
		match[i] = -1;
	}

	for (int i = 0; i < n; ++i) {
		for (int i = 0; i < n; i++)
			visited[i] = 0;
		if (match[i] == -1) {
			bi_match_dfs(i);
		}
	}

	for (int i = 0; i < n; ++i) {
		cout << "Match: " << i << " - " << match[i] << endl;
	}

	return 0;
}
