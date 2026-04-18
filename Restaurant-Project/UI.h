#ifndef _UI_H_
#define _UI_H_

#include <iostream>
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

using std::cout, std::cin, std::string, std::endl;

enum SimulationMode {
	RANDOM = 0,
	INTERACTIVE = 1,
	SILENT = 2
};

class UI {
public:
	UI() {}

	string readInputFileName();
	string readOutputFileName();
	SimulationMode readSimulationMode();
	int UIMenuRandom(int& totalOrdersGenerated);

	void printTimestep(int timestep);
	void waitForUser();
	void clearScreen();
	void printMessage(string msg);

	void printSystemStatus(
		LinkedQueue<Action*>* actionList,
		LinkedQueue<Order*>* pendTakeaway, LinkedQueue<Order*>* pendODN, LinkedQueue<Order*>* pendODG,
		CancellableQueue<Order*>* pendOVC, priQueue<Order*>* pendOVG, LinkedQueue<Order*>* pendOVN,
		LinkedQueue<Chef*>* availCS, LinkedQueue<Chef*>* availCN,
		CancellablePriQueue<Order*>* cookingOrders,
		LinkedQueue<Order*>* readyTakeaway, LinkedQueue<Order*>* readyDineIn, CancellableQueue<Order*>* readyOVC, LinkedQueue<Order*>* readyOVG, LinkedQueue<Order*>* readyOVN,
		priQueue<Scooter*>* availScooters,
		TablePriQueue<Table*>* availTables, TablePriQueue<Table*>* busySharable, TablePriQueue<Table*>* busyNoShare,
		priQueue<Order*>* inServiceOrders,
		LinkedQueue<Scooter*>* inMaintenanceScooters,
		priQueue<Scooter*>* scootersBack,
		LinkedQueue<Order*>* cancelledOrders,
		LinkedStack<Order*>* finishedOrders);

	~UI() {}
};

#endif