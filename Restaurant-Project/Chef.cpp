#include "Chef.h"
#include "Order.h"

Chef::Chef(int id, int spd) : speed(spd), currOrder(nullptr), ID(id), isBusy(false), TF(0), totalBusyTime(0) {}

// Setters
void Chef::setID(int id) { ID = id; }
void Chef::setSpeed(int spd) { speed = spd; }
void Chef::setBusy(bool busy) { isBusy = busy; }
void Chef::setTF(int tf) { TF = tf; }
void Chef::setAvailable() { isBusy = false; currOrder = nullptr; TF = 0; }

void Chef::assignOrder(Order* ord, int currentTime) {
	currOrder = ord;
	isBusy = true;
	int cookDuration = ord->getSize() / speed;
	if (ord->getSize() % speed != 0) cookDuration++;
	totalBusyTime += cookDuration;
	ord->setTA(currentTime);
	ord->setTR(currentTime + cookDuration);
	ord->setAssignedChef(this);

	TF = ord->getTR();
}

std::ostream& operator<<(std::ostream& os, const Chef* c) {
	if (!c) return os;
	os << c->ID;
	return os;
}

// Getters
int Chef::getSpeed() const { return speed; }
Order* Chef::getCurrentOrder() const { return currOrder; }
int Chef::getID() const { return ID; }
bool Chef::getBusy() const { return isBusy; }
int Chef::getTF() const { return TF; }
int Chef::getTotalBusyTime() const { return totalBusyTime; }

Chef::~Chef() {}

CS::CS(int id, int spd) : Chef(id, spd) {}

CN::CN(int id, int spd) : Chef(id, spd) {}