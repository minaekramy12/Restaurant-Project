#ifndef _Chef_H_
#define _Chef_H_
#include "Order.h"


class Chef
{
private:
	int speed;
	int ID;
	Order* currOrder;
	bool isBusy;
	int TF;
	int totalBusyTime;

public:
	Chef(int id, int spd) : speed(spd), currOrder(nullptr), ID(id), isBusy(false), TF(0), totalBusyTime(0) {}
	// Setters
	void setID(int id) { ID = id; }
	void setSpeed(int spd) { speed = spd; }
	void setBusy(bool busy) { isBusy = busy; }
	void setTF(int tf) { TF = tf; }
	void setAvailable() { isBusy = false; currOrder = nullptr; TF = 0; }
	void assignOrder(Order* ord, int currentTime) {
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


	// Getters
	int getSpeed() const { return speed; }
	Order* getCurrentOrder() const { return currOrder; }
	int getID() const { return ID; }
	bool getBusy() const { return isBusy; }
	int getTF() const { return TF; }
	int getTotalBusyTime() const { return totalBusyTime; }

	virtual ~Chef() {}
};

class CS : public Chef
{
public:
	CS(int id, int spd) : Chef(id, spd) {}
};

class CN : public Chef
{
public:
	CN(int id, int spd) : Chef(id, spd) {}
};

#endif 