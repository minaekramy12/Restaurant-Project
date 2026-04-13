#ifndef SCOOTER_H
#define SCOOTER_H
#include "Order.h"

class Scooter
{
private:
	int speed;
	int ID;
	int Main_Ords;
	int Main_Dur;
	bool inMaintenance;
	int ordersDoneCount;
	int backTime;
	DeliveryOrder* currentOrder;
	int totalBusyTime;
public:
	Scooter(int id, int sp, int cap, int mOrds, int mDur)
		: ID(id), speed(sp),
		Main_Ords(mOrds), Main_Dur(mDur),
		ordersDoneCount(0), backTime(0), inMaintenance(false), currentOrder(nullptr), totalBusyTime(0) {}
	// Setters
	void setID(int id) { ID = id; }
	void setMOrders(int morders) { Main_Ords = morders; }
	void setMDur(int mdur) { Main_Dur = mdur; }
	void setMaintenance(bool m) { inMaintenance = m; }
	void deliverOrder(Order* ord, int currentTime) {
		DeliveryOrder* delOrd = dynamic_cast<DeliveryOrder*>(ord);
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

	// Getters
	int getID() const { return ID; }
	int getSpeed() const { return speed; }
	bool getInMaintenance() const { return inMaintenance; }
	int getBackTime() const { return backTime; }
	int getTotalBusyTime() const { return totalBusyTime; }

	~Scooter() {}
};


#endif