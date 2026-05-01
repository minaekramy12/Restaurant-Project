#ifndef _ACTION_H_
#define _ACTION_H_

#include <iostream>
#include <string>

class Restaurant; // Forward declaration
using std::string;

class Action {
protected:
	int timestep;
	int orderID;

public:
	Action(int time, int id) : timestep(time), orderID(id) {}
	virtual ~Action() {}

	// Getters
	int getTimestep() const { return timestep; }
	int getOrderID() const { return orderID; }

	virtual void Execute(Restaurant* pRest) = 0;
	virtual void print(std::ostream& os) const = 0;

	friend std::ostream& operator<<(std::ostream& os, const Action* act) {
		if (!act) return os;
		act->print(os);
		return os;
	}
};

class RequestAction : public Action {
private:
	string orderType; // "ODG", "ODN", "OT", "OVC", "OVG", "OVN", "COMBO"
	int size;
	double price;

	// For Dine-in
	int seats;
	int duration;
	bool canShare;

	// For Delivery
	double distance;

	// For COMBO orders
	int reqChefs;
	int reqScooters;

public:
	// ODG, ODN
	RequestAction(int time, int id, string type, int sz, double pr, int s, int dur, bool share)
		: Action(time, id), orderType(type), size(sz), price(pr), seats(s), duration(dur), canShare(share), distance(0), reqChefs(0), reqScooters(0) {
	}

	// OVC, OVG, OVN
	RequestAction(int time, int id, string type, int sz, double pr, double dist)
		: Action(time, id), orderType(type), size(sz), price(pr), seats(0), duration(0), canShare(false), distance(dist), reqChefs(0), reqScooters(0) {
	}

	// OT
	RequestAction(int time, int id, string type, int sz, double pr)
		: Action(time, id), orderType(type), size(sz), price(pr), seats(0), duration(0), canShare(false), distance(0), reqChefs(0), reqScooters(0) {
	}

	// COMBO
	RequestAction(int ts, int id, string typ, int sz, double mon, double dist, int rChefs, int rScooters)
		: Action(ts, id), orderType(typ), size(sz), price(mon), seats(0), duration(0), canShare(false), distance(dist), reqChefs(rChefs), reqScooters(rScooters)
	{
	}

	virtual void Execute(Restaurant* pRest) override;

	virtual void print(std::ostream& os) const override {
		os << "[" << orderType << ", " << timestep << ", " << orderID << "]";
	}
};

class CancelAction : public Action {
public:
	CancelAction(int time, int id) : Action(time, id) {}

	virtual void Execute(Restaurant* pRest) override;

	virtual void print(std::ostream& os) const override {
		os << "(X, " << timestep << ", " << orderID << ")";
	}
};

#endif