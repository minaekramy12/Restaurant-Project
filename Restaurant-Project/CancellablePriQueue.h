#ifndef _CANCELLABLE_PRI_QUEUE_H_
#define _CANCELLABLE_PRI_QUEUE_H_

#include "PriQueue.h"

template <typename T>
class CancellablePriQueue : public priQueue<T> {
public:
	bool CancelOrder(int id, T& cancelledItem) {
		int currentSize = this->getCount();
		bool found = false;
		for (int i = 0; i < currentSize; i++) {
			T temp;
			double pri;
			this->dequeue(temp, pri);
			if (!found && temp->getID() == id) {
				cancelledItem = temp;
				found = true;
			}
			else {
				this->enqueue(temp, pri);
			}
		}
		return found;
	}
};

#endif