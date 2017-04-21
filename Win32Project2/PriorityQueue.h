#pragma once
#include <vector>

template <class T>
struct QueueNode
{
	T node;
	double priority;
};

template <class T>
class PriorityQueue
{
private:
	std::vector<QueueNode<T>> heap;
	void upHeap(int index);
	void downHeap(int index);
	void heapify();
public:
	PriorityQueue(const std::vector<T> &vector);
	PriorityQueue();

	T getHighestPriorityNode();
	double getHighestPriority();
	void insert(T, double);
	void extract();
	bool decreasePriority(T, double);
	void decreasePriority(int index, double newPriority);
	int searchQueue(T);
	bool empty();
	void printHeap();
};

template <class T>
PriorityQueue<T>::PriorityQueue(const std::vector<T> &vector) : heap(vector)
{
	heapify();
}

template <class T>
PriorityQueue<T>::PriorityQueue()
{

}

template <class T>
void PriorityQueue<T>::upHeap(int index)
{
	if (index == 0)
		return;
	int parent = (index - 1) / 2;
	if (heap[parent].priority > heap[index].priority)
	{
		std::swap(heap[parent], heap[index]);
		upHeap(parent);
	}
}

template <class T>
void PriorityQueue<T>::downHeap(int index)
{
	int child1 = 2 * index + 1;
	if (child1 >= heap.size())
		return;

	int minChild;
	int child2 = 2 * index + 2;
	if (child2 < heap.size() && heap[child2].priority < heap[child1].priority)
	{
		minChild = child2;
	}
	else
	{
		minChild = child1;
	}

	if (heap[index].priority > heap[minChild].priority)
	{
		std::swap(heap[index], heap[minChild]);
		downHeap(minChild);
	}
}

template <class T>
void PriorityQueue<T>::heapify()
{
	for (int i = heap.size() / 2; i >= 0; --i)
	{
		downHeap(i);
	}
}

template <class T>
T PriorityQueue<T>::getHighestPriorityNode()
{
	return heap[0].node;
}

template <class T>
double PriorityQueue<T>::getHighestPriority()
{
	return heap[0].priority;
}

template <class T>
void PriorityQueue<T>::insert(T node, double priority)
{
	QueueNode<T> queueNode = { node, priority };
	heap.push_back(queueNode);
	upHeap(heap.size() - 1);
}

template <class T>
void PriorityQueue<T>::extract()
{
	if (heap.size() == 0)
		return;

	heap[0] = heap[heap.size() - 1];
	heap.pop_back();
	downHeap(0);
}

template <class T>
int PriorityQueue<T>::searchQueue(T node)
{
	for (int i = 0; i < heap.size(); ++i)
	{
		if (node == heap[i].node)
		{
			return i;
		}
	}
	return -1;
}

template <class T>
bool PriorityQueue<T>::decreasePriority(T node, double newPriority)
{
	int index = searchQueue(node);
	if (index != -1)
	{
		heap[index].priority = newPriority;
		upHeap(index);
		return true;
	}
	return false;
}

template <class T>
void PriorityQueue<T>::decreasePriority(int index, double newPriority)
{
	heap[index].priority = newPriority;
	upHeap(index);
}

template <class T>
bool PriorityQueue<T>::empty()
{
	return !heap.size();
}

template <class T>
void PriorityQueue<T>::printHeap()
{
	for (int i = 0; i < heap.size(); ++i)
	{
		std::cout << heap[i].priority << " ";
	}
}