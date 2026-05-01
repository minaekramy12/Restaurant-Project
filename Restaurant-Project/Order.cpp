#include "Order.h"
#include "Chef.h"
#include "Table.h"
#include "Scooter.h"

Order::Order(int id, int tq, int sz, double pr) : ID(id), TQ(tq), size(sz), price(pr),
chef(nullptr), Tserv(0), TA(0), TR(0), TS(0), status(PENDING) {
}

// Setters
void Order::setID(int id) { ID = id; }
void Order::setTQ(int tq) { TQ = tq; }
void Order::setSize(int sz) { size = sz; }
void Order::setPrice(double pr) { price = pr; }
void Order::setAssignedChef(Chef* c) { chef = c; }
void Order::setTA(int ta) { TA = ta; }
void Order::setTR(int tr) { TR = tr; }
void Order::setTS(int ts) { TS = ts; }
void Order::setTserv(int t) { Tserv = t; }
void Order::setStatus(OrderState s) { status = s; }

// Getters
int Order::getID() const { return ID; }
int Order::getTQ() const { return TQ; }
int Order::getTA() const { return TA; }
int Order::getTR() const { return TR; }
int Order::getTS() const { return TS; }
int Order::getTI() const { return TA - TQ + TS - TR; }
int Order::getTC() const { return TR - TA; }
int Order::getTW() const { return getTI() + getTC(); }
int Order::getTserv() const { return Tserv; }
int Order::getTF() const { return TS + Tserv; }
int Order::getSize() const { return size; }
double Order::getPrice() const { return price; }
Chef* Order::getAssignedChef() const { return chef; }
double Order::getPriority() const { return -TQ; }
OrderState Order::getStatus() const { return status; }

// Virtual Base Print Method
void Order::printOrder(std::ostream& os) const {
	switch (status) {
	case IN_SERVICE:
	{
		string type = GetType();
		if (type == "ODG" || type == "ODN") {
			os << "[" << ID << ", T";
			if (getTable()) os << getTable()->getID();
			os << "]";
		}
		else {
			os << "[" << ID << ", S";
			if (getScooter()) os << getScooter()->getID();
			os << "]";
		}
		break;
	}
	case COOKING:
	{
		os << "[" << ID << ", ";
		if (chef) os << chef->getID();
		os << "]";
		break;
	}
	default:
	{
		os << ID;
		break;
	}
	}
}

Order::~Order() {}

DineInOrder::DineInOrder(int id, int tq, int sz, double pr, int seats, int dur, bool sh) :
	Order(id, tq, sz, pr), nSeats(seats), duration(dur), share(sh), table(nullptr) {
}

void DineInOrder::setNSeats(int seats) { nSeats = seats; }
void DineInOrder::setDuration(int dur) { duration = dur; }
void DineInOrder::setShare(bool sh) { share = sh; }
void DineInOrder::setTable(Table* tab) { table = tab; }
int DineInOrder::getNSeats() const { return nSeats; }
int DineInOrder::getDuration() const { return duration; }
bool DineInOrder::getShare() const { return share; }
Table* DineInOrder::getTable() const { return table; }

ODGOrder::ODGOrder(int id, int tq, int sz, double pr, int seats, int dur, bool sh) :
	DineInOrder(id, tq, sz, pr, seats, dur, sh) {
}
ODNOrder::ODNOrder(int id, int tq, int sz, double pr, int seats, int dur, bool sh) :
	DineInOrder(id, tq, sz, pr, seats, dur, sh) {
}

TakeawayOrder::TakeawayOrder(int id, int tq, int sz, double pr) : Order(id, tq, sz, pr) {}

DeliveryOrder::DeliveryOrder(int id, int tq, int sz, double pr, double dis) :
	Order(id, tq, sz, pr), distance(dis), scooter(nullptr) {
}

void DeliveryOrder::setDistance(double dis) { distance = dis; }
void DeliveryOrder::setScooter(Scooter* sc) { scooter = sc; }
double DeliveryOrder::getDistance() const { return distance; }
Scooter* DeliveryOrder::getScooter() const { return scooter; }

OVC::OVC(int id, int tq, int sz, double pr, double dis) : DeliveryOrder(id, tq, sz, pr, dis) {}
OVG::OVG(int id, int tq, int sz, double pr, double dis) : DeliveryOrder(id, tq, sz, pr, dis) {}
double OVG::getPriority() const {
	double dist = (getDistance() > 0) ? getDistance() : 1.0;
	int TC = (getTC() > 0) ? getTC() : 1;
	return (getPrice() * getSize()) / (dist * TC);
}
OVN::OVN(int id, int tq, int sz, double pr, double dis) : DeliveryOrder(id, tq, sz, pr, dis) {}


COMBO::COMBO(int id, int tq, int sz, double pr, double dis, int rChefs, int rScooters)
	: DeliveryOrder(id, tq, sz, pr, dis)
{
	reqChefs = rChefs;
	reqScooters = rScooters;

	extraChefCount = 0;
	extraScooterCount = 0;

	for (int i = 0; i < 3; i++) {
		extraChefs[i] = nullptr;
	}

	// allocate total required - 1 scooter in base
	int extraNeeded = reqScooters - 1;
	if (extraNeeded > 0) {
		extraScooters = new Scooter * [extraNeeded];
		for (int i = 0; i < extraNeeded; i++) {
			extraScooters[i] = nullptr;
		}
	}
	else {
		extraScooters = nullptr;
	}
}

COMBO::~COMBO() {
	if (extraScooters != nullptr) {
		delete[] extraScooters;
	}
}
string COMBO::GetType() const { return "COMBO"; }
int COMBO::getReqChefs() const { return reqChefs; }
int COMBO::getReqScooters() const { return reqScooters; }

void COMBO::addExtraChef(Chef* pChef) {
	if (extraChefCount < (reqChefs - 1)) {
		extraChefs[extraChefCount++] = pChef;
	}
}
Chef** COMBO::getExtraChefs() { return extraChefs; }
int COMBO::getExtraChefCount() const { return extraChefCount; }

void COMBO::addExtraScooter(Scooter* pScooter) {
	if (extraScooterCount < (reqScooters - 1)) {
		extraScooters[extraScooterCount++] = pScooter;
	}
}
Scooter** COMBO::getExtraScooters() { return extraScooters; }
int COMBO::getExtraScooterCount() const { return extraScooterCount; }

//Print for COMBO
void COMBO::printOrder(std::ostream& os) const {
	if (getStatus() == COOKING) {
		os << "[" << getID() << ", ";
		if (getAssignedChef()) os << getAssignedChef()->getID();
		for (int i = 0; i < extraChefCount; i++) {
			if (extraChefs[i]) os << " + " << extraChefs[i]->getID();
		}
		os << "]";
	}
	else if (getStatus() == IN_SERVICE) {
		os << "[" << getID() << ", S";
		if (getScooter()) os << getScooter()->getID();
		for (int i = 0; i < extraScooterCount; i++) {
			if (extraScooters[i]) os << " + S" << extraScooters[i]->getID();
		}
		os << "]";
	}
	else {
		os << getID();
	}
}