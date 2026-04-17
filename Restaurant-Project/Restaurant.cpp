#include "Restaurant.h"
#include <stdlib.h>
#include <time.h>

Restaurant::Restaurant() {
	pUI = new UI();
	currentTimestep = 1;
}

Restaurant::~Restaurant() {
	delete pUI;

	while (!actionList.isEmpty()) {
		Action* pAct = actionList.peekFront();
		actionList.dequeue();
		delete pAct;
	}
	while (!pendTakeaway.isEmpty()) {
		Order* pOrd = pendTakeaway.peekFront();
		pendTakeaway.dequeue();
		delete pOrd;
	}

	while (!pendODN.isEmpty()) {
		Order* pOrd = pendODN.peekFront();
		pendODN.dequeue();
		delete pOrd;
	}

	while (!pendODG.isEmpty()) {
		Order* pOrd = pendODG.peekFront();
		pendODG.dequeue();
		delete pOrd;
	}

	while (!pendOVC.isEmpty()) {
		Order* pOrd = pendOVC.peekFront();
		pendOVC.dequeue();
		delete pOrd;
	}

	while (!pendOVN.isEmpty()) {
		Order* pOrd = pendOVN.peekFront();
		pendOVN.dequeue();
		delete pOrd;
	}

	while (!readyTakeaway.isEmpty()) {
		Order* pOrd = readyTakeaway.peekFront();
		readyTakeaway.dequeue();
		delete pOrd;
	}

	while (!readyDineIn.isEmpty()) {
		Order* pOrd = readyDineIn.peekFront();
		readyDineIn.dequeue();
		delete pOrd;
	}

	while (!readyDelivery.isEmpty()) {
		Order* pOrd = readyDelivery.peekFront();
		readyDelivery.dequeue();
		delete pOrd;
	}

	while (!cancelledOrders.isEmpty()) {
		Order* pOrd = cancelledOrders.peekFront();
		cancelledOrders.dequeue();
		delete pOrd;
	}

	Order* pOrd;
	double pri;
	while (pendOVG.dequeue(pOrd, pri)) delete pOrd;
	while (cookingOrders.dequeue(pOrd, pri)) delete pOrd;
	while (inServiceOrders.dequeue(pOrd, pri)) delete pOrd;

	while (!finishedOrders.isEmpty()) {
		Order* o = finishedOrders.top();
		finishedOrders.pop();
		delete o;
	}

	while (!availCS.isEmpty()) {
		Chef* pChef = availCS.peekFront();
		availCS.dequeue();
		delete pChef;
	}
	while (!availCN.isEmpty()) {
		Chef* pChef = availCN.peekFront();
		availCN.dequeue();
		delete pChef;
	}

	Scooter* pScooter;
	while (availScooters.dequeue(pScooter, pri)) delete pScooter;
	while (scootersBack.dequeue(pScooter, pri)) delete pScooter;
	while (!inMaintenanceScooters.isEmpty()) {
		pScooter = inMaintenanceScooters.peekFront();
		inMaintenanceScooters.dequeue();
		delete pScooter;
	}

	Table* pTable;
	while (availTables.dequeue(pTable, pri)) delete pTable;
	while (busySharable.dequeue(pTable, pri)) delete pTable;
	while (busyNoShare.dequeue(pTable, pri)) delete pTable;
}

bool Restaurant::IsSimulationComplete() const {
	return actionList.isEmpty() && pendTakeaway.isEmpty() && pendODN.isEmpty() &&
		pendODG.isEmpty() && pendOVC.isEmpty() && pendOVG.isEmpty() &&
		pendOVN.isEmpty() && cookingOrders.isEmpty() && readyTakeaway.isEmpty() &&
		readyDineIn.isEmpty() && readyDelivery.isEmpty() && inServiceOrders.isEmpty();
}

