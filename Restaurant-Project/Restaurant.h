#ifndef __RESTAURANT_H_
#define __RESTAURANT_H_

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <format>

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

using std::string, std::format;

class Restaurant {
private:
	UI* pUI;
	int currentTimestep;
	LinkedQueue<Action*> actionList;

	bool isBonus;           
	int TH;  
	int totalOverwaitCount; 
	int failProb;           
	int totalRescues;       

	LinkedQueue<Order*> pendCombo;
	LinkedQueue<Order*> pendTakeaway;
	LinkedQueue<Order*> pendODN;
	LinkedQueue<Order*> pendODG;
	CancellableQueue<Order*> pendOVC;
	priQueue<Order*> pendOVG;
	LinkedQueue<Order*> pendOVN;

	// free Resources
	LinkedQueue<Chef*> availCS;
	LinkedQueue<Chef*> availCN;
	priQueue<Scooter*> availScooters;

	TablePriQueue<Table*> availTables;
	TablePriQueue<Table*> busySharable;
	TablePriQueue<Table*> busyNoShare;

	CancellablePriQueue<Order*> cookingOrders;

	LinkedQueue<Order*> readyCombo;
	LinkedQueue<Order*> readyTakeaway;
	LinkedQueue<Order*> readyDineIn;
	CancellableQueue<Order*> readyOVC;
	LinkedQueue<Order*> readyOVG;
	LinkedQueue<Order*> readyOVN;

	priQueue<Order*> overwaitOVG;

	priQueue<Order*> inServiceOrders;

	priQueue<Scooter*> scootersBack;
	LinkedQueue<Scooter*> inMaintenanceScooters;

	LinkedQueue<Order*> cancelledOrders;
	LinkedStack<Order*> finishedOrders;

	//helper
	string formatClean(double val, int percision);

public:
	Restaurant();
	~Restaurant();

	void RunSimulation();

	bool LoadFromFile(string filename, bool& isBonusFile);
	void SaveOutput(string filename);

	void ExecuteEvents();
	void AddToPending(Order* pOrd);
	void CancelOVC(int id);

	void CheckScootersLists();         
	void CheckFinishedDeliveryOrders();
	void CheckFinishedDineInOrders();  

	void AssignPendingToChef();        
	void MoveFromCookingToReady();     

	void AssignTable();                
	void AssignScooter();              

	void FinalizeTakeawayOrders();     
	void UpdateOverwaitOrders();       

	void ExtractAndMoveTable(Table* pTable, bool isShared);
	bool IsSimulationComplete() const;
	void ClearUp();
};

#endif