/*
 * Author: Kairui Song
 * Date: 2018-03-08
 */
#include <iostream>
#include <algorithm>
#include <array>
#include <bitset>
#include <cstring>
#include <forward_list>
#include <limits>
#include <list>
#include <map>
#include <queue>
#include <unordered_map>
#include <vector>

using namespace::std;

#define INF numeric_limits<int>::max()
#define len(x) x.size()
#define N_MAX 100000


vector<int> to[N_MAX];
int match[N_MAX] = {-1};
bool visited[N_MAX] = {false};
bool skip[N_MAX] = {0};

bool bi_match_dfs(int num) {
	/* Do DFS bi match in place */
	if (skip[num]) {
		return false;
	}
	for (int next : to[num]) {
		if (visited[next] == false && !skip[next]) {
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


// SIZE, STEP: unit: Byte
template <const size_t SIZE = 32768 * 1024, const size_t STEP = 1024 * 1024>
struct StaticMemoryPool {
	int8_t mem[SIZE];
	int8_t *current;

	StaticMemoryPool(): mem(), current(mem) {}

	void* alloc(const size_t size) {
		if (current == mem + size) {
			return malloc(size);
		}
		return (current += size);
	}
};

StaticMemoryPool<> pool;
