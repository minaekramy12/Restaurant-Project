#ifndef SCOOTER_H
#define SCOOTER_H

#include <iostream>

class Order;
class DeliveryOrder;

class Scooter
{
private:
	int speed;
	int ID;
	int Main_Ords; //number of orders it can deliver befor needing maintenance
	int Main_Dur; //maintenance duration 
	bool inMaintenance;
	int ordersDoneCount;
	int backTime;
	DeliveryOrder* currentOrder;
	int totalBusyTime;
public:
	Scooter(int id, int sp, int cap, int mOrds, int mDur);

	// Setters
	void setID(int id);
	void setMOrders(int morders);
	void setMDur(int mdur);
	void setMaintenance(bool m);
	void deliverOrder(Order* ord, int currentTime);

	friend std::ostream& operator<<(std::ostream& os, const Scooter* s);

	// Getters
	int getID() const;
	int getSpeed() const;
	bool getInMaintenance() const;
	int getBackTime() const;
	int getTotalBusyTime() const;

	~Scooter();
};

#endif