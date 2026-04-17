#ifndef __RESTAURANT_H_
#define __RESTAURANT_H_

#include <iostream>
#include <fstream>
#include <string>

#include "Queue.h"
#include "PriQueue.h"
#include "Stack.h"
#include "CancellableQueue.h"
#include "CancellablePriQueue.h"
#include "TablePriQueue.h"

#include "Order.h"
#include "Chef.h"
#include "Scooter.h"
#include "Table.h"
#include "Action.h"
#include "UI.h"

using namespace std;

class Restaurant {
private:
	UI* pUI;
	int currentTimestep;
	LinkedQueue<Action*> actionList;

	// Pending Orders
	LinkedQueue<Order*> pendTakeaway;
	LinkedQueue<Order*> pendODN;
	LinkedQueue<Order*> pendODG;
	CancellableQueue<Order*> pendOVC;
	priQueue<Order*> pendOVG;
	LinkedQueue<Order*> pendOVN;

	// Available Resources
	LinkedQueue<Chef*> availCS;
	LinkedQueue<Chef*> availCN;
	priQueue<Scooter*> availScooters;

	TablePriQueue<Table*> availTables;
	TablePriQueue<Table*> busySharable;
	TablePriQueue<Table*> busyNoShare;

	// In-Progress and Ready
	CancellablePriQueue<Order*> cookingOrders;
	LinkedQueue<Order*> readyTakeaway;
	LinkedQueue<Order*> readyDineIn;
	CancellableQueue<Order*> readyDelivery;

	// In-Service, Maintenance, and Finished
	priQueue<Order*> inServiceOrders;
	LinkedQueue<Scooter*> inMaintenanceScooters;
	priQueue<Scooter*> scootersBack;

	LinkedQueue<Order*> cancelledOrders;
	LinkedStack<Order*> finishedOrders;

public:
	Restaurant();
	~Restaurant();

	void RunSimulation();
	bool LoadFromFile(string filename);

	void ExecuteEvents();
	void AddToPending(Order* pOrd);
	void CancelOVC(int id);
};

#endif