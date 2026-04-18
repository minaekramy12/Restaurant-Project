#include "Scooter.h"
#include "Order.h"

Scooter::Scooter(int id, int sp, int cap, int mOrds, int mDur)
	: ID(id), speed(sp),
	Main_Ords(mOrds), Main_Dur(mDur),
	ordersDoneCount(0), backTime(0), inMaintenance(false), currentOrder(nullptr), totalBusyTime(0) {
}

// Setters
void Scooter::setID(int id) { ID = id; }
void Scooter::setMOrders(int morders) { Main_Ords = morders; }
void Scooter::setMDur(int mdur) { Main_Dur = mdur; }
void Scooter::setMaintenance(bool m) { inMaintenance = m; }

void Scooter::deliverOrder(Order* ord, int currentTime) {
	DeliveryOrder* delOrd = dynamic_cast<DeliveryOrder*>(ord); //downcast to make sure its deliverable
	if (!delOrd) return;
	currentOrder = (DeliveryOrder*)ord;
	int dist = delOrd->getDistance();
	int tripDuration = dist / speed;
	if (dist % speed != 0) tripDuration++;
	totalBusyTime += (2 * tripDuration);
	ord->setTS(currentTime);
	ord->setTserv(tripDuration);
	backTime = currentTime + (2 * tripDuration);
	ordersDoneCount++;
	if (ordersDoneCount >= Main_Ords) {
		inMaintenance = true;
		backTime += Main_Dur;
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
bool Scooter::getInMaintenance() const { return inMaintenance; }
int Scooter::getBackTime() const { return backTime; }
int Scooter::getTotalBusyTime() const { return totalBusyTime; }
ScooterState Scooter::getState() const { return state; }
double Scooter::getPriority(DeliveryOrder* ord) const {
	if (state == Available) return -(ord->getDistance());

}

Scooter::~Scooter() {}