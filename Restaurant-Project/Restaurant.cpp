#include "Restaurant.h"

Restaurant::Restaurant() {
	pUI = new UI();
	currentTimestep = 1;
}

Restaurant::~Restaurant() {
	delete pUI;
	ClearUp();
}


void Restaurant::ClearUp()
{
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
	while (!readyOVC.isEmpty()) {
		Order* pOrd = readyOVC.peekFront();
		readyOVC.dequeue();
		delete pOrd;
	}
	while (!readyOVG.isEmpty()) {
		Order* pOrd = readyOVG.peekFront();
		readyOVG.dequeue();
		delete pOrd;
	}
	while (!readyOVN.isEmpty()) {
		Order* pOrd = readyOVN.peekFront();
		readyOVN.dequeue();
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
		readyDineIn.isEmpty() && readyOVC.isEmpty() && readyOVG.isEmpty() &&
		readyOVN.isEmpty() && inServiceOrders.isEmpty();
}


void Restaurant::RunSimulation() {
	pUI->clearScreen();
	ClearUp();
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

		if (mode == INTERACTIVE) {
			pUI->clearScreen();
			pUI->printTimestep(currentTimestep);

			pUI->printSystemStatus(
				&actionList,
				&pendTakeaway, &pendODN, &pendODG,
				&pendOVC, &pendOVG, &pendOVN,
				&availCS, &availCN,
				&cookingOrders,
				&readyTakeaway, &readyDineIn,
				&readyOVC, &readyOVG, &readyOVN,
				&availScooters,
				&availTables, &busySharable, &busyNoShare,
				&inServiceOrders,
				&inMaintenanceScooters,
				&scootersBack,
				&cancelledOrders,
				&finishedOrders
			);

			ReturnScooters();
			FinishOrders();
			ServeOrders();
			FinishCooking();
			AssignOrders();

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
			if (dynamic_cast<CS*>(assignedChef)) availCS.enqueue(assignedChef);
			else if (dynamic_cast<CN*>(assignedChef)) availCN.enqueue(assignedChef);
		}
		pCancelledOrder->setStatus(CANCELLED);
		cancelledOrders.enqueue(pCancelledOrder);
		return;
	}

	if (readyOVC.CancelOrder(id, pCancelledOrder)) {
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
		pTable->setTableFree(); // set free
		availTables.enqueue(pTable, pTable->getPriority()); //-capacity
	}
	else {
		busySharable.enqueue(pTable, pTable->getPriority()); //-freeSeats
	}
}

