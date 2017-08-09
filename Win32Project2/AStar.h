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

double heuristic(int start, int end);
vector<int> getNeighbours(const vector<double> &maze, int position);
bool AStar(const vector<double> &maze, int start, int goal, vector<int> &result);