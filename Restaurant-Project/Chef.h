#ifndef _Chef_H_
#define _Chef_H_

#include <iostream>

class Order;

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
	Chef(int id, int spd);

	// Setters
	void setID(int id);
	void setSpeed(int spd);
	void setBusy(bool busy);
	void setTF(int tf);
	void setAvailable();
	void assignOrder(Order* ord, int currentTime);

	friend std::ostream& operator<<(std::ostream& os, const Chef* c);

	// Getters
	int getSpeed() const;
	Order* getCurrentOrder() const;
	int getID() const;
	bool getBusy() const;
	int getTF() const;
	int getTotalBusyTime() const;

	virtual ~Chef();
};

class CS : public Chef
{
public:
	CS(int id, int spd);
};

class CN : public Chef
{
public:
	CN(int id, int spd);
};

#endif