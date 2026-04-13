#ifndef _ORDER_H_
#define _ORDER_H_
#include "Chef.h"
#include "Table.h"
#include "Scooter.h"

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

public:
	Order(int id, int tq, int sz, double pr) : ID(id), TQ(tq), size(sz), price(pr),
		chef(nullptr), Tserv(0), TA(0), TR(0), TS(0) {}
	// Setters
	void setID(int id) { ID = id; }
	void setTQ(int tq) { TQ = tq; }
	void setSize(int sz) { size = sz; }
	void setPrice(double pr) { price = pr; }
	void setAssignedChef(Chef* c) { chef = c; }
	void setTA(int ta) { TA = ta; }
	void setTR(int tr) { TR = tr; }
	void setTS(int ts) { TS = ts; }
	void setTserv(int t) { Tserv = t; }

	// Getters
	int getID() const { return ID; }
	int getTQ() const { return TQ; }
	int getTA() const { return TA; }
	int getTR() const { return TR; }
	int getTS() const { return TS; }
	int getTI() const { return TA - TQ + TS - TR; }
	int getTC() const { return TR - TA; }
	int getTW() const { return getTI() + getTC(); }
	int getTserv() const { return Tserv; }
	int getTF() const { return TS + Tserv; }
	int getSize() const { return size; }
	double getPrice() const { return price; }
	Chef* getAssignedChef() const { return chef; }
	virtual double getPriority() const { return -TQ; }

	virtual ~Order() {}
};






class DineInOrder : public Order
{
private:
	int nSeats;
	int duration;
	bool share;
	Table* table;
public:
	DineInOrder(int id, int tq, int sz, double pr, int seats, int dur, bool sh) :
		Order(id, tq, sz, pr), nSeats(seats), duration(dur), share(sh), table(nullptr) {}
	// Setters
	void setNSeats(int seats) { nSeats = seats; }
	void setDuration(int dur) { duration = dur; }
	void setShare(bool sh) { share = sh; }
	void setTable(Table* tab) { table = tab; }

	// Getters
	int getNSeats() const { return nSeats; }
	int getDuration() const { return duration; }
	bool getShare() const { return share; }
	Table* getTable() const { return table; }

};



class ODGOrder : public DineInOrder
{
public:
	ODGOrder(int id, int tq, int sz, double pr, int seats, int dur, bool sh) :
		DineInOrder (id, tq, sz, pr, seats, dur, sh) {}
};


class ODNOrder : public DineInOrder
{
public:
	ODNOrder(int id, int tq, int sz, double pr, int seats, int dur, bool sh) :
		DineInOrder(id, tq, sz, pr, seats, dur, sh) {
	}
};



class TakeawayOrder : public Order
{
public:
	TakeawayOrder(int id, int tq, int sz, double pr) : Order(id, tq, sz, pr) {}
};


class DeliveryOrder : public Order
{
private:
	double distance;
	Scooter* scooter;
public:
	DeliveryOrder(int id, int tq, int sz, double pr, double dis) :
		Order(id, tq, sz, pr), distance(dis), scooter(nullptr) {}

	// Setters
	void setDistance(double dis) { distance = dis; }
	void setScooter(Scooter* sc) { scooter = sc; }
	// Getters
	double getDistance() const { return distance; }
	Scooter* getScooter() const { return scooter; }
};


class OVC : public DeliveryOrder
{
public:
	OVC(int id, int tq, int sz, double pr, double dis) : DeliveryOrder(id, tq, sz, pr, dis) {}
};

class OVG : public DeliveryOrder
{
public:
	OVG(int id, int tq, int sz, double pr, double dis) : DeliveryOrder(id, tq, sz, pr, dis) {}

	double getPriority() const override {
		double dist = (getDistance() > 0) ? getDistance() : 1.0;
		return (getPrice() * getSize()) / dist;
	}
};

class OVN : public DeliveryOrder
{
public:
	OVN(int id, int tq, int sz, double pr, double dis) : DeliveryOrder(id, tq, sz, pr, dis) {}
};
#endif