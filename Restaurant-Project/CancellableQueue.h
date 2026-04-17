#ifndef _CANCELLABLE_QUEUE_H_
#define _CANCELLABLE_QUEUE_H_

#include "Queue.h"

template <typename T>
class CancellableQueue : public LinkedQueue<T> {
public:
	bool CancelOrder(int id, T& cancelledItem) {
		int currentSize = this->getCount();
		bool found = false;
		for (int i = 0; i < currentSize; i++) {
			T temp = this->peekFront();
			this->dequeue();
			if (!found && temp->getID() == id) {
				cancelledItem = temp;
				found = true;
			}
			else {
				this->enqueue(temp);
			}
		}
		return found;
	}
};

#endif