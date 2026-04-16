#include "Node.h"
#include "PrecondViolatedExcep.h"
#ifndef _QUEUE_H_
#define _QUEUE_H_

template <typename T>
class LinkedQueue
{
private:
	Node<T>* backPtr;
	Node<T>* frontPtr;
	int count;
public:
	LinkedQueue()
	{
		backPtr = nullptr;
		frontPtr = nullptr;
		count = 0;
	}

	LinkedQueue(const LinkedQueue& aQueue)
	{
		count = aQueue.count;
		Node<T>* nodePtr = aQueue.frontPtr;
		if (nodePtr == nullptr)
		{
			frontPtr = nullptr;
			backPtr = nullptr;
		}
		else
		{
			frontPtr = new Node<T>();
			frontPtr->setItem(nodePtr->getItem());
			Node<T>* newNodePtr = frontPtr;
			nodePtr = nodePtr->getNext();
			while (nodePtr != nullptr)
			{
				Node<T>* nextNodePtr = new Node<T>();
				nextNodePtr->setItem(nodePtr->getItem());
				newNodePtr->setNext(nextNodePtr);
				newNodePtr = nextNodePtr;
				nodePtr = nodePtr->getNext();
			}
			backPtr = newNodePtr;
			backPtr->setNext(nullptr);
		}
	}
	~LinkedQueue()
	{
		while (!isEmpty())
			dequeue();
	}
	bool isEmpty() const
	{
		return (frontPtr == nullptr);
	}

	bool enqueue(const T& newEntry)
	{
		Node<T>* newNodePtr = new Node<T>(newEntry);
		if (isEmpty())
			frontPtr = newNodePtr;
		else
			backPtr->setNext(newNodePtr);
		backPtr = newNodePtr;
		count++;
		return true;
	}

	bool dequeue()
	{
		bool result = false;
		if (!isEmpty())
		{
			Node<T>* nodeToDeletePtr = frontPtr;
			if (frontPtr == backPtr)
			{
				frontPtr = nullptr;
				backPtr = nullptr;
			}
			else
				frontPtr = frontPtr->getNext();
			nodeToDeletePtr->setNext(nullptr);
			delete nodeToDeletePtr;
			nodeToDeletePtr = nullptr;
			result = true;
			count--;
		}
		return result;
	}
	T peekFront() const throw(PrecondViolatedExcep)
	{
		if (isEmpty())
			throw PrecondViolatedExcep("Queue is empty");
		return frontPtr->getItem();
	}
	int getCount() const
	{
		return count;
	}
	void Print() const
	{
		Node<T>* current = frontPtr;
		while (current)
		{
			cout << current->getItem() << endl;
			current = current->getNext();
		}
	}
};
#endif _QUEUE_H_