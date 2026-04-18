#ifndef __RESTAURANT_H_
#define __RESTAURANT_H_

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <time.h>

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

using std::string;

class Restaurant {
private:
	UI* pUI;
	int currentTimestep;
	LinkedQueue<Action*> actionList;

	//Pending Orders
	LinkedQueue<Order*> pendTakeaway;    
	LinkedQueue<Order*> pendODN;         
	LinkedQueue<Order*> pendODG;    
	CancellableQueue<Order*> pendOVC;
	priQueue<Order*> pendOVG;     
	LinkedQueue<Order*> pendOVN;       

	//Free Resources
	LinkedQueue<Chef*> availCS;
	LinkedQueue<Chef*> availCN;
	priQueue<Scooter*> availScooters;

	TablePriQueue<Table*> availTables;
	TablePriQueue<Table*> busySharable;
	TablePriQueue<Table*> busyNoShare;

	//In-Progress & Ready
	CancellablePriQueue<Order*> cookingOrders;

	LinkedQueue<Order*> readyTakeaway;
	LinkedQueue<Order*> readyDineIn; 

	
	CancellableQueue<Order*> readyOVC;
	LinkedQueue<Order*> readyOVG;
	LinkedQueue<Order*> readyOVN;

	
	priQueue<Order*> inServiceOrders;

	priQueue<Scooter*> scootersBack;
	LinkedQueue<Scooter*> inMaintenanceScooters;

	LinkedQueue<Order*> cancelledOrders;
	LinkedStack<Order*> finishedOrders;

public:
	Restaurant();
	~Restaurant();

	void RandomSimulation();
	void RunSimulation();
	bool LoadFromFile(string filename);
	void SaveOutput(string filename);

	void ExecuteEvents();
	void AddToPending(Order* pOrd);
	void CancelOVC(int id);

	// manage functions
	void AssignOrders();
	void FinishCooking();
	void ServeOrders();
	void FinishOrders();
	void ReturnScooters();

	void ExtractAndMoveTable(Table* pTable, bool isShared);
	bool IsSimulationComplete() const;
	void ClearUp();
};

#endif