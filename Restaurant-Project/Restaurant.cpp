#include "Restaurant.h"

Restaurant::Restaurant() {
	pUI = new UI();
	currentTimestep = 1;

	// Initialize Bonus Variables safely
	isBonus = false;
	TH = 0;
	totalOverwaitCount = 0;
	failProb = 0;
	totalRescues = 0;

	// Seed random number generator for the Rescue Scooter probability
	srand((unsigned)time(NULL));
}

Restaurant::~Restaurant() {
	delete pUI;
	ClearUp();
}

void Restaurant::ClearUp() {
	while (!actionList.isEmpty()) { Action* pAct = actionList.peekFront(); actionList.dequeue(); delete pAct; }
	while (!pendCombo.isEmpty()) { Order* pOrd = pendCombo.peekFront(); pendCombo.dequeue(); delete pOrd; }
	while (!pendTakeaway.isEmpty()) { Order* pOrd = pendTakeaway.peekFront(); pendTakeaway.dequeue(); delete pOrd; }
	while (!pendODN.isEmpty()) { Order* pOrd = pendODN.peekFront(); pendODN.dequeue(); delete pOrd; }
	while (!pendODG.isEmpty()) { Order* pOrd = pendODG.peekFront(); pendODG.dequeue(); delete pOrd; }
	while (!pendOVC.isEmpty()) { Order* pOrd = pendOVC.peekFront(); pendOVC.dequeue(); delete pOrd; }
	while (!pendOVN.isEmpty()) { Order* pOrd = pendOVN.peekFront(); pendOVN.dequeue(); delete pOrd; }

	while (!readyCombo.isEmpty()) { Order* pOrd = readyCombo.peekFront(); readyCombo.dequeue(); delete pOrd; }
	while (!readyTakeaway.isEmpty()) { Order* pOrd = readyTakeaway.peekFront(); readyTakeaway.dequeue(); delete pOrd; }
	while (!readyDineIn.isEmpty()) { Order* pOrd = readyDineIn.peekFront(); readyDineIn.dequeue(); delete pOrd; }
	while (!readyOVC.isEmpty()) { Order* pOrd = readyOVC.peekFront(); readyOVC.dequeue(); delete pOrd; }
	while (!readyOVG.isEmpty()) { Order* pOrd = readyOVG.peekFront(); readyOVG.dequeue(); delete pOrd; }
	while (!readyOVN.isEmpty()) { Order* pOrd = readyOVN.peekFront(); readyOVN.dequeue(); delete pOrd; }

	while (!cancelledOrders.isEmpty()) { Order* pOrd = cancelledOrders.peekFront(); cancelledOrders.dequeue(); delete pOrd; }

	Order* pOrd; double pri;
	while (pendOVG.dequeue(pOrd, pri)) delete pOrd;
	while (overwaitOVG.dequeue(pOrd, pri)) delete pOrd;
	while (cookingOrders.dequeue(pOrd, pri)) delete pOrd;
	while (inServiceOrders.dequeue(pOrd, pri)) delete pOrd;

	while (!finishedOrders.isEmpty()) { Order* o = finishedOrders.top(); finishedOrders.pop(); delete o; }

	while (!availCS.isEmpty()) { Chef* pChef = availCS.peekFront(); availCS.dequeue(); delete pChef; }
	while (!availCN.isEmpty()) { Chef* pChef = availCN.peekFront(); availCN.dequeue(); delete pChef; }

	Scooter* pScooter;
	while (availScooters.dequeue(pScooter, pri)) delete pScooter;
	while (scootersBack.dequeue(pScooter, pri)) delete pScooter;
	while (!inMaintenanceScooters.isEmpty()) { pScooter = inMaintenanceScooters.peekFront(); inMaintenanceScooters.dequeue(); delete pScooter; }

	Table* pTable;
	while (availTables.dequeue(pTable, pri)) delete pTable;
	while (busySharable.dequeue(pTable, pri)) delete pTable;
	while (busyNoShare.dequeue(pTable, pri)) delete pTable;
}

bool Restaurant::IsSimulationComplete() const {
	return actionList.isEmpty() && pendCombo.isEmpty() && pendTakeaway.isEmpty() && pendODN.isEmpty() &&
		pendODG.isEmpty() && pendOVC.isEmpty() && pendOVG.isEmpty() && pendOVN.isEmpty() &&
		cookingOrders.isEmpty() && readyCombo.isEmpty() && readyTakeaway.isEmpty() &&
		readyDineIn.isEmpty() && readyOVC.isEmpty() && readyOVG.isEmpty() && readyOVN.isEmpty() &&
		overwaitOVG.isEmpty() && inServiceOrders.isEmpty();
}

