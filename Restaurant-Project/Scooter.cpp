#include "Scooter.h"
#include "Order.h"

Scooter::Scooter(int id, int sp, int cap, int mOrds, int mDur)
	: ID(id), speed(sp),
	Main_Ords(mOrds), Main_Dur(mDur),
	ordersDoneCount(0), backTime(0), currentOrder(nullptr), totalBusyTime(0), totalDistance(0),
	state(Available), maintenanceFinishTime(0) {
}

// Setters
void Scooter::setID(int id) { ID = id; }
void Scooter::setMOrders(int morders) { Main_Ords = morders; }
void Scooter::setMDur(int mdur) { Main_Dur = mdur; }
void Scooter::setMaintenance(int curTime) {
	setState(InMaintainance);
	maintenanceFinishTime = curTime + Main_Dur;
}
void Scooter::setAvailable() { setState(Available); }

void Scooter::setBack() { setState(Back); }
void Scooter::deliverOrder(Order* ord, int currentTime) {
	DeliveryOrder* delOrd = dynamic_cast<DeliveryOrder*>(ord); //downcast to make sure its deliverable
	if (!delOrd) return;
	if (speed <= 0) speed = 1;
	int dist = delOrd->getDistance();
	int tripDuration = dist / speed;
	if (dist % speed != 0) tripDuration++;
	totalBusyTime += (2 * tripDuration);
	ord->setTS(currentTime);
	ord->setTserv(tripDuration);
	backTime = currentTime + (2 * tripDuration);
	totalDistance += dist;
	ordersDoneCount++;
	if (ordersDoneCount >= Main_Ords) {
		setState(InMaintainance);
		ordersDoneCount = 0;
	}
}

void Scooter::setState(ScooterState s) { state = s; }

std::ostream& operator<<(std::ostream& os, const Scooter* s) {
	if (!s) return os;
	os << s->ID;
	return os;
}

// Getters
int Scooter::getID() const { return ID; }
int Scooter::getSpeed() const { return speed; }
int Scooter::getBackTime() const { return backTime; }
int Scooter::getTotalBusyTime() const { return totalBusyTime; }
ScooterState Scooter::getState() const { return state; }
double Scooter::getPriority() const {
	if (state == Available) return -(totalDistance);
	if (state == Back) return -getBackTime();
	else return -maintenanceFinishTime;
}

Scooter::~Scooter() {}