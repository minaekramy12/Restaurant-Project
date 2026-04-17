#ifndef _Table_H_
#define _Table_H_

#include <iostream>
#include "Node.h"

class DineInOrder;

class Table
{
private:
	int capacity;
	int freeSeats;
	int ID;
	bool isShared;
	Node<DineInOrder*>* order;

	bool canFit(int nSeats) const;
	void addOrder(DineInOrder* ord);
	Node<DineInOrder*>* hasOrder(DineInOrder* ord, Node<DineInOrder*>*& prevPtr) const;
	bool deleteOrder(DineInOrder* ord);
	void deleteAllOrders();

public:
	Table(int id, int cap);

	// Setters
	void setCapacity(int cap);
	void setFreeSeats(int free);
	void setID(int id);
	void setShared(bool shared);

	bool reserveTable(DineInOrder* ord, int nSeats, bool shared);
	void setTableFree();
	bool finishOrder(DineInOrder* ord);

	friend std::ostream& operator<<(std::ostream& os, const Table* t);

	// Getters
	int getCapacity() const;
	int getFreeSeats() const;
	int getID() const;
	bool isFull() const;
	bool getShared() const;
	double getPriority() const;

	~Table();
};

#endif