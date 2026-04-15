#ifndef _Table_H_
#define _Table_H_

#include "Order.h"
#include "Node.h"

class Table
{
private:
	int capacity;
	int freeSeats;
	int ID;
	bool isShared;
	Node<DineInOrder*>* order;


	bool canFit(int nSeats) const { return freeSeats >= nSeats; }
	void addOrder(DineInOrder* ord)
	{
		Node<DineInOrder*>* temp = new Node<DineInOrder*>(ord);
		temp->setNext(order);
		order = temp;
	}

	Node<DineInOrder*>* hasOrder(DineInOrder* ord, Node<DineInOrder*>*& prevPtr) const
	{
		Node<DineInOrder*>* current = order;
		prevPtr = nullptr;
		while (current != nullptr)
		{
			if (current->getItem() == ord) return current;
			prevPtr = current;
			current = current->getNext();
		}
		return nullptr;
	}

	bool deleteOrder(DineInOrder* ord)
	{
		Node<DineInOrder*>* prevPtr = nullptr;
		Node<DineInOrder*>* deleteNode = hasOrder(ord, prevPtr);
		if (deleteNode)
		{
			if (deleteNode == order) {
				order = deleteNode->getNext();
				delete deleteNode;
				deleteNode = nullptr;
				return true;
			}
			prevPtr->setNext(deleteNode->getNext());
			delete deleteNode;
			deleteNode = nullptr;
			return true;
		}
		return false;
	}

	void deleteAllOrders()
	{
		Node<DineInOrder*>* current = order;
		while (current != nullptr)
		{
			Node<DineInOrder*>* temp = current;
			current = current->getNext();
			delete temp;
			temp = nullptr;
		}
		order = nullptr;
	}

public:
	Table(int id, int cap) { capacity = cap; freeSeats = cap; ID = id; order = nullptr; isShared = false; }
	// Setters
	void setCapacity(int cap) { capacity = cap; }
	void setFreeSeats(int free) { freeSeats = free; }
	void setID(int id) { ID = id; }
	void setShared(bool shared) { isShared = shared; }


	bool reserveTable(DineInOrder* ord, int nSeats, bool shared)
	{
		if (!canFit(nSeats)) return false;
		freeSeats -= nSeats;
		setShared(shared);
		addOrder(ord);
		return true;
	}
	void setTableFree()
	{
		freeSeats = capacity;
		isShared = false;
		deleteAllOrders();
	}
	bool finishOrder(DineInOrder* ord)
	{
		if (deleteOrder(ord))
		{
			freeSeats += ord->getNSeats();
			if (freeSeats == capacity) isShared = false;
			return true;
		}
		return false;
	}


	// Getters
	int getCapacity() const { return capacity; }
	int getFreeSeats() const { return freeSeats; }
	int getID() const { return ID; }
	bool isFull() const { return freeSeats == capacity; }
	bool getShared() const { return isShared; }



	~Table() { deleteAllOrders(); }
};

#endif