
#include "Node.h"
#ifndef _STACK_H_
#define _STACK_H_


template<typename T>
class LinkedStack
{
private:
	Node<T>* topPtr;
	int count;
public:
	LinkedStack()
	{
		topPtr = nullptr;
		count = 0;
	}
	LinkedStack(const LinkedStack& aStack)
	{
		count = aStack.count;
		Node<T>* nodePtr = aStack.topPtr;
		if (nodePtr == nullptr)
			topPtr = nullptr;
		else
		{
			topPtr = new Node<T>();
			topPtr->setItem(nodePtr->getItem());
			Node<T>* newNodePtr = topPtr;
			nodePtr = nodePtr->getNext();
			while (nodePtr != nullptr)
			{
				Node<T>* nextNodePtr = new Node<T>();
				nextNodePtr->setItem(nodePtr->getItem());
				newNodePtr->setNext(nextNodePtr);
				newNodePtr = nextNodePtr;
				nodePtr = nodePtr->getNext();
			}
			newNodePtr->setNext(nullptr);
		}
	}
	~LinkedStack()
	{
		while (!isEmpty())
			pop();
	}
	bool isEmpty() const
	{
		return (topPtr == nullptr);
	}
	bool push(const T& newEntry)
	{
		Node<T>* newNodePtr = new Node<T>(newEntry);
		newNodePtr->setNext(topPtr);
		topPtr = newNodePtr;
		newNodePtr = nullptr;
		count++;
		return true;
	}
	bool pop()
	{
		bool result = false;
		if (!isEmpty())
		{
			Node<T>* nodeToDeletePtr = topPtr;
			topPtr = topPtr->getNext();
			nodeToDeletePtr->setNext(nullptr);
			delete nodeToDeletePtr;
			nodeToDeletePtr = nullptr;
			result = true;
			count--;
		}
		return result;
	}
	T peak() const throw(PrecondViolatedExcep)
	{
		if (isEmpty())
			throw PrecondViolatedExcep("Stack is empty");
		return topPtr->getItem();
	}
	int getCount() const
	{
		return count;
	}
	void Print() const
	{
		Node<T>* current = topPtr;
		while (current)
		{
			cout << current->getItem() << endl;
			current = current->getNext();
		}
	}
};

#endif