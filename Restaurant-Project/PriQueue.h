#include "Node.h"
#include "PrecondViolatedExcep.h"
#include <iostream>
#ifndef _PRI_QUEUE_H_
#define _PRI_QUEUE_H_
using std::cout;

template <typename T>
class priQueue
{
private:
    priNode<T>* head;
    int count;
public:
    priQueue() : head(nullptr), count(0) {}

    ~priQueue() {
        T tmp;
        double p;
        while (dequeue(tmp, p));
    }

    bool enqueue(const T& data, double priority) {
        priNode<T>* newNode = new priNode<T>(data, priority);

        if (head == nullptr || priority > head->getPri()) {

            newNode->setNext(head);
            head = newNode;
            count++;
            return true;
        }

        priNode<T>* current = head;
        while (current->getNext() && priority <= current->getNext()->getPri()) {
            current = current->getNext();
        }
        newNode->setNext(current->getNext());
        current->setNext(newNode);
        count++;
        return true;
    }

    bool dequeue(T& topEntry, double& pri) {
        if (isEmpty())
            return false;

        topEntry = head->getItem(pri);
        priNode<T>* temp = head;
        head = head->getNext();
        delete temp;
        count--;
        return true;
    }

    T peekFront() const throw(PrecondViolatedExcep) {
        if (isEmpty())
            throw PrecondViolatedExcep("Priority Queue is empty");
        double val;
        return head->getItem(val);
    }

    bool isEmpty() const {
        return head == nullptr;
    }

    int getCount() const { return count; }

    void Print() const
    {
        priNode<T>*current = head;
        while (current)
        {
            double p;
            cout << current->getItem(p);
            if (current->getNext() != nullptr) {
                cout << ", ";
            }
            current = current->getNext();
        }
    }
};


#endif