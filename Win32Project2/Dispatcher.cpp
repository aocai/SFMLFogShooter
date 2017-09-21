#include "Dispatcher.h"

void Dispatcher::dispatch()
{
	sysclock now = std::chrono::system_clock::now();
	for (auto it = funcs.begin(); it != funcs.end();)
	{
		if (now >= it->first)
		{
			it->second();
			it = funcs.erase(it);
			--count;
		}
		else
			++it;
	}
}

void Dispatcher::accept(std::function<void()> f, float delay)
{
	sysclock now = std::chrono::system_clock::now();
	now += std::chrono::milliseconds(static_cast<int>(delay*1000));
	funcs.push_back(std::make_pair(now, f));
	++count;
}

void Dispatcher::clear()
{
	if (count == 0)
		return;
	std::vector<std::pair<sysclock, std::function<void()>>>().swap(funcs);
	count = 0;
}