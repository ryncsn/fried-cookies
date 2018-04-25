/*
 * Author: Kairui Song
 * Date: 2018-02-28
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

#define INF numeric_limits<int>::max()

void hacks() {
	cout.sync_with_stdio(false);
}

void clean() {
	cin >> ws;
}

int main(int argc, char *argv[]) {
	hacks();

	int T,count=0;

	for(cin >> T; count < T; count++){
		cout << "Case #" << count + 1 << "" << ": " << '\n';
	}

	return 0;
}
