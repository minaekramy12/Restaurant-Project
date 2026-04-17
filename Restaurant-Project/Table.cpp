#include "Table.h"
#include "Order.h"

Table::Table(int id, int cap) { capacity = cap; freeSeats = cap; ID = id; order = nullptr; isShared = false; }

bool Table::canFit(int nSeats) const { return freeSeats >= nSeats; }

void Table::addOrder(DineInOrder* ord)
{
	Node<DineInOrder*>* temp = new Node<DineInOrder*>(ord);
	temp->setNext(order);
	order = temp;
}

Node<DineInOrder*>* Table::hasOrder(DineInOrder* ord, Node<DineInOrder*>*& prevPtr) const
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

bool Table::deleteOrder(DineInOrder* ord)
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

void Table::deleteAllOrders()
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

// Setters
void Table::setCapacity(int cap) { capacity = cap; }
void Table::setFreeSeats(int free) { freeSeats = free; }
void Table::setID(int id) { ID = id; }
void Table::setShared(bool shared) { isShared = shared; }

bool Table::reserveTable(DineInOrder* ord, int nSeats, bool shared)
{
	if (!canFit(nSeats)) return false;
	freeSeats -= nSeats;
	setShared(shared);
	addOrder(ord);
	return true;
}

void Table::setTableFree()
{
	freeSeats = capacity;
	isShared = false;
	deleteAllOrders();
}

bool Table::finishOrder(DineInOrder* ord)
{
	if (deleteOrder(ord))
	{
		freeSeats += ord->getNSeats();
		if (freeSeats == capacity) isShared = false;
		return true;
	}
	return false;
}

std::ostream& operator<<(std::ostream& os, const Table* t) {
	if (!t) return os;
	os << "[T" << t->ID << ", " << t->capacity << ", " << t->freeSeats << "]";
	return os;
}

// Getters
int Table::getCapacity() const { return capacity; }
int Table::getFreeSeats() const { return freeSeats; }
int Table::getID() const { return ID; }
bool Table::isFull() const { return freeSeats == capacity; }
bool Table::getShared() const { return isShared; }

Table::~Table() { deleteAllOrders(); }