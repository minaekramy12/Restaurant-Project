#ifndef _TABLE_PRI_QUEUE_H_
#define _TABLE_PRI_QUEUE_H_

#include "PriQueue.h"
#include "Queue.h"

template <typename T>
class TablePriQueue : public priQueue<T> {
public:
	bool GetBest(int requiredSeats, T& bestTable) {
		bool found = false;
		LinkedQueue<T> tempQueue;
		LinkedQueue<double> tempPriQueue;
		while (!this->isEmpty()) {
			T temp;
			double pri;
			this->dequeue(temp, pri);
			if (temp->getFreeSeats() >= requiredSeats) {
				bestTable = temp;
				found = true;
				break;
			}
			else {
				tempQueue.enqueue(temp);
				tempPriQueue.enqueue(pri);
			}
		}

		while (!tempQueue.isEmpty()) {
			T t = tempQueue.peekFront();
			double p = tempPriQueue.peekFront();
			tempQueue.dequeue();
			tempPriQueue.dequeue();
			this->enqueue(t, p);
		}
		return found;
	}
};

#endif