void Restaurant::RandomSimulation() {
	srand(time(NULL));
	int totalOrdersGenerated = 499;
	while (totalOrdersGenerated < 500) {
		cout << "\nEnter total number of orders (must be > 500): ";
		cin >> totalOrdersGenerated;
		if (totalOrdersGenerated < 500) cout << "\n Orders must be at least 500";
	}
	pUI->clearScreen();
	pUI->printMessage("Starting Random Simulation (Phase 1)...");

	for (int i = 1; i <= 10; i++) availCS.enqueue(new CS(i, 5));
	for (int i = 11; i <= 20; i++) availCN.enqueue(new CN(i, 4));
	for (int i = 1; i <= 10; i++) availScooters.enqueue(new Scooter(i, 30, 1, 10, 5), 30);
	for (int i = 1; i <= 10; i++) availTables.enqueue(new Table(i, 6), -6);

	for (int i = 1; i <= totalOrdersGenerated; i++) {
		int typeRoll = rand() % 6;
		Order* pOrd = nullptr;

		int size = rand() % 10 + 1;
		double money = rand() % 200 + 50;
		int seats = rand() % 4 + 1;
		int duration = rand() % 15 + 5;
		double distance = rand() % 500 + 100;

		if (typeRoll == 0) pOrd = new ODGOrder(i, 1, size, money, seats, duration, true);
		else if (typeRoll == 1) pOrd = new ODNOrder(i, 1, size, money, seats, duration, false);
		else if (typeRoll == 2) pOrd = new TakeawayOrder(i, 1, size, money);
		else if (typeRoll == 3) pOrd = new OVC(i, 1, size, money, distance);
		else if (typeRoll == 4) pOrd = new OVG(i, 1, size, money, distance);
		else if (typeRoll == 5) pOrd = new OVN(i, 1, size, money, distance);

		AddToPending(pOrd);
	}

	currentTimestep = 1;

	while (finishedOrders.getCount() + cancelledOrders.getCount() < totalOrdersGenerated) {

		for (int i = 0; i < 30; i++) {
			Order* pOrd = nullptr;
			int listChoice = rand() % 6;

			if (listChoice == 0 && !pendTakeaway.isEmpty()) { pOrd = pendTakeaway.peekFront(); pendTakeaway.dequeue(); }
			else if (listChoice == 1 && !pendODN.isEmpty()) { pOrd = pendODN.peekFront(); pendODN.dequeue(); }
			else if (listChoice == 2 && !pendODG.isEmpty()) { pOrd = pendODG.peekFront(); pendODG.dequeue(); }
			else if (listChoice == 3 && !pendOVC.isEmpty()) { pOrd = pendOVC.peekFront(); pendOVC.dequeue(); }
			else if (listChoice == 4 && !pendOVG.isEmpty()) { double pri; pendOVG.dequeue(pOrd, pri); }
			else if (listChoice == 5 && !pendOVN.isEmpty()) { pOrd = pendOVN.peekFront(); pendOVN.dequeue(); }

			if (pOrd) {
				Chef* pChef = nullptr;
				if (rand() % 2 == 0 && !availCS.isEmpty()) { pChef = availCS.peekFront(); availCS.dequeue(); }
				else if (!availCN.isEmpty()) { pChef = availCN.peekFront(); availCN.dequeue(); }
				if (pChef) availCS.enqueue(pChef);

				int speed = pChef ? pChef->getSpeed() : 1;
				double expectedTR = currentTimestep + ((double)pOrd->getSize() / speed);
				cookingOrders.enqueue(pOrd, -expectedTR);
			}
		}

		if ((rand() % 100) < 75) {
			for (int i = 0; i < 15; i++) {
				Order* pOrd = nullptr;
				double pri;
				if (!cookingOrders.isEmpty()) {
					cookingOrders.dequeue(pOrd, pri);
					string type = pOrd->GetType();
					if (type == "OT") readyTakeaway.enqueue(pOrd);
					else if (type == "ODG" || type == "ODN") readyDineIn.enqueue(pOrd);
					else readyDelivery.enqueue(pOrd);
				}
			}
		}

		for (int i = 0; i < 10; i++) {
			Order* pOrd = nullptr;
			int listChoice = rand() % 3;

			if (listChoice == 0 && !readyTakeaway.isEmpty()) { pOrd = readyTakeaway.peekFront();  readyTakeaway.dequeue(); }
			else if (listChoice == 1 && !readyDineIn.isEmpty()) { pOrd = readyDineIn.peekFront();  readyDineIn.dequeue(); }
			else if (listChoice == 2 && !readyDelivery.isEmpty()) { pOrd = readyDelivery.peekFront();  readyDelivery.dequeue(); }

			if (pOrd) {
				string type = pOrd->GetType();
				if (type == "OT") {
					finishedOrders.push(pOrd);
				}
				else if (type == "OVC" || type == "OVG" || type == "OVN") {
					Scooter* pScooter = nullptr;
					double pri;
					double expectedTF = currentTimestep;
					if (!availScooters.isEmpty()) {
						availScooters.dequeue(pScooter, pri);
						dynamic_cast<DeliveryOrder*>(pOrd)->setScooter(pScooter);

						double dist = dynamic_cast<DeliveryOrder*>(pOrd)->getDistance();
						int spd = pScooter->getSpeed() > 0 ? pScooter->getSpeed() : 1;
						expectedTF += (dist / spd);
					}
					inServiceOrders.enqueue(pOrd, -expectedTF);
				}
				else if (type == "ODG" || type == "ODN") {
					Table* pTable = nullptr;
					double pri;
					if (!availTables.isEmpty()) {
						availTables.dequeue(pTable, pri);

						DineInOrder* dOrd = dynamic_cast<DineInOrder*>(pOrd);
						int freeSeats = pTable->getCapacity() - dOrd->getNSeats();
						if (freeSeats < 0) freeSeats = 0;

						if (dOrd->getShare() && freeSeats > 0) {
							busySharable.enqueue(pTable, freeSeats);
						}
						else {
							busyNoShare.enqueue(pTable, 1);
						}
					}
					double expectedTF = currentTimestep + dynamic_cast<DineInOrder*>(pOrd)->getDuration();
					inServiceOrders.enqueue(pOrd, -expectedTF);
				}
			}
		}

		int cancelId1 = (rand() % totalOrdersGenerated) + 1;
		Order* pCancelled1 = nullptr;
		if (pendOVC.CancelOrder(cancelId1, pCancelled1)) cancelledOrders.enqueue(pCancelled1);

		int cancelId2 = (rand() % totalOrdersGenerated) + 1;
		Order* pCancelled2 = nullptr;
		if (readyDelivery.CancelOrder(cancelId2, pCancelled2)) cancelledOrders.enqueue(pCancelled2);

		int cancelId3 = (rand() % totalOrdersGenerated) + 1;
		Order* pCancelled3 = nullptr;
		if (cookingOrders.CancelOrder(cancelId3, pCancelled3)) cancelledOrders.enqueue(pCancelled3);

		if ((rand() % 100) < 25) {
			Order* pOrd = nullptr;
			double pri;
			if (!inServiceOrders.isEmpty()) {
				inServiceOrders.dequeue(pOrd, pri);
				finishedOrders.push(pOrd);

				string type = pOrd->GetType();
				if (type == "OVC" || type == "OVG" || type == "OVN") {
					Scooter* pScooter = dynamic_cast<DeliveryOrder*>(pOrd)->getScooter();
					if (pScooter) {
						double dist = dynamic_cast<DeliveryOrder*>(pOrd)->getDistance();
						int spd = pScooter->getSpeed() > 0 ? pScooter->getSpeed() : 1;
						double returnTime = currentTimestep + (dist / spd);
						scootersBack.enqueue(pScooter, -returnTime);
					}
				}
				else if (type == "ODG" || type == "ODN") {
					Table* t = nullptr;
					double tpri;
					if (!busySharable.isEmpty() && rand() % 2 == 0) {
						busySharable.dequeue(t, tpri);
						availTables.enqueue(t, -(t->getCapacity()));
					}
					else if (!busyNoShare.isEmpty()) {
						busyNoShare.dequeue(t, tpri);
						availTables.enqueue(t, -(t->getCapacity()));
					}
					else if (!busySharable.isEmpty()) {
						busySharable.dequeue(t, tpri);
						availTables.enqueue(t, -(t->getCapacity()));
					}
				}
			}
		}

		if ((rand() % 100) < 50) {
			Scooter* pScooter = nullptr;
			double pri;
			if (!scootersBack.isEmpty()) {
				scootersBack.dequeue(pScooter, pri);
				if (rand() % 2 == 0) availScooters.enqueue(pScooter, pScooter->getSpeed());
				else inMaintenanceScooters.enqueue(pScooter);
			}
		}

		if ((rand() % 100) < 50) {
			Scooter* pScooter = nullptr;
			if (!inMaintenanceScooters.isEmpty()) {
				pScooter = inMaintenanceScooters.peekFront();
				inMaintenanceScooters.dequeue();
				availScooters.enqueue(pScooter, pScooter->getSpeed());
			}
		}


		currentTimestep++;
	}
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

	pUI->clearScreen();

	pUI->printMessage("Phase 1 Random Simulation Finished! All " + to_string(totalOrdersGenerated) + " Orders Processed.");
	pUI->printMessage("\nSimulation ends\n");
}


void Restaurant::RunSimulation() {
	pUI->clearScreen();
	SimulationMode mode = pUI->readSimulationMode();
	if (mode == RANDOM) {
		this->RandomSimulation();
		return;
	}

	bool fileLoaded = false;

	while (!fileLoaded) {
		pUI->clearScreen();
		pUI->printMessage("-----------------------------------Restaurant Simulation-----------------------------------");

		string inputFile = "../" + pUI->readInputFileName();
		fileLoaded = LoadFromFile(inputFile);

		if (!fileLoaded) {
			pUI->printMessage("Error: Could not open the file. Please check the name and try again.");
			pUI->waitForUser();
		}
	}

	string outputFile = "../" + pUI->readOutputFileName();

	if (mode == SILENT) {
		pUI->clearScreen();
		pUI->printMessage("Simulation Starts in Silent mode ...\n");
	}

	while (!IsSimulationComplete()) {
		ExecuteEvents();
		ReturnScooters();
		FinishOrders();
		ServeOrders();
		FinishCooking();
		AssignOrders();

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

	this->SaveOutput(outputFile);
	pUI->printMessage("\n\nSimulation ends, Output file created\n");
	pUI->printMessage("Output saved to " + outputFile.substr(3));
}

void Restaurant::ExecuteEvents() {
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

	if (pendOVC.CancelOrder(id, pCancelledOrder)) {
		pCancelledOrder->setStatus(CANCELLED);
		cancelledOrders.enqueue(pCancelledOrder);
		return;
	}

	if (cookingOrders.CancelOrder(id, pCancelledOrder)) {
		Chef* assignedChef = pCancelledOrder->getAssignedChef();
		if (assignedChef) {
			assignedChef->setAvailable();
			if (dynamic_cast<CS*>(assignedChef)) {
				availCS.enqueue(assignedChef);
			}
			else if (dynamic_cast<CN*>(assignedChef)) {
				availCN.enqueue(assignedChef);
			}
		}
		pCancelledOrder->setStatus(CANCELLED);
		cancelledOrders.enqueue(pCancelledOrder);
		return;
	}

	if (readyDelivery.CancelOrder(id, pCancelledOrder)) {
		pCancelledOrder->setStatus(CANCELLED);
		cancelledOrders.enqueue(pCancelledOrder);
		return;
	}
}

void Restaurant::ExtractAndMoveTable(Table* pTable, bool isShared) {
	TablePriQueue<Table*>* targetQueue = isShared ? &busySharable : &busyNoShare;
	TablePriQueue<Table*> tempQueue;
	Table* t;
	double pri;

	while (!targetQueue->isEmpty()) {
		targetQueue->dequeue(t, pri);
		if (t != pTable) {
			tempQueue.enqueue(t, pri);
		}
	}

	while (!tempQueue.isEmpty()) {
		tempQueue.dequeue(t, pri);
		targetQueue->enqueue(t, pri);
	}

	if (pTable->getFreeSeats() == pTable->getCapacity()) {
		pTable->setShared(false);
		availTables.enqueue(pTable, -(pTable->getCapacity()));
	}
	else {
		busySharable.enqueue(pTable, pTable->getFreeSeats());
	}
}

void Restaurant::AssignOrders() {
	Order* pOrd;
	Chef* pChef;
	double pri;

	while (!pendOVG.isEmpty()) {
		if (!availCS.isEmpty()) {
			pChef = availCS.peekFront();
			availCS.dequeue();
		}
		else if (!availCN.isEmpty()) {
			pChef = availCN.peekFront();
			availCN.dequeue();
		}
		else break;

		pendOVG.dequeue(pOrd, pri);
		pChef->assignOrder(pOrd, currentTimestep);
		pOrd->setStatus(COOKING);
		cookingOrders.enqueue(pOrd, -(pOrd->getTR()));
	}

	while (!pendOVC.isEmpty()) {
		if (!availCS.isEmpty()) {
			pChef = availCS.peekFront();
			availCS.dequeue();
		}
		else if (!availCN.isEmpty()) {
			pChef = availCN.peekFront();
			availCN.dequeue();
		}
		else break;

		pOrd = pendOVC.peekFront();
		pendOVC.dequeue();

		pChef->assignOrder(pOrd, currentTimestep);
		pOrd->setStatus(COOKING);
		cookingOrders.enqueue(pOrd, -(pOrd->getTR()));
	}

	while (!pendODG.isEmpty()) {
		if (!availCS.isEmpty()) {
			pChef = availCS.peekFront();
			availCS.dequeue();
		}
		else if (!availCN.isEmpty()) {
			pChef = availCN.peekFront();
			availCN.dequeue();
		}
		else break;

		pOrd = pendODG.peekFront();
		pendODG.dequeue();

		pChef->assignOrder(pOrd, currentTimestep);
		pOrd->setStatus(COOKING);
		cookingOrders.enqueue(pOrd, -(pOrd->getTR()));
	}

	while (!pendOVN.isEmpty()) {
		if (!availCN.isEmpty()) {
			pChef = availCN.peekFront();
			availCN.dequeue();
		}
		else if (!availCS.isEmpty()) {
			pChef = availCS.peekFront();
			availCS.dequeue();
		}
		else break;

		pOrd = pendOVN.peekFront();
		pendOVN.dequeue();

		pChef->assignOrder(pOrd, currentTimestep);
		pOrd->setStatus(COOKING);
		cookingOrders.enqueue(pOrd, -(pOrd->getTR()));
	}

	while (!pendODN.isEmpty()) {
		if (!availCN.isEmpty()) {
			pChef = availCN.peekFront();
			availCN.dequeue();
		}
		else if (!availCS.isEmpty()) {
			pChef = availCS.peekFront();
			availCS.dequeue();
		}
		else break;

		pOrd = pendODN.peekFront();
		pendODN.dequeue();

		pChef->assignOrder(pOrd, currentTimestep);
		pOrd->setStatus(COOKING);
		cookingOrders.enqueue(pOrd, -(pOrd->getTR()));
	}

	while (!pendTakeaway.isEmpty()) {
		if (!availCN.isEmpty()) {
			pChef = availCN.peekFront();
			availCN.dequeue();
		}
		else if (!availCS.isEmpty()) {
			pChef = availCS.peekFront();
			availCS.dequeue();
		}
		else break;

		pOrd = pendTakeaway.peekFront();
		pendTakeaway.dequeue();

		pChef->assignOrder(pOrd, currentTimestep);
		pOrd->setStatus(COOKING);
		cookingOrders.enqueue(pOrd, -(pOrd->getTR()));
	}
}

void Restaurant::FinishCooking() {
	Order* pOrd;
	double pri;

	while (!cookingOrders.isEmpty()) {
		pOrd = cookingOrders.peekFront();
		if (currentTimestep >= pOrd->getTR()) {
			cookingOrders.dequeue(pOrd, pri);
			pOrd->setStatus(READY);

			Chef* pChef = pOrd->getAssignedChef();
			pChef->setAvailable();

			if (dynamic_cast<CS*>(pChef)) {
				availCS.enqueue(pChef);
			}
			else if (dynamic_cast<CN*>(pChef)) {
				availCN.enqueue(pChef);
			}

			string type = pOrd->GetType();
			if (type == "OT") readyTakeaway.enqueue(pOrd);
			else if (type == "ODG" || type == "ODN") readyDineIn.enqueue(pOrd);
			else readyDelivery.enqueue(pOrd);
		}
		else {
			break;
		}
	}
}

void Restaurant::ServeOrders() {
	Order* pOrd;
	Scooter* pScooter;
	Table* pTable;
	double pri;

	while (!readyTakeaway.isEmpty()) {
		pOrd = readyTakeaway.peekFront();
		readyTakeaway.dequeue();

		pOrd->setTS(currentTimestep);
		pOrd->setTserv(0);
		pOrd->setStatus(FINISHED);
		finishedOrders.push(pOrd);
	}

	while (!readyDelivery.isEmpty() && !availScooters.isEmpty()) {
		pOrd = readyDelivery.peekFront();
		readyDelivery.dequeue();

		availScooters.dequeue(pScooter, pri);

		pScooter->deliverOrder(pOrd, currentTimestep);
		dynamic_cast<DeliveryOrder*>(pOrd)->setScooter(pScooter);

		pOrd->setStatus(IN_SERVICE);
		inServiceOrders.enqueue(pOrd, -(pOrd->getTF()));
	}

	while (!readyDineIn.isEmpty()) {
		pOrd = readyDineIn.peekFront();
		DineInOrder* dineOrd = dynamic_cast<DineInOrder*>(pOrd);
		int reqSeats = dineOrd->getNSeats();
		bool wantsShare = dineOrd->getShare();

		bool tableFound = false;

		if (wantsShare) {
			if (busySharable.GetBest(reqSeats, pTable)) {
				tableFound = true;
				pTable->reserveTable(dineOrd, reqSeats, wantsShare);
				dineOrd->setTable(pTable);
				busySharable.enqueue(pTable, pTable->getFreeSeats());
			}
		}

		if (!tableFound) {
			if (availTables.GetBest(reqSeats, pTable)) {
				tableFound = true;
				pTable->reserveTable(dineOrd, reqSeats, wantsShare);
				dineOrd->setTable(pTable);

				if (wantsShare) busySharable.enqueue(pTable, pTable->getFreeSeats());
				else busyNoShare.enqueue(pTable, pTable->getFreeSeats());
			}
		}

		if (tableFound) {
			readyDineIn.dequeue();
			pOrd->setTS(currentTimestep);
			pOrd->setTserv(dineOrd->getDuration());
			pOrd->setStatus(IN_SERVICE);
			inServiceOrders.enqueue(pOrd, -(pOrd->getTF()));
		}
		else {
			break;
		}
	}
}

void Restaurant::FinishOrders() {
	Order* pOrd;
	double pri;

	while (!inServiceOrders.isEmpty()) {
		pOrd = inServiceOrders.peekFront();

		if (currentTimestep >= pOrd->getTF()) {
			inServiceOrders.dequeue(pOrd, pri);
			pOrd->setStatus(FINISHED);
			finishedOrders.push(pOrd);

			string type = pOrd->GetType();

			if (type == "OVC" || type == "OVG" || type == "OVN") {
				Scooter* pScooter = dynamic_cast<DeliveryOrder*>(pOrd)->getScooter();
				if (pScooter) {
					if (pScooter->getInMaintenance()) {
						inMaintenanceScooters.enqueue(pScooter);
					}
					else {
						scootersBack.enqueue(pScooter, -(pScooter->getBackTime()));
					}
				}
			}
			else if (type == "ODG" || type == "ODN") {
				DineInOrder* dineOrd = dynamic_cast<DineInOrder*>(pOrd);
				Table* pTable = dineOrd->getTable();
				if (pTable) {
					bool wasShared = pTable->getShared();
					pTable->finishOrder(dineOrd);
					ExtractAndMoveTable(pTable, wasShared);
				}
			}
		}
		else {
			break;
		}
	}
}

void Restaurant::ReturnScooters() {
	Scooter* s;
	double pri;

	while (!scootersBack.isEmpty()) {
		s = scootersBack.peekFront();
		if (currentTimestep >= s->getBackTime()) {
			scootersBack.dequeue(s, pri);
			availScooters.enqueue(s, s->getSpeed());
		}
		else {
			break;
		}
	}

	int maintCount = inMaintenanceScooters.getCount();
	for (int i = 0; i < maintCount; i++) {
		s = inMaintenanceScooters.peekFront();
		inMaintenanceScooters.dequeue();

		if (currentTimestep >= s->getBackTime()) {
			s->setMaintenance(false);
			availScooters.enqueue(s, s->getSpeed());
		}
		else {
			inMaintenanceScooters.enqueue(s);
		}
	}
}

bool Restaurant::LoadFromFile(string filename) {
	ifstream inFile(filename);

	if (!inFile.is_open()) {
		return false;
	}

	int numCS, numCN, speedCS, speedCN;
	inFile >> numCS >> numCN >> speedCS >> speedCN;

	int chefID = 1;
	for (int i = 0; i < numCS; i++) {
		availCS.enqueue(new CS(chefID++, speedCS));
	}
	for (int i = 0; i < numCN; i++) {
		availCN.enqueue(new CN(chefID++, speedCN));
	}

	int numScooters, speedScooter, mOrds, mDur;
	inFile >> numScooters >> speedScooter >> mOrds >> mDur;

	for (int i = 0; i < numScooters; i++) {
		availScooters.enqueue(new Scooter(i + 1, speedScooter, 1, mOrds, mDur), 0);
	}

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

void Restaurant::SaveOutput(string filename) {
	ofstream outFile(filename);
	if (!outFile.is_open()) {
		pUI->printMessage("Error: Could not create output file.");
		return;
	}

	outFile << "TF\tID\tTQ\tTA\tTR\tTS\tTi\tTC\tTW\tTserv\n";

	int totalOrders = 0, totalODG = 0, totalODN = 0, totalOT = 0;
	int totalOVC = 0, totalOVG = 0, totalOVN = 0;
	int totalFinished = 0, totalCancelled = 0;
	double sumTi = 0, sumTC = 0, sumTw = 0, sumTserv = 0;

	LinkedStack<Order*> tempStack;
	Order* pOrd;

	while (!finishedOrders.isEmpty()) {
		pOrd = finishedOrders.top();
		finishedOrders.pop();
		tempStack.push(pOrd);

		string type = pOrd->GetType();
		if (type == "ODG") totalODG++;
		else if (type == "ODN") totalODN++;
		else if (type == "OT") totalOT++;
		else if (type == "OVC") totalOVC++;
		else if (type == "OVG") totalOVG++;
		else if (type == "OVN") totalOVN++;

		totalFinished++;
		sumTi += pOrd->getTI();
		sumTC += pOrd->getTC();
		sumTw += pOrd->getTW();
		sumTserv += pOrd->getTserv();

		outFile << pOrd->getTF() << "\t"
			<< pOrd->getID() << "\t"
			<< pOrd->getTQ() << "\t"
			<< pOrd->getTA() << "\t"
			<< pOrd->getTR() << "\t"
			<< pOrd->getTS() << "\t"
			<< pOrd->getTI() << "\t"
			<< pOrd->getTC() << "\t"
			<< pOrd->getTW() << "\t"
			<< pOrd->getTserv() << "\n";
	}

	while (!tempStack.isEmpty()) {
		pOrd = tempStack.top();
		tempStack.pop();
		finishedOrders.push(pOrd);
	}

	LinkedQueue<Order*> tempQueue;
	while (!cancelledOrders.isEmpty()) {
		pOrd = cancelledOrders.peekFront();
		cancelledOrders.dequeue();
		tempQueue.enqueue(pOrd);

		string type = pOrd->GetType();
		if (type == "ODG") totalODG++;
		else if (type == "ODN") totalODN++;
		else if (type == "OT") totalOT++;
		else if (type == "OVC") totalOVC++;
		else if (type == "OVG") totalOVG++;
		else if (type == "OVN") totalOVN++;

		totalCancelled++;
	}

	while (!tempQueue.isEmpty()) {
		pOrd = tempQueue.peekFront();
		tempQueue.dequeue();
		cancelledOrders.enqueue(pOrd);
	}

	totalOrders = totalFinished + totalCancelled;

	int totalCS = availCS.getCount();
	int totalCN = availCN.getCount();
	int totalChefs = totalCS + totalCN;

	double chefBusyTime = 0;
	Chef* pChef;

	for (int i = 0; i < totalCS; i++) {
		pChef = availCS.peekFront();
		availCS.dequeue();
		chefBusyTime += pChef->getTotalBusyTime();
		availCS.enqueue(pChef);
	}
	for (int i = 0; i < totalCN; i++) {
		pChef = availCN.peekFront();
		availCN.dequeue();
		chefBusyTime += pChef->getTotalBusyTime();
		availCN.enqueue(pChef);
	}

	int totalScooters = availScooters.getCount();
	double scooterBusyTime = 0;
	Scooter* pScooter;
	double pri;
	priQueue<Scooter*> tempPriQ;

	while (availScooters.dequeue(pScooter, pri)) {
		scooterBusyTime += pScooter->getTotalBusyTime();
		tempPriQ.enqueue(pScooter, pri);
	}
	while (tempPriQ.dequeue(pScooter, pri)) {
		availScooters.enqueue(pScooter, pri);
	}

	double avgTi = totalFinished > 0 ? sumTi / totalFinished : 0;
	double avgTC = totalFinished > 0 ? sumTC / totalFinished : 0;
	double avgTw = totalFinished > 0 ? sumTw / totalFinished : 0;
	double avgTserv = totalFinished > 0 ? sumTserv / totalFinished : 0;

	double pctFinished = totalOrders > 0 ? (totalFinished * 100.0) / totalOrders : 0;
	double pctCancelled = totalOrders > 0 ? (totalCancelled * 100.0) / totalOrders : 0;
	double pctOverwait = 0.0;

	int actualSimTime = currentTimestep > 1 ? currentTimestep - 1 : 1;

	double chefUtil = totalChefs > 0 ? (chefBusyTime / (actualSimTime * totalChefs)) * 100.0 : 0;
	double scooterUtil = totalScooters > 0 ? (scooterBusyTime / (actualSimTime * totalScooters)) * 100.0 : 0;

	outFile << "\n\n";
	outFile << "Total Orders: " << totalOrders << " [ODG: " << totalODG << ", ODN: " << totalODN << ", OT: " << totalOT
		<< ", OVC: " << totalOVC << ", OVG: " << totalOVG << ", OVN: " << totalOVN << "]\n";
	outFile << "Total Chefs: " << totalChefs << " [CS: " << totalCS << ", CN: " << totalCN << "]\n";
	outFile << "Total Scooters: " << totalScooters << "\n";
	outFile << "Finished Orders: " << pctFinished << "%, Cancelled Orders: " << pctCancelled << "%\n";
	outFile << "Overwait Orders: " << pctOverwait << "%\n";
	outFile << "Averages for Finished Orders => Ti: " << avgTi << ", TC: " << avgTC << ", Tw: " << avgTw << ", Tserv: " << avgTserv << "\n";
	outFile << "Scooters Utilization: " << scooterUtil << "%\n";
	outFile << "Chefs Utilization: " << chefUtil << "%\n";

	outFile.close();
}