void Restaurant::AssignOrders() {
	Order* pOrd;
	Chef* pChef;
	double pri;

	while (!pendOVG.isEmpty()) {
		if (!availCS.isEmpty()) { pChef = availCS.peekFront(); availCS.dequeue(); }
		else if (!availCN.isEmpty()) { pChef = availCN.peekFront(); availCN.dequeue(); }
		else break;

		pendOVG.dequeue(pOrd, pri);
		pChef->assignOrder(pOrd, currentTimestep);
		pOrd->setStatus(COOKING);
		cookingOrders.enqueue(pOrd, -(pOrd->getTR())); //-TR
	}

	while (!pendOVC.isEmpty()) {
		if (!availCS.isEmpty()) { pChef = availCS.peekFront(); availCS.dequeue(); }
		else if (!availCN.isEmpty()) { pChef = availCN.peekFront(); availCN.dequeue(); }
		else break;

		pOrd = pendOVC.peekFront(); pendOVC.dequeue();

		pChef->assignOrder(pOrd, currentTimestep);
		pOrd->setStatus(COOKING);
		cookingOrders.enqueue(pOrd, -(pOrd->getTR()));
	}

	while (!pendODG.isEmpty()) {
		if (!availCS.isEmpty()) { pChef = availCS.peekFront(); availCS.dequeue(); }
		else if (!availCN.isEmpty()) { pChef = availCN.peekFront(); availCN.dequeue(); }
		else break;

		pOrd = pendODG.peekFront(); pendODG.dequeue();

		pChef->assignOrder(pOrd, currentTimestep);
		pOrd->setStatus(COOKING);
		cookingOrders.enqueue(pOrd, -(pOrd->getTR()));
	}

	while (!pendOVN.isEmpty()) {
		if (!availCN.isEmpty()) { pChef = availCN.peekFront(); availCN.dequeue(); }
		else if (!availCS.isEmpty()) { pChef = availCS.peekFront(); availCS.dequeue(); }
		else break;

		pOrd = pendOVN.peekFront(); pendOVN.dequeue();

		pChef->assignOrder(pOrd, currentTimestep);
		pOrd->setStatus(COOKING);
		cookingOrders.enqueue(pOrd, -(pOrd->getTR()));
	}

	while (!pendODN.isEmpty()) {
		if (!availCN.isEmpty()) { pChef = availCN.peekFront(); availCN.dequeue(); }
		else if (!availCS.isEmpty()) { pChef = availCS.peekFront(); availCS.dequeue(); }
		else break;

		pOrd = pendODN.peekFront(); pendODN.dequeue();

		pChef->assignOrder(pOrd, currentTimestep);
		pOrd->setStatus(COOKING);
		cookingOrders.enqueue(pOrd, -(pOrd->getTR()));
	}

	while (!pendTakeaway.isEmpty()) {
		if (!availCN.isEmpty()) { pChef = availCN.peekFront(); availCN.dequeue(); }
		else if (!availCS.isEmpty()) { pChef = availCS.peekFront(); availCS.dequeue(); }
		else break;

		pOrd = pendTakeaway.peekFront(); pendTakeaway.dequeue();

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

			if (dynamic_cast<CS*>(pChef)) availCS.enqueue(pChef);
			else if (dynamic_cast<CN*>(pChef)) availCN.enqueue(pChef);

			string type = pOrd->GetType();
			if (type == "OT") readyTakeaway.enqueue(pOrd);
			else if (type == "ODG" || type == "ODN") readyDineIn.enqueue(pOrd);
			else if (type == "OVC") readyOVC.enqueue(pOrd);
			else if (type == "OVG") readyOVG.enqueue(pOrd);
			else if (type == "OVN") readyOVN.enqueue(pOrd);
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

	while (!availScooters.isEmpty() && (!readyOVC.isEmpty() || !readyOVG.isEmpty() || !readyOVN.isEmpty())) {

		if (!readyOVC.isEmpty()) {
			pOrd = readyOVC.peekFront();
			readyOVC.dequeue();
		}
		else if (!readyOVG.isEmpty()) {
			pOrd = readyOVG.peekFront();
			readyOVG.dequeue();
		}
		else {
			pOrd = readyOVN.peekFront();
			readyOVN.dequeue();
		}

		availScooters.dequeue(pScooter, pri);

		pScooter->deliverOrder(pOrd, currentTimestep); // InMaintainance if orders > max
		dynamic_cast<DeliveryOrder*>(pOrd)->setScooter(pScooter);

		pOrd->setStatus(IN_SERVICE);
		inServiceOrders.enqueue(pOrd, -(pOrd->getTF())); // Priority is -TF
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
				busySharable.enqueue(pTable, pTable->getPriority());
			}
		}

		if (!tableFound) {
			if (availTables.GetBest(reqSeats, pTable)) {
				tableFound = true;
				pTable->reserveTable(dineOrd, reqSeats, wantsShare);
				dineOrd->setTable(pTable);

				if (wantsShare) busySharable.enqueue(pTable, pTable->getPriority());
				else busyNoShare.enqueue(pTable, pTable->getPriority());
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
					if (pScooter->getState() != InMaintainance) { //if not maintenance set to back
						pScooter->setBack();
					}
					scootersBack.enqueue(pScooter, -(pScooter->getBackTime()));
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

			if (s->getState() == InMaintainance) {
				s->setMaintenance(currentTimestep); // Sets main finish
				inMaintenanceScooters.enqueue(s);
			}
			else {
				s->setAvailable();
				availScooters.enqueue(s, s->getPriority()); // -totalDistance
			}
		}
		else {
			break;
		}
	}

	int maintCount = inMaintenanceScooters.getCount();
	for (int i = 0; i < maintCount; i++) {
		s = inMaintenanceScooters.peekFront();
		inMaintenanceScooters.dequeue();
		if (currentTimestep >= -(s->getPriority())) {
			s->setAvailable(); // Resets scooter state successfully
			availScooters.enqueue(s, s->getPriority()); //-totalDistance
		}
		else {
			inMaintenanceScooters.enqueue(s);
		}
	}
}

bool Restaurant::LoadFromFile(string filename) {
	std::ifstream inFile(filename);

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
	std::ofstream outFile(filename);
	if (!outFile.is_open()) {
		pUI->printMessage("Error: Could not create output file.");
		return;
	}

	outFile << "TF\tID\tTQ\tTA\tTR\tTS\tTi\tTC\tTW\tTserv\n";

	int totalOrders = 0, totalODG = 0, totalODN = 0, totalOT = 0;
	int totalOVC = 0, totalOVG = 0, totalOVN = 0;
	int totalFinished = 0, totalCancelled = 0;
	double sumTi = 0, sumTC = 0, sumTw = 0, sumTserv = 0;

	Order* pOrd;
	while (!finishedOrders.isEmpty()) {
		pOrd = finishedOrders.top();
		finishedOrders.pop();

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

		delete pOrd;
	}

	while (!cancelledOrders.isEmpty()) {
		pOrd = cancelledOrders.peekFront();
		cancelledOrders.dequeue();

		string type = pOrd->GetType();
		if (type == "ODG") totalODG++;
		else if (type == "ODN") totalODN++;
		else if (type == "OT") totalOT++;
		else if (type == "OVC") totalOVC++;
		else if (type == "OVG") totalOVG++;
		else if (type == "OVN") totalOVN++;

		totalCancelled++;

		delete pOrd;
	}

	totalOrders = totalFinished + totalCancelled;

	int totalCS = availCS.getCount();
	int totalCN = availCN.getCount();
	int totalChefs = totalCS + totalCN;

	double chefBusyTime = 0;
	Chef* pChef;

	while (!availCS.isEmpty()) {
		pChef = availCS.peekFront();
		availCS.dequeue();
		chefBusyTime += pChef->getTotalBusyTime();
		delete pChef;
	}
	while (!availCN.isEmpty()) {
		pChef = availCN.peekFront();
		availCN.dequeue();
		chefBusyTime += pChef->getTotalBusyTime();
		delete pChef;
	}

	int totalScooters = availScooters.getCount();
	double scooterBusyTime = 0;
	Scooter* pScooter;
	double pri;

	while (availScooters.dequeue(pScooter, pri)) {
		scooterBusyTime += pScooter->getTotalBusyTime();
		delete pScooter;
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
