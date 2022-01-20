/*
 * =====================================================================================
 *
 *       Filename:  build_zonelists.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2022/01/20 08 ±49∑÷59√Î
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), richard.weiyang@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMA_EIGHT

#ifdef NUMA_FOUR
#define INT_MAX		((int)(~0U >> 1))
#define NUM_NODES 4

int node_distance[NUM_NODES][NUM_NODES] = {
	{10, 12, 32, 32},
	{12, 10, 32, 32},
	{32, 32, 10, 12},
	{32, 32, 12, 10},
};
#endif

#ifdef NUMA_SIX
#define INT_MAX		((int)(~0U >> 1))
#define NUM_NODES 6

int node_distance[NUM_NODES][NUM_NODES] = {
	{10, 12, 12, 32, 32, 32},
	{12, 10, 12, 32, 32, 32},
	{12, 12, 10, 32, 32, 32},
	{32, 32, 32, 10, 12, 12},
	{32, 32, 32, 12, 10, 12},
	{32, 32, 32, 12, 12, 10},
};
#endif

#ifdef NUMA_EIGHT
#define INT_MAX		((int)(~0U >> 1))
#define NUM_NODES 8

int node_distance[NUM_NODES][NUM_NODES] = {
	{10, 12, 12, 12, 32, 32, 32, 32},
	{12, 10, 12, 12, 32, 32, 32, 32},
	{12, 12, 10, 12, 32, 32, 32, 32},
	{12, 12, 12, 10, 32, 32, 32, 32},
	{32, 32, 32, 32, 10, 12, 12, 12},
	{32, 32, 32, 32, 12, 10, 12, 12},
	{32, 32, 32, 32, 12, 12, 10, 12},
	{32, 32, 32, 32, 12, 12, 12, 10},
};
#endif

#ifdef UMA_FOUR
#define INT_MAX		((int)(~0U >> 1))
#define NUM_NODES 4

int node_distance[NUM_NODES][NUM_NODES] = {
	{10, 10, 10, 10},
	{10, 10, 10, 10},
	{10, 10, 10, 10},
	{10, 10, 10, 10},
};
#endif

#ifdef UMA_FOUR2
#define INT_MAX		((int)(~0U >> 1))
#define NUM_NODES 4

int node_distance[NUM_NODES][NUM_NODES] = {
	{10, 10, 20, 20},
	{10, 10, 20, 20},
	{20, 20, 10, 10},
	{20, 20, 10, 10},
};
#endif

#ifdef UMA_FIVE
#define INT_MAX		((int)(~0U >> 1))
#define NUM_NODES 5

int node_distance[NUM_NODES][NUM_NODES] = {
	{10, 10, 10, 20, 20},
	{10, 10, 10, 20, 20},
	{10, 10, 10, 20, 20},
	{20, 20, 20, 10, 10},
	{20, 20, 20, 10, 10},
};
#endif

#ifdef UMA_SIX
#define INT_MAX		((int)(~0U >> 1))
#define NUM_NODES 6

int node_distance[NUM_NODES][NUM_NODES] = {
	{10, 10, 10, 10, 20, 20},
	{10, 10, 10, 10, 20, 20},
	{10, 10, 10, 10, 20, 20},
	{10, 10, 10, 10, 20, 20},
	{20, 20, 20, 20, 10, 10},
	{20, 20, 20, 20, 10, 10},
};
#endif

int node_load[NUM_NODES];
int used_node[NUM_NODES];

int find_next_best_node(int node)
{
	int n, best_node = -1;
	int min_val = INT_MAX;

	if (!used_node[node]) {
		used_node[node] = 1;
		return node;
	}

	for (n = 0; n < NUM_NODES; n++) {
		int val;

		if (used_node[n])
			continue;

		val = node_distance[node][n];

		val *= 1000;
		val += node_load[n];

		if (val < min_val) {
			min_val = val;
			best_node = n;
		}
	}
	if (best_node >= 0)
		used_node[best_node] = 1;

	return best_node;
}

void build_zonelist(int local_node)
{
	int prev_node, node, nr_nodes = 0;
	int node_order[NUM_NODES];
	memset(used_node, 0, sizeof(used_node));
	int load = NUM_NODES;

	prev_node = local_node;

	while ((node = find_next_best_node(local_node)) >= 0) {

		if ((node_distance[local_node][node] != 
		     node_distance[local_node][prev_node]) ||
		     node == local_node)
			node_load[node] += load;

		node_order[nr_nodes++] = node;
		prev_node = node;
	}

	printf("Fallback order for Node %d: ", local_node);
	for (node = 0; node < nr_nodes; node++)
		printf("%3d", node_order[node]);
	printf("\n");
	printf("\t node_load : ");
	for (node = 0; node < NUM_NODES; node++)
		printf("%3d", node_load[node]);
	printf("\n");
}

int main()
{
	int node;
	memset(node_load, 0, sizeof(node_load));

	for (node = 0; node < NUM_NODES; node++)
		build_zonelist(node);
	return 0;
}
