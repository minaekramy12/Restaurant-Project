#ifndef _TABLE_PRI_QUEUE_H_
#define _TABLE_PRI_QUEUE_H_

#include "PriQueue.h"

template <typename T>
class TablePriQueue : public priQueue<T> {
public:
    bool GetBest(int requiredSeats, T& bestTable) {
        int currentSize = this->getCount();
        bool found = false;
        LinkedQueue<T> tempQueue;
        LinkedQueue<double> tempPriQueue;
        for (int i = 0; i < currentSize; i++) {
            T temp;
            double pri;
            this->dequeue(temp, pri);
            if (!found && temp->getFreeSeats() >= requiredSeats) {
                bestTable = temp;
                found = true;
            }
            else {
                tempQueue.enqueue(temp);
                tempPriQueue.enqueue(pri);
            }
        }
        while (!tempQueue.isEmpty()) {
            T t; double p;
            t = tempQueue.peekFront();
            p = tempPriQueue.peekFront();
            tempQueue.dequeue();
            tempPriQueue.dequeue();
            this->enqueue(t, p);
        }
        return found;
    }
};

#endif