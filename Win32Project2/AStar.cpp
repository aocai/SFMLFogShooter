//#include <iostream>
//#include <vector>
//#include <algorithm>
//#include <unordered_set>
//#include <unordered_map>
//#include <utility>
//#include "PriorityQueue.h"
//
//using namespace std;
//
//struct AStarQNode
//{
//	int position; //y*256 + x
//	double f, g, h;
//};
//
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
//
//double heuristic(int start, int end)
//{
//	int x1, x2, y1, y2;
//	y1 = start / 256;
//	x1 = start % 256;
//	y2 = end / 256;
//	x1 = start % 256;
//	return abs(x1 - x2) + abs(y1 - y2);
//}
//
//vector<int> getNeighbours(vector<double> &maze, int position)
//{
//	vector<int> neighbours;
//	int x, y;
//	y = position / 256;
//	x = position % 256;
//	if (x > 0 && maze[(x - 1)*y] != -1)
//		neighbours.push_back((x - 1)*y);
//	if (x < 256 && maze[(x + 1)*y] != -1)
//		neighbours.push_back((x + 1)*y);
//	if (y > 0 && maze[x*(y - 1)] != -1)
//		neighbours.push_back(x*(y - 1));
//	if (y < 144 && maze[x*(y + 1)] != -1)
//		neighbours.push_back(x*(y + 1));
//	return neighbours;
//}
//
//void AStar(vector<double> &maze, int start, int goal, vector<int> &result)
//{
//	PriorityQueue<AStarQNode> openQueue;
//	unordered_map<int, double, pair_hash> closedSet;
//	vector<int> inList(maze.size(), INT_MAX); //store priority so far
//	AStarQNode startNode{ start, 0, 0, 0 };
//	openQueue.insert(startNode, 0);
//
//	while (!openQueue.empty())
//	{
//		AStarQNode qNode = openQueue.getHighestPriorityNode();
//		if (qNode.position == goal)
//		{
//			cout << "goal found" << endl;
//			return;
//		}
//		openQueue.extract();
//
//		vector<int> neighbours = getNeighbours(maze, qNode.position);
//		for (int i = 0; i < neighbours.size(); ++i)
//		{
//			int p = neighbours[i];
//			AStarQNode node;
//
//			double cost = qNode.g + maze[p];
//			node.position = p;
//			node.g = cost;
//			node.h = heuristic(p, goal);
//			node.f = node.g + node.h;
//
//			if (node.f >= inList[p])
//				continue;
//
//			openQueue.insert(node, node.f);
//			inList[p] = node.f;
//			result[p] = qNode.position;
//		}
//		closedSet.insert(make_pair(qNode.position, qNode.f));
//		inList[qNode.position] = qNode.f;
//	}
//	cout << "no paths found" << endl;
//}
//
//int main()
//{
//	vector<vector<double>> maze = { 
//		{ 1, 1, 1, 1, 1,-1, 1, 1, 1, 1 },
//		{ 1, 1, 1, 1, 1,-1, 1, 1,-1, 1 },
//		{ 1, 1, 1, 1, 1,-1, 1,-1, 1, 1 },
//		{-1,-1,-1,-1, 1,-1, 1,-1, 1, 1 },
//		{ 1, 1, 1, 1, 1,-1, 1,-1, 1, 1 },
//		{ 1,-1,-1,-1,-1,-1, 1,-1, 1, 1 },
//		{ 1, 1, 1, 1, 1, 1, 1,-1, 1, 1 },
//		{ 1, 1, 1,-1, 1, 1, 1,-1, 1, 1 },
//		{ 1, 1, 1,-1, 1, 1, 1,-1, 1, 1 },
//		{ 1, 1, 1,-1, 1, 1, 1,-1, 1, 1 }
//	};
//
//	pair<int, int> start = make_pair(0, 0); //pair(y,x)
//	pair<int, int> goal = make_pair(9, 9);
//
//	vector<vector<pair<int, int>>> result(10,vector<pair<int,int>>(10, make_pair(-1,-1)));
//	AStar(maze, start, goal, result);
//
//	while (start != goal)
//	{
//		maze[goal.first][goal.second] = 0;
//		goal = result[goal.first][goal.second];
//	}
//	maze[start.first][start.second] = 0;
//
//	for (int i = 0; i < 10; ++i)
//	{
//		for (int j = 0; j < 10; ++j)
//		{
//			if (maze[i][j] == -1)
//				cout << "x ";
//			else
//				cout << maze[i][j] << " ";
//		}
//		cout << endl;
//	}
//
//	return 1;
//}