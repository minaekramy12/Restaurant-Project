#ifndef _ORDER_H_
#define _ORDER_H_

#include <iostream>
#include <string>

using std::string;

// Forward declarations
class Chef;
class Table;
class Scooter;

enum OrderState {
	PENDING,
	COOKING,
	READY,
	IN_SERVICE,
	FINISHED,
	CANCELLED
};

class Order
{
private:
	Chef* chef;
	int ID;
	int TQ;
	int TA;
	int TR;
	int TS;
	int Tserv;
	int size;
	double price;
	OrderState status;

public:
	Order(int id, int tq, int sz, double pr);

	// Setters
	void setID(int id);
	void setTQ(int tq);
	void setSize(int sz);
	void setPrice(double pr);
	void setAssignedChef(Chef* c);
	void setTA(int ta);
	void setTR(int tr);
	void setTS(int ts);
	void setTserv(int t);
	void setStatus(OrderState s);

	// Getters
	int getID() const;
	int getTQ() const;
	int getTA() const;
	int getTR() const;
	int getTS() const;
	int getTI() const;
	int getTC() const;
	int getTW() const;
	int getTserv() const;
	int getTF() const;
	int getSize() const;
	double getPrice() const;
	Chef* getAssignedChef() const;
	OrderState getStatus() const;
	virtual double getPriority() const;

	virtual string GetType() const = 0;
	virtual Table* getTable() const { return nullptr; }
	virtual Scooter* getScooter() const { return nullptr; }

	// Polymorphic Print function
	virtual void printOrder(std::ostream& os) const;

	friend std::ostream& operator<<(std::ostream& os, const Order* ord) {
		if (ord) ord->printOrder(os);
		return os;
	}

	virtual ~Order();
};

class DineInOrder : public Order
{
private:
	int nSeats;
	int duration;
	bool share;
	Table* table;

public:
	DineInOrder(int id, int tq, int sz, double pr, int seats, int dur, bool sh);
	// Setters
	void setNSeats(int seats);
	void setDuration(int dur);
	void setShare(bool sh);
	void setTable(Table* tab);

	// Getters
	int getNSeats() const;
	int getDuration() const;
	bool getShare() const;
	Table* getTable() const;
};

class ODGOrder : public DineInOrder
{
public:
	ODGOrder(int id, int tq, int sz, double pr, int seats, int dur, bool sh);
	virtual string GetType() const { return "ODG"; }
};

class ODNOrder : public DineInOrder
{
public:
	ODNOrder(int id, int tq, int sz, double pr, int seats, int dur, bool sh);
	virtual string GetType() const { return "ODN"; }
};

class TakeawayOrder : public Order
{
public:
	TakeawayOrder(int id, int tq, int sz, double pr);
	virtual string GetType() const { return "OT"; }
};

class DeliveryOrder : public Order
{
private:
	double distance;
	Scooter* scooter;

public:
	DeliveryOrder(int id, int tq, int sz, double pr, double dis);

	// Setters
	void setDistance(double dis);
	void setScooter(Scooter* sc);

	// Getters
	double getDistance() const;
	virtual Scooter* getScooter() const;
};

class OVC : public DeliveryOrder
{
public:
	OVC(int id, int tq, int sz, double pr, double dis);
	virtual string GetType() const { return "OVC"; }
};

class OVG : public DeliveryOrder
{
public:
	OVG(int id, int tq, int sz, double pr, double dis);
	virtual double getPriority() const;
	virtual string GetType() const { return "OVG"; }
};

class OVN : public DeliveryOrder
{
public:
	OVN(int id, int tq, int sz, double pr, double dis);
	virtual string GetType() const { return "OVN"; }
};

class COMBO : public DeliveryOrder {
private:
	int reqChefs;            //chefs it needs "1 to 4"
	int reqScooters;         //scooters it needs "2+"

	Chef* extraChefs[3];     //max 3 extra chefs (base Order class has the 1st CS chef)
	int extraChefCount;

	Scooter** extraScooters; //for remaining scooters (one in the base)
	int extraScooterCount;

public:
	COMBO(int id, int tq, int sz, double pr, double dis, int rChefs, int rScooters);

	~COMBO();

	virtual string GetType() const;

	int getReqChefs() const;
	int getReqScooters() const;

	void addExtraChef(Chef* pChef);
	Chef** getExtraChefs();
	int getExtraChefCount() const;

	void addExtraScooter(Scooter* pScooter);
	Scooter** getExtraScooters();
	int getExtraScooterCount() const;

	//print for COMBO
	virtual void printOrder(std::ostream& os) const;
};

#endif