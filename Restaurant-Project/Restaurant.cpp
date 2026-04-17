#include "Restaurant.h"

Restaurant::Restaurant() {
	pUI = new UI();
	currentTimestep = 1;
}

Restaurant::~Restaurant() {
	delete pUI;
}

void Restaurant::RunSimulation() {
	bool fileLoaded = false;

	// Keep prompting and clearing screen until a valid file is loaded
	while (!fileLoaded) {
		pUI->clearScreen();
		pUI->printMessage("-----------------------------------Restaurant Simulation-----------------------------------");

		string inputFile = pUI->readInputFileName();
		fileLoaded = LoadFromFile(inputFile);

		if (!fileLoaded) {
			pUI->printMessage("Error: Could not open the file. Please check the name and try again.");
			pUI->waitForUser();
		}
	}

	string outputFile = pUI->readOutputFileName();
	SimulationMode mode = pUI->readSimulationMode();

	if (mode == SILENT) {
		pUI->clearScreen();
		pUI->printMessage("Simulation Starts in Silent mode ...");
	}
	// Main Simulation Loop
	while (!actionList.isEmpty()) {

		ExecuteEvents();

		if (mode == INTERACTIVE) {
			pUI->clearScreen();
			pUI->printTimestep(currentTimestep);

			pUI->printSystemStatus(
				&actionList,
				&pendTakeaway, &pendODN, &pendODG,
				&pendOVC, &pendOVG, &pendOVN,
				&availCS, &availCN,
				&cookingOrders,
				&readyTakeaway, &readyDineIn, &readyDelivery,
				&availScooters,
				&availTables, &busySharable, &busyNoShare,
				&inServiceOrders,
				&inMaintenanceScooters,
				&scootersBack,
				&cancelledOrders,
				&finishedOrders
			);

			pUI->waitForUser();
		}

		currentTimestep++;
	}
	pUI->printMessage("Simulation ends, Output file created");
	pUI->printMessage("Output saved to " + outputFile);
}

void Restaurant::ExecuteEvents() {
	// Process all actions that belong to the current timestep
	while (!actionList.isEmpty()) {
		Action* pAct = actionList.peekFront();

		if (pAct && pAct->getTimestep() == currentTimestep) {
			actionList.dequeue();
			pAct->Execute(this);
			delete pAct;
		}
		else {
			break;
		}
	}
}

void Restaurant::AddToPending(Order* pOrd) {
	string type = pOrd->GetType();

	// Route the new order to the correct pending list
	if (type == "OT") pendTakeaway.enqueue(pOrd);
	else if (type == "ODN") pendODN.enqueue(pOrd);
	else if (type == "ODG") pendODG.enqueue(pOrd);
	else if (type == "OVC") pendOVC.enqueue(pOrd);
	else if (type == "OVN") pendOVN.enqueue(pOrd);
	else if (type == "OVG") {
		pendOVG.enqueue(pOrd, pOrd->getPriority());
	}
}

void Restaurant::CancelOVC(int id) {
	Order* pCancelledOrder = nullptr;

	// find ovc delete it 
	if (pendOVC.CancelOrder(id, pCancelledOrder)) {
		cancelledOrders.enqueue(pCancelledOrder);
		return;
	}

	if (cookingOrders.CancelOrder(id, pCancelledOrder)) {
		cancelledOrders.enqueue(pCancelledOrder);
		return;
	}

	if (readyDelivery.CancelOrder(id, pCancelledOrder)) {
		cancelledOrders.enqueue(pCancelledOrder);
		return;
	}
}

bool Restaurant::LoadFromFile(string filename) {
	ifstream inFile(filename);

	if (!inFile.is_open()) {
		return false;
	}

	// 1. Load Chefs
	int numCS, numCN, speedCS, speedCN;
	inFile >> numCS >> numCN >> speedCS >> speedCN;

	int chefID = 1;
	for (int i = 0; i < numCS; i++) {
		availCS.enqueue(new CS(chefID++, speedCS));
	}
	for (int i = 0; i < numCN; i++) {
		availCN.enqueue(new CN(chefID++, speedCN));
	}

	// 2. Load Scooters
	int numScooters, speedScooter, mOrds, mDur;
	inFile >> numScooters >> speedScooter >> mOrds >> mDur;

	for (int i = 0; i < numScooters; i++) {
		availScooters.enqueue(new Scooter(i + 1, speedScooter, 1, mOrds, mDur), 0);
	}

	// Load Tables
	int numTablesTotal;
	inFile >> numTablesTotal;

	int tablesCreated = 0;
	int tableID = 1;

	while (tablesCreated < numTablesTotal) {
		int count, capacity;
		inFile >> count >> capacity;

		for (int i = 0; i < count; i++) {
			availTables.enqueue(new Table(tableID++, capacity), -capacity);
			tablesCreated++;
		}
	}

	// Load Actions
	int numActions;
	inFile >> numActions;

	for (int i = 0; i < numActions; i++) {
		char actionType;
		inFile >> actionType;

		if (actionType == 'Q' || actionType == 'R') {
			string orderType;
			int TS, ID, size;
			double money;
			inFile >> orderType >> TS >> ID >> size >> money;

			if (orderType == "ODG" || orderType == "ODN") {
				int seats, duration;
				char shareChar;
				inFile >> seats >> duration >> shareChar;
				bool share = (shareChar == 'Y');
				actionList.enqueue(new RequestAction(TS, ID, orderType, size, money, seats, duration, share));
			}
			else if (orderType == "OVC" || orderType == "OVG" || orderType == "OVN") {
				double distance;
				inFile >> distance;
				actionList.enqueue(new RequestAction(TS, ID, orderType, size, money, distance));
			}
			else if (orderType == "OT") {
				actionList.enqueue(new RequestAction(TS, ID, orderType, size, money));
			}
		}
		else if (actionType == 'X') {
			int TS, ID;
			inFile >> TS >> ID;
			actionList.enqueue(new CancelAction(TS, ID));
		}
	}

	inFile.close();
	pUI->printMessage("File Loaded Successfully!");
	return true;
}