#ifndef _CANCELLABLE_PRI_QUEUE_H_
#define _CANCELLABLE_PRI_QUEUE_H_

#include "PriQueue.h"
#include "Queue.h"

template <typename T>
class CancellablePriQueue : public priQueue<T> {
public:
	bool CancelOrder(int id, T& cancelledItem) {
		bool found = false;
		LinkedQueue<T> tempQueue;
		LinkedQueue<double> tempPriQueue;
		int currentSize = this->getCount();
		for (int i = 0; i < currentSize; i++) {
			T temp;
			double pri;
			this->dequeue(temp, pri);
			if (!found && temp->getID() == id) {
				cancelledItem = temp;
				found = true;
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