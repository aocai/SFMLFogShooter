#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include "PriorityQueue.h"

using namespace std;

struct AStarQNode
{
	int position; //y*128 + x
	double f, g, h;
};

//bool operator==(const AStarQNode &lhs, const AStarQNode &rhs)
//{
//	return (lhs.position == rhs.position);
//}
//
////pair hashing function from Boost library
//template <class T>
//inline void hash_combine(std::size_t & seed, const T & v)
//{
//	std::hash<T> hasher;
//	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
//}
//
//struct pair_hash
//{
//	template <class S, class T>
//	size_t operator() (const pair<S, T> &v) const
//	{
//		size_t seed = 0;
//		hash_combine(seed, v.first);
//		hash_combine(seed, v.second);
//		return seed;
//	}
//};

double heuristic(int start, int end)
{
	int x1, x2, y1, y2;
	y1 = start / 128;
	x1 = start % 128;
	y2 = end / 128;
	x2 = end % 128;
	return abs(x1 - x2) + abs(y1 - y2);
}

vector<int> getNeighbours(const vector<double> &maze, int position)
{
	vector<int> neighbours;
	int x, y;
	y = position / 128;
	x = position % 128;
	if (x > 0 && maze[position - 1] != -1)
		neighbours.push_back(position - 1);
	if (x < 127 && maze[position + 1] != -1)
		neighbours.push_back(position + 1);
	if (y > 0 && maze[position - 128] != -1)
		neighbours.push_back(position - 128);
	if (y < 71 && maze[position + 128] != -1)
		neighbours.push_back(position + 128);
	return neighbours;
}

bool AStar(const vector<double> &maze, int start, int goal, vector<int> &result)
{
	PriorityQueue<AStarQNode> openQueue;
	vector<int> inList(maze.size(), INT_MAX); //store priority so far
	AStarQNode startNode{ start, 0, 0, 0 };
	openQueue.insert(startNode, 0);

	while (!openQueue.empty())
	{
		AStarQNode qNode = openQueue.getHighestPriorityNode();
		if (qNode.position == goal)
		{
			return 1;
		}
		openQueue.extract();

		vector<int> neighbours = getNeighbours(maze, qNode.position);
		for (int i = 0; i < neighbours.size(); ++i)
		{
			AStarQNode node;
			node.position = neighbours[i];
			double cost = qNode.g + maze[node.position];
			node.g = cost;
			node.h = heuristic(node.position, goal);
			node.f = node.g + node.h;

			if (node.f >= inList[node.position])
			{
				continue;
			}
			openQueue.insert(node, node.f);
			inList[node.position] = node.f;
			result[node.position] = qNode.position;
		}
		inList[qNode.position] = qNode.f;
	}
	return 0;
}
