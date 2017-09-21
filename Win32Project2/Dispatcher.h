#pragma once
#include <unordered_map>
#include <functional>
#include <chrono>

//typedef std::chrono::time_point<std::chrono::system_clock> sysclock;
typedef std::chrono::system_clock::time_point sysclock;

class Dispatcher
{
private:
	int count = 0;
	std::vector<std::pair<sysclock, std::function<void()>>> funcs;

public:
	void dispatch();
	void accept(std::function<void()> f, float delay);
	void clear();
};