void Restaurant::RunSimulation() {
	pUI->clearScreen();
	ClearUp();

	SimulationMode mode = pUI->readSimulationMode();

	bool fileLoaded = false;
	while (!fileLoaded) {
		pUI->clearScreen();
		pUI->printMessage("-----------------------------------Restaurant Simulation-----------------------------------");

		string inputFile = "../" + pUI->readInputFileName();
		fileLoaded = LoadFromFile(inputFile, isBonus);

		if (!fileLoaded) {
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

		if (mode == INTERACTIVE) {
			pUI->clearScreen();
			pUI->printTimestep(currentTimestep);

			pUI->printSystemStatus(
				isBonus,
				&actionList,
				&pendCombo, &pendTakeaway, &pendODN, &pendODG,
				&pendOVC, &pendOVG, &pendOVN,
				&availCS, &availCN,
				&cookingOrders,
				&readyCombo, &readyTakeaway, &readyDineIn,
				&readyOVC, &readyOVG, &readyOVN,
				&overwaitOVG,
				&availScooters,
				&availTables, &busySharable, &busyNoShare,
				&inServiceOrders,
				&inMaintenanceScooters,
				&scootersBack,
				&cancelledOrders,
				&finishedOrders
			);
		}

		CheckScootersLists();
		CheckFinishedDeliveryOrders();
		CheckFinishedDineInOrders();
		FinalizeTakeawayOrders();

		if (isBonus) UpdateOverwaitOrders();
		AssignScooter();

		AssignTable();
		MoveFromCookingToReady();
		AssignPendingToChef();

		if (mode == INTERACTIVE) pUI->waitForUser();

		currentTimestep++;
	}

	pUI->clearScreen();
	pUI->printTimestep(currentTimestep);
	pUI->printSystemStatus(
		isBonus,
		&actionList,
		&pendCombo, &pendTakeaway, &pendODN, &pendODG,
		&pendOVC, &pendOVG, &pendOVN,
		&availCS, &availCN,
		&cookingOrders,
		&readyCombo, &readyTakeaway, &readyDineIn,
		&readyOVC, &readyOVG, &readyOVN,
		&overwaitOVG,
		&availScooters,
		&availTables, &busySharable, &busyNoShare,
		&inServiceOrders,
		&inMaintenanceScooters,
		&scootersBack,
		&cancelledOrders,
		&finishedOrders
	);

	this->SaveOutput(outputFile);
	pUI->printMessage("\n\nSimulation ends, Output file created");
	pUI->printMessage("\nOutput saved to " + outputFile.substr(3));
}

void Restaurant::ExecuteEvents() {
	while (!actionList.isEmpty()) {
		Action* pAct = actionList.peekFront();

		if (pAct && pAct->getTimestep() == currentTimestep) {
			actionList.dequeue();
			pAct->Execute(this);
			delete pAct;
		}
		else break;
	}
}

void Restaurant::AddToPending(Order* pOrd) {
	string type = pOrd->GetType();

	if (type == "COMBO") pendCombo.enqueue(pOrd);
	else if (type == "OT") pendTakeaway.enqueue(pOrd);
	else if (type == "ODN") pendODN.enqueue(pOrd);
	else if (type == "ODG") pendODG.enqueue(pOrd);
	else if (type == "OVC") pendOVC.enqueue(pOrd);
	else if (type == "OVN") pendOVN.enqueue(pOrd);
	else if (type == "OVG") pendOVG.enqueue(pOrd, pOrd->getPriority());
}

void Restaurant::CancelOVC(int id) {
	Order* pCancelledOrder = nullptr;

	if (pendOVC.CancelOrder(id, pCancelledOrder)) {
		pCancelledOrder->setStatus(CANCELLED); cancelledOrders.enqueue(pCancelledOrder); return;
	}
	if (cookingOrders.CancelOrder(id, pCancelledOrder)) {
		Chef* assignedChef = pCancelledOrder->getAssignedChef();
		if (assignedChef) {
			assignedChef->deductBusyTime(pCancelledOrder->getTR() - currentTimestep);
			assignedChef->setAvailable();
			if (dynamic_cast<CS*>(assignedChef)) availCS.enqueue(assignedChef);
			else if (dynamic_cast<CN*>(assignedChef)) availCN.enqueue(assignedChef);
		}
		pCancelledOrder->setStatus(CANCELLED); cancelledOrders.enqueue(pCancelledOrder); return;
	}
	if (readyOVC.CancelOrder(id, pCancelledOrder)) {
		pCancelledOrder->setStatus(CANCELLED); cancelledOrders.enqueue(pCancelledOrder); return;
	}
}

void Restaurant::ExtractAndMoveTable(Table* pTable, bool isShared) {
	TablePriQueue<Table*>* targetQueue = isShared ? &busySharable : &busyNoShare;
	TablePriQueue<Table*> tempQueue; Table* t; double pri;

	while (!targetQueue->isEmpty()) {
		targetQueue->dequeue(t, pri);
		if (t != pTable) tempQueue.enqueue(t, pri);
	}
	while (!tempQueue.isEmpty()) {
		tempQueue.dequeue(t, pri); targetQueue->enqueue(t, pri);
	}

	if (pTable->getFreeSeats() == pTable->getCapacity()) {
		pTable->setTableFree(); availTables.enqueue(pTable, pTable->getPriority());
	}
	else {
		(isShared) ? busySharable.enqueue(pTable, pTable->getPriority()) : busyNoShare.enqueue(pTable, pTable->getPriority());
	}
}

void Restaurant::CheckScootersLists() {
	Scooter* s; double pri;

	while (!scootersBack.isEmpty()) {
		s = scootersBack.peekFront();
		if (currentTimestep >= s->getBackTime()) {
			scootersBack.dequeue(s, pri);

			if (s->getState() == InMaintainance) {
				s->setMaintenance(currentTimestep);
				inMaintenanceScooters.enqueue(s);
			}
			else {
				s->setAvailable();
				availScooters.enqueue(s, s->getPriority());
			}
		}
		else break;
	}

	int maintCount = inMaintenanceScooters.getCount();
	for (int i = 0; i < maintCount; i++) {
		s = inMaintenanceScooters.peekFront(); inMaintenanceScooters.dequeue();
		if (currentTimestep >= -(s->getPriority())) {
			s->setAvailable();
			availScooters.enqueue(s, s->getPriority());
		}
		else {
			inMaintenanceScooters.enqueue(s);
		}
	}
}


void Restaurant::CheckFinishedDeliveryOrders() {
	priQueue<Order*> temp; Order* pOrd; double pri;

	while (inServiceOrders.dequeue(pOrd, pri)) {
		if (currentTimestep >= pOrd->getTF()) {
			string type = pOrd->GetType();
			if (type == "OVC" || type == "OVG" || type == "OVN" || type == "COMBO") {
				pOrd->setStatus(FINISHED); finishedOrders.push(pOrd);

				Scooter* pScooter = dynamic_cast<DeliveryOrder*>(pOrd)->getScooter();
				if (pScooter) {
					if (pScooter->getState() != InMaintainance) pScooter->setBack();
					scootersBack.enqueue(pScooter, -(pScooter->getBackTime()));
				}

				if (type == "COMBO") {
					COMBO* cOrd = dynamic_cast<COMBO*>(pOrd);
					Scooter** extraS = cOrd->getExtraScooters();
					for (int i = 0; i < cOrd->getExtraScooterCount(); i++) {
						if (extraS[i]) {
							if (extraS[i]->getState() != InMaintainance) extraS[i]->setBack();
							scootersBack.enqueue(extraS[i], -(extraS[i]->getBackTime()));
						}
					}
				}
			}
			else temp.enqueue(pOrd, pri);      //dine in order or take away
		}
		else temp.enqueue(pOrd, pri);           //not reached the dist
	}
	while (temp.dequeue(pOrd, pri)) inServiceOrders.enqueue(pOrd, pri);
}

void Restaurant::CheckFinishedDineInOrders() {
	priQueue<Order*> temp; Order* pOrd; double pri;

	while (inServiceOrders.dequeue(pOrd, pri)) {
		if (currentTimestep >= pOrd->getTF()) {
			string type = pOrd->GetType();
			if (type == "ODG" || type == "ODN") {
				pOrd->setStatus(FINISHED); finishedOrders.push(pOrd);
				DineInOrder* dineOrd = dynamic_cast<DineInOrder*>(pOrd);
				Table* pTable = dineOrd->getTable();
				if (pTable) {
					bool wasShared = pTable->getShared();
					pTable->finishOrder(dineOrd); ExtractAndMoveTable(pTable, wasShared);
				}
			}
			else temp.enqueue(pOrd, pri);
		}
		else temp.enqueue(pOrd, pri);
	}
	while (temp.dequeue(pOrd, pri)) inServiceOrders.enqueue(pOrd, pri);
}

void Restaurant::AssignPendingToChef() {                 //COMBO ---> FCFS min TQ: ODG ---> ODN ---> OT ---> OVG (Priority Eq) ---> OVC ---> OVN
	while (true) {
		int bestType = -1;
		int minTQ = INT_MAX;

		int csCount = availCS.getCount();
		int cnCount = availCN.getCount();
		int totalChefs = csCount + cnCount;

		if (isBonus && !pendCombo.isEmpty()) {
			COMBO* cOrd = dynamic_cast<COMBO*>(pendCombo.peekFront());
			if (cOrd && csCount >= 1 && totalChefs >= cOrd->getReqChefs()) {
				if (cOrd->getTQ() < minTQ) { minTQ = cOrd->getTQ(); bestType = 0; }
			}
		}

		if (!pendODG.isEmpty() && csCount >= 1) {
			if (pendODG.peekFront()->getTQ() < minTQ) { minTQ = pendODG.peekFront()->getTQ(); bestType = 1; }
		}

		if (!pendODN.isEmpty() && totalChefs >= 1) {
			if (pendODN.peekFront()->getTQ() < minTQ) { minTQ = pendODN.peekFront()->getTQ(); bestType = 2; }
		}

		if (!pendTakeaway.isEmpty() && cnCount >= 1) {
			if (pendTakeaway.peekFront()->getTQ() < minTQ) { minTQ = pendTakeaway.peekFront()->getTQ(); bestType = 3; }
		}

		if (!pendOVG.isEmpty() && csCount >= 1) {
			Order* tempOVG; double tempPri;
			pendOVG.dequeue(tempOVG, tempPri);
			if (tempOVG->getTQ() < minTQ) { minTQ = tempOVG->getTQ(); bestType = 4; }
			pendOVG.enqueue(tempOVG, tempPri);
		}

		if (!pendOVC.isEmpty() && totalChefs >= 1) {
			if (pendOVC.peekFront()->getTQ() < minTQ) { minTQ = pendOVC.peekFront()->getTQ(); bestType = 5; }
		}

		if (!pendOVN.isEmpty() && cnCount >= 1) {
			if (pendOVN.peekFront()->getTQ() < minTQ) { minTQ = pendOVN.peekFront()->getTQ(); bestType = 6; }
		}

		if (bestType == -1) break;

		Order* pOrd = nullptr; Chef* pChef = nullptr; double pri;

		if (bestType == 0) {
			pOrd = pendCombo.peekFront(); pendCombo.dequeue();
			COMBO* cOrd = dynamic_cast<COMBO*>(pOrd);

			if (!availCS.isEmpty()) {
				pChef = availCS.peekFront(); availCS.dequeue();
			}

			int extraNeeded = cOrd->getReqChefs() - 1;
			for (int i = 0; i < extraNeeded; i++) {
				Chef* extraChef = nullptr;
				if (!availCN.isEmpty()) {
					extraChef = availCN.peekFront(); availCN.dequeue();
				}
				else if (!availCS.isEmpty()) {
					extraChef = availCS.peekFront(); availCS.dequeue();
				}

				if (extraChef != nullptr) {
					extraChef->assignOrder(pOrd, currentTimestep);
					cOrd->addExtraChef(extraChef);
				}
			}
		}
		else if (bestType == 1) {
			pOrd = pendODG.peekFront(); pendODG.dequeue();
			if (!availCS.isEmpty()) { pChef = availCS.peekFront(); availCS.dequeue(); }
		}
		else if (bestType == 2) {
			pOrd = pendODN.peekFront(); pendODN.dequeue();
			if (!availCN.isEmpty()) { pChef = availCN.peekFront(); availCN.dequeue(); }
			else if (!availCS.isEmpty()) { pChef = availCS.peekFront(); availCS.dequeue(); }
		}
		else if (bestType == 3) {
			pOrd = pendTakeaway.peekFront(); pendTakeaway.dequeue();
			if (!availCN.isEmpty()) { pChef = availCN.peekFront(); availCN.dequeue(); }
		}
		else if (bestType == 4) {
			pendOVG.dequeue(pOrd, pri);
			if (!availCS.isEmpty()) { pChef = availCS.peekFront(); availCS.dequeue(); }
		}
		else if (bestType == 5) {
			pOrd = pendOVC.peekFront(); pendOVC.dequeue();
			if (!availCN.isEmpty()) { pChef = availCN.peekFront(); availCN.dequeue(); }
			else if (!availCS.isEmpty()) { pChef = availCS.peekFront(); availCS.dequeue(); }
		}
		else if (bestType == 6) {
			pOrd = pendOVN.peekFront(); pendOVN.dequeue();
			if (!availCN.isEmpty()) { pChef = availCN.peekFront(); availCN.dequeue(); }
		}

		if (pOrd != nullptr && pChef != nullptr) {
			pChef->assignOrder(pOrd, currentTimestep);
			pOrd->setStatus(COOKING);
			cookingOrders.enqueue(pOrd, -(pOrd->getTR()));
		}
	}
}


void Restaurant::MoveFromCookingToReady() {
	Order* pOrd; double pri;

	while (!cookingOrders.isEmpty()) {
		pOrd = cookingOrders.peekFront();
		if (currentTimestep >= pOrd->getTR()) {
			cookingOrders.dequeue(pOrd, pri); pOrd->setStatus(READY);

			Chef* pChef = pOrd->getAssignedChef(); pChef->setAvailable();
			if (dynamic_cast<CS*>(pChef)) availCS.enqueue(pChef);
			else if (dynamic_cast<CN*>(pChef)) availCN.enqueue(pChef);

			if (pOrd->GetType() == "COMBO") {
				COMBO* cOrd = dynamic_cast<COMBO*>(pOrd);
				Chef** extraC = cOrd->getExtraChefs();
				for (int i = 0; i < cOrd->getExtraChefCount(); i++) {
					extraC[i]->setAvailable();
					if (dynamic_cast<CS*>(extraC[i])) availCS.enqueue(extraC[i]);
					else availCN.enqueue(extraC[i]);
				}
			}

			string type = pOrd->GetType();
			if (type == "COMBO") readyCombo.enqueue(pOrd);
			else if (type == "OT") readyTakeaway.enqueue(pOrd);
			else if (type == "ODG" || type == "ODN") readyDineIn.enqueue(pOrd);
			else if (type == "OVC") readyOVC.enqueue(pOrd);
			else if (type == "OVG") readyOVG.enqueue(pOrd);
			else if (type == "OVN") readyOVN.enqueue(pOrd);
		}
		else break;
	}
}

void Restaurant::AssignTable() {
	Order* pOrd; Table* pTable;
	while (!readyDineIn.isEmpty()) {
		pOrd = readyDineIn.peekFront();
		DineInOrder* dineOrd = dynamic_cast<DineInOrder*>(pOrd);
		int reqSeats = dineOrd->getNSeats(); bool wantsShare = dineOrd->getShare();
		bool tableFound = false;

		if (wantsShare) {
			if (busySharable.GetBest(reqSeats, pTable)) {
				tableFound = true; pTable->reserveTable(dineOrd, reqSeats, wantsShare);
				dineOrd->setTable(pTable); busySharable.enqueue(pTable, pTable->getPriority());
			}
		}
		if (!tableFound) {
			if (availTables.GetBest(reqSeats, pTable)) {
				tableFound = true; pTable->reserveTable(dineOrd, reqSeats, wantsShare);
				dineOrd->setTable(pTable);
				if (wantsShare) busySharable.enqueue(pTable, pTable->getPriority());
				else busyNoShare.enqueue(pTable, pTable->getPriority());
			}
		}
		if (tableFound) {
			readyDineIn.dequeue(); pOrd->setTS(currentTimestep);
			pOrd->setTserv(dineOrd->getDuration()); pOrd->setStatus(IN_SERVICE);
			inServiceOrders.enqueue(pOrd, -(pOrd->getTF()));
		}
		else break;
	}
}

void Restaurant::UpdateOverwaitOrders() {
	int count = readyOVG.getCount();


	//first in the queue is the order that waited the most
	for (int i = 0; i < count; i++) {
		Order* pOrd = readyOVG.peekFront();
		if ((currentTimestep - pOrd->getTR()) > TH) {
			readyOVG.dequeue();
			overwaitOVG.enqueue(pOrd, -(pOrd->getTQ()));
			totalOverwaitCount++;
		}
		else break;
	}
}

void Restaurant::AssignScooter() {                        //COMBO ---> Overwait OVG ---> OVC ---> FCFS min TR of OVG & OVN
	Order* pOrd = nullptr; Scooter* pScooter; double pri;

	while (!availScooters.isEmpty()) {
		bool assigned = false;

		if (isBonus && !readyCombo.isEmpty()) {
			COMBO* cOrd = dynamic_cast<COMBO*>(readyCombo.peekFront());

			if (availScooters.getCount() >= cOrd->getReqScooters()) {
				pOrd = readyCombo.peekFront(); readyCombo.dequeue();

				availScooters.dequeue(pScooter, pri);
				pScooter->deliverOrder(pOrd, currentTimestep);
				dynamic_cast<DeliveryOrder*>(pOrd)->setScooter(pScooter);

				int extraNeeded = cOrd->getReqScooters() - 1;
				for (int i = 0; i < extraNeeded; i++) {
					Scooter* extraScooter;
					availScooters.dequeue(extraScooter, pri);
					extraScooter->deliverOrder(pOrd, currentTimestep);
					cOrd->addExtraScooter(extraScooter);
				}
				assigned = true;
			}
		}

		if (!assigned) {
			if (!overwaitOVG.isEmpty()) {
				overwaitOVG.dequeue(pOrd, pri); assigned = true;
			}
			else if (!readyOVC.isEmpty()) {
				pOrd = readyOVC.peekFront(); readyOVC.dequeue(); assigned = true;
			}
			else if (!readyOVG.isEmpty() && !readyOVN.isEmpty()) {
				if (readyOVG.peekFront()->getTR() < readyOVN.peekFront()->getTR()) {
					pOrd = readyOVG.peekFront(); readyOVG.dequeue(); assigned = true;
				}
				else {
					pOrd = readyOVN.peekFront(); readyOVN.dequeue(); assigned = true;
				}
			}
			else if (!readyOVG.isEmpty()) {
				pOrd = readyOVG.peekFront(); readyOVG.dequeue(); assigned = true;
			}
			else if (!readyOVN.isEmpty()) {
				pOrd = readyOVN.peekFront(); readyOVN.dequeue(); assigned = true;
			}
		}

		if (!assigned) break;

		if (pOrd && pOrd->GetType() != "COMBO") {
			availScooters.dequeue(pScooter, pri);
			pScooter->deliverOrder(pOrd, currentTimestep);
			dynamic_cast<DeliveryOrder*>(pOrd)->setScooter(pScooter);

			if (isBonus && availScooters.getCount() > 0 && (rand() % 100 < failProb)) {
				pScooter->setMaintenance(currentTimestep);
				scootersBack.enqueue(pScooter, -(pScooter->getBackTime()));

				Scooter* rescueScooter;
				availScooters.dequeue(rescueScooter, pri);
				rescueScooter->deliverOrder(pOrd, currentTimestep);
				dynamic_cast<DeliveryOrder*>(pOrd)->setScooter(rescueScooter);

				totalRescues++;
			}
		}

		pOrd->setStatus(IN_SERVICE);
		inServiceOrders.enqueue(pOrd, -(pOrd->getTF()));
	}
}

void Restaurant::FinalizeTakeawayOrders() {
	Order* pOrd;
	while (!readyTakeaway.isEmpty()) {
		pOrd = readyTakeaway.peekFront(); readyTakeaway.dequeue();
		pOrd->setTS(currentTimestep); pOrd->setTserv(0);
		pOrd->setStatus(FINISHED); finishedOrders.push(pOrd);
	}
}

bool Restaurant::LoadFromFile(string filename, bool& isBonusFile) {
	std::ifstream inFile(filename);

	if (!inFile.is_open()) {
		pUI->printMessage("Error: Could not open the file. Please check the name and try again.");
		return false;
	}

	ClearUp();

	int numCS, numCN, speedCS, speedCN;
	inFile >> numCN >> numCS >> speedCN >> speedCS;

	int chefID = 1;
	for (int i = 0; i < numCS; i++) availCS.enqueue(new CS(chefID++, speedCS));
	for (int i = 0; i < numCN; i++) availCN.enqueue(new CN(chefID++, speedCN));

	int numScooters, speedScooter, mOrds, mDur;
	inFile >> numScooters >> speedScooter >> mOrds >> mDur;

	for (int i = 0; i < numScooters; i++) {
		availScooters.enqueue(new Scooter(i + 1, speedScooter, 1, mOrds, mDur), 0);
	}

	int numTablesTotal;
	inFile >> numTablesTotal;

	int tablesCreated = 0; int tableID = 1;
	while (tablesCreated < numTablesTotal) {
		int count, capacity; inFile >> count >> capacity;
		for (int i = 0; i < count; i++) {
			availTables.enqueue(new Table(tableID++, capacity), -capacity);
			tablesCreated++;
		}
	}

	LinkedQueue<int> midVars;
	string token;

	while (inFile >> token) {
		if (token == "Q" || token == "X") break;
		midVars.enqueue(std::stoi(token));

	}

	isBonusFile = (midVars.getCount() >= 3);

	int numActions = 0;
	if (isBonusFile) {
		TH = midVars.peekFront(); midVars.dequeue();
		failProb = midVars.peekFront(); midVars.dequeue();
		numActions = midVars.peekFront(); midVars.dequeue();
		pUI->printMessage("\n[Bonus File] >>>>>> " + filename.substr(3) + " loaded successfully!");
	}
	else {
		if (!midVars.isEmpty()) numActions = midVars.peekFront(); midVars.dequeue();
		pUI->printMessage("\n[Standard File] >>>>>> " + filename.substr(3) + " loaded successfully!");
	}

	for (int i = 0; i < numActions; i++) {
		char actionType;
		if (i == 0 && (token == "Q" || token == "X")) {
			actionType = token[0];
		}
		else {
			inFile >> actionType;
		}

		if (actionType == 'Q') {
			string orderType; int TS, ID, size; double money;
			inFile >> orderType >> TS >> ID >> size >> money;

			if (orderType == "ODG" || orderType == "ODN") {
				int seats, duration; char shareChar; inFile >> seats >> duration >> shareChar;
				bool share = (shareChar == 'Y');
				actionList.enqueue(new RequestAction(TS, ID, orderType, size, money, seats, duration, share));
			}
			else if (orderType == "OVC" || orderType == "OVG" || orderType == "OVN") {
				double distance; inFile >> distance;
				actionList.enqueue(new RequestAction(TS, ID, orderType, size, money, distance));
			}
			else if (orderType == "COMBO" && isBonusFile) {
				double distance; int reqC, reqS;
				inFile >> distance >> reqC >> reqS;
				actionList.enqueue(new RequestAction(TS, ID, orderType, size, money, distance, reqC, reqS));
			}
			else if (orderType == "OT") {
				actionList.enqueue(new RequestAction(TS, ID, orderType, size, money));
			}
		}
		else if (actionType == 'X') {
			int TS, ID; inFile >> TS >> ID; actionList.enqueue(new CancelAction(TS, ID));
		}
	}

	inFile.close();
	return true;
}


void Restaurant::SaveOutput(string filename) {
	std::ofstream outFile(filename);
	if (!outFile.is_open()) return;
	int percision = 3;
	outFile << "TF\tID\tTQ\tTA\tTR\tTS\tTi\tTC\tTW\tTserv\n";

	int totalOrders = 0, totalODG = 0, totalODN = 0, totalOT = 0;
	int totalOVC = 0, totalOVG = 0, totalOVN = 0, totalCombo = 0;
	int totalFinished = 0, totalCancelled = 0;
	double sumTi = 0, sumTC = 0, sumTw = 0, sumTserv = 0;

	Order* pOrd;
	while (!finishedOrders.isEmpty()) {
		pOrd = finishedOrders.top(); finishedOrders.pop();

		string type = pOrd->GetType();
		if (type == "ODG") totalODG++;
		else if (type == "ODN") totalODN++;
		else if (type == "OT") totalOT++;
		else if (type == "OVC") totalOVC++;
		else if (type == "OVG") totalOVG++;
		else if (type == "OVN") totalOVN++;
		else if (type == "COMBO") totalCombo++;

		totalFinished++;
		sumTi += pOrd->getTI(); sumTC += pOrd->getTC();
		sumTw += pOrd->getTW(); sumTserv += pOrd->getTserv();

		outFile << pOrd->getTF() << "\t" << pOrd->getID() << "\t" << pOrd->getTQ() << "\t"
			<< pOrd->getTA() << "\t" << pOrd->getTR() << "\t" << pOrd->getTS() << "\t"
			<< pOrd->getTI() << "\t" << pOrd->getTC() << "\t" << pOrd->getTW() << "\t"
			<< pOrd->getTserv() << "\n";

		delete pOrd;
	}

	while (!cancelledOrders.isEmpty()) {
		pOrd = cancelledOrders.peekFront(); cancelledOrders.dequeue();
		string type = pOrd->GetType();
		if (type == "ODG") totalODG++; else if (type == "ODN") totalODN++;
		else if (type == "OT") totalOT++; else if (type == "OVC") totalOVC++;
		else if (type == "OVG") totalOVG++; else if (type == "OVN") totalOVN++;
		totalCancelled++; delete pOrd;
	}

	totalOrders = totalFinished + totalCancelled;
	int totalCS = availCS.getCount(); int totalCN = availCN.getCount();
	int totalChefs = totalCS + totalCN;

	double chefBusyTime = 0; Chef* pChef;
	while (!availCS.isEmpty()) { pChef = availCS.peekFront(); availCS.dequeue(); chefBusyTime += pChef->getTotalBusyTime(); delete pChef; }
	while (!availCN.isEmpty()) { pChef = availCN.peekFront(); availCN.dequeue(); chefBusyTime += pChef->getTotalBusyTime(); delete pChef; }

	int totalScooters = availScooters.getCount();
	double scooterBusyTime = 0; Scooter* pScooter; double pri;
	while (availScooters.dequeue(pScooter, pri)) { scooterBusyTime += pScooter->getTotalBusyTime(); delete pScooter; }

	double avgTi = totalFinished > 0 ? sumTi / totalFinished : 0;
	double avgTC = totalFinished > 0 ? sumTC / totalFinished : 0;
	double avgTw = totalFinished > 0 ? sumTw / totalFinished : 0;
	double avgTserv = totalFinished > 0 ? sumTserv / totalFinished : 0;

	double pctFinished = totalOrders > 0 ? (totalFinished * 100.0) / totalOrders : 0;
	double pctCancelled = totalOrders > 0 ? (totalCancelled * 100.0) / totalOrders : 0;
	double pctOverwait = totalOrders > 0 ? (totalOverwaitCount * 100.0) / totalOrders : 0;

	int actualSimTime = currentTimestep > 1 ? currentTimestep - 1 : 1;
	double chefUtil = totalChefs > 0 ? (chefBusyTime / (actualSimTime * totalChefs)) * 100.0 : 0;
	double scooterUtil = totalScooters > 0 ? (scooterBusyTime / (actualSimTime * totalScooters)) * 100.0 : 0;

	outFile << "\n\n";
	outFile << "Total Orders: " << totalOrders << " [ODG: " << totalODG << ", ODN: " << totalODN << ", OT: " << totalOT
		<< ", OVC: " << totalOVC << ", OVG: " << totalOVG << ", OVN: " << totalOVN;
	if (isBonus) outFile << ", COMBO: " << totalCombo;
	outFile << "]\n";

	outFile << "Total Chefs: " << totalChefs << " [CS: " << totalCS << ", CN: " << totalCN << "]\n";
	outFile << "Total Scooters: " << totalScooters << "\n";

	outFile << "\nFinished Orders: " << formatClean(pctFinished, percision)
		<< "% \nCancelled Orders: " << formatClean(pctCancelled, percision) << "%\n";

	if (isBonus) {
		outFile << "\nOverwait Threshold: " << TH << "\n";
		outFile << "Overwait OVG Orders: " << formatClean(pctOverwait, percision) << "%\n";
		outFile << "Rescue Scooters Deployed: " << totalRescues << "\n";
	}

	outFile << "\nAverages for Finished Orders => "
		<< "Ti: " << formatClean(avgTi, percision) << ", "
		<< "TC: " << formatClean(avgTC, percision) << ", "
		<< "Tw: " << formatClean(avgTw, percision) << ", "
		<< "Tserv: " << formatClean(avgTserv, percision) << "\n";

	outFile << "\nScooters Utilization: " << formatClean(scooterUtil, percision) << "%\n";
	outFile << "Chefs Utilization: " << formatClean(chefUtil, percision) << "%\n";

	outFile.close();
}

string Restaurant::formatClean(double val, int percision) {
	string s = format("{:.{}f}", val, percision);
	int lastValidIndx = s.length() - 1;
	while (lastValidIndx > 0 && s[lastValidIndx] == '0')
		--lastValidIndx;
	if (s[lastValidIndx] == '.')
		--lastValidIndx;

	return s.substr(0, lastValidIndx + 1);
}