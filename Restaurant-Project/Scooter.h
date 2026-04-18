#ifndef SCOOTER_H
#define SCOOTER_H

#include <iostream>

class Order;
class DeliveryOrder;

enum ScooterState {
	Available,
	Back,
	InMaintainance
};

class Scooter
{
private:
	int speed;
	int ID;
	int Main_Ords; //number of orders it can deliver befor needing maintenance
	int Main_Dur; //maintenance duration 
	int ordersDoneCount;
	int totalDistance;
	int backTime;
	DeliveryOrder* currentOrder;
	int totalBusyTime;
	int maintenanceFinishTime;
	ScooterState state;
public:
	Scooter(int id, int sp, int cap, int mOrds, int mDur);

	// Setters
	void setID(int id);
	void setMOrders(int morders);
	void setMDur(int mdur);
	void setMaintenance(int curTime);
	void setAvailable();
	void setBack();
	void deliverOrder(Order* ord, int currentTime);
	void setState(ScooterState s);

	friend std::ostream& operator<<(std::ostream& os, const Scooter* s);

	// Getters
	int getID() const;
	int getSpeed() const;
	int getBackTime() const;
	int getTotalBusyTime() const;
	ScooterState getState() const;
	double getPriority() const;

	~Scooter();
};

#endif