#include "Restaurant.h"
#include <stdlib.h>
#include <time.h>

void Restaurant::RandomSimulation() {
	srand(time(NULL));
	int totalOrdersGenerated = 0;
	int modeChoice = 0;

	pUI->clearScreen();
	pUI->printMessage("Starting Random Simulation (Phase 1)...");

	bool stepByStep = (pUI->UIMenuRandom(totalOrdersGenerated) == 1);

	//resources
	for (int i = 1; i <= 20; i++) availCS.enqueue(new CS(i, 5));
	for (int i = 21; i <= 40; i++) availCN.enqueue(new CN(i, 4));
	for (int i = 1; i <= 20; i++) availScooters.enqueue(new Scooter(i, 30, 1, 10, 5), rand() % 100);
	for (int i = 1; i <= 20; i++) availTables.enqueue(new Table(i, 6), rand() % 10);

	for (int i = 1; i <= totalOrdersGenerated; i++) {
		int typeRoll = rand() % 6;
		Order* pOrd = nullptr;

		// Random values
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

	// end when == totalordergenerated
	while (finishedOrders.getCount() + cancelledOrders.getCount() < totalOrdersGenerated) {

		// Pending -> Random Chef -> Cooking
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
				else if (!availCS.isEmpty()) { pChef = availCS.peekFront(); availCS.dequeue(); }

				if (pChef) {
					pOrd->setStatus(COOKING);
					pChef->assignOrder(pOrd, currentTimestep);
					cookingOrders.enqueue(pOrd, rand() % 100);
				}
				else {
					// If no chefs
					AddToPending(pOrd);
				}
			}
		}

		// cooking -> ready list
		if ((rand() % 100) < 75) {
			for (int i = 0; i < 15; i++) {
				Order* pOrd = nullptr;
				double pri;
				if (!cookingOrders.isEmpty()) {
					cookingOrders.dequeue(pOrd, pri);
					pOrd->setStatus(READY);

					// free chef
					Chef* pChef = pOrd->getAssignedChef();
					if (pChef) {
						pChef->setAvailable();
						if (dynamic_cast<CS*>(pChef)) availCS.enqueue(pChef);
						else if (dynamic_cast<CN*>(pChef)) availCN.enqueue(pChef);
					}

					string type = pOrd->GetType();
					if (type == "OT") readyTakeaway.enqueue(pOrd);
					else if (type == "ODG" || type == "ODN") readyDineIn.enqueue(pOrd);
					else if (type == "OVC") readyOVC.enqueue(pOrd);
					else if (type == "OVG") readyOVG.enqueue(pOrd);
					else if (type == "OVN") readyOVN.enqueue(pOrd);
				}
			}
		}

		// Ready -> In-Service / Finish
		for (int i = 0; i < 10; i++) {
			Order* pOrd = nullptr;
			int listChoice = rand() % 5;

			if (listChoice == 0 && !readyTakeaway.isEmpty()) { pOrd = readyTakeaway.peekFront(); readyTakeaway.dequeue(); }
			else if (listChoice == 1 && !readyDineIn.isEmpty()) { pOrd = readyDineIn.peekFront(); readyDineIn.dequeue(); }
			else if (listChoice == 2 && !readyOVC.isEmpty()) { pOrd = readyOVC.peekFront(); readyOVC.dequeue(); }
			else if (listChoice == 3 && !readyOVG.isEmpty()) { pOrd = readyOVG.peekFront(); readyOVG.dequeue(); }
			else if (listChoice == 4 && !readyOVN.isEmpty()) { pOrd = readyOVN.peekFront(); readyOVN.dequeue(); }

			if (pOrd) {
				string type = pOrd->GetType();

				if (type == "OT") { // OT -> Finish list
					pOrd->setStatus(FINISHED);
					finishedOrders.push(pOrd);
				}
				else if (type == "OVC" || type == "OVG" || type == "OVN") { //Pick Scooter -> In-Service
					Scooter* pScooter = nullptr;
					double pri;
					if (!availScooters.isEmpty()) {
						availScooters.dequeue(pScooter, pri);
						dynamic_cast<DeliveryOrder*>(pOrd)->setScooter(pScooter);
						pOrd->setStatus(IN_SERVICE);
						inServiceOrders.enqueue(pOrd, rand() % 100);
					}
					else readyOVC.enqueue(pOrd); // if no scooters
				}
				else if (type == "ODG" || type == "ODN") { // Pick Table -> In-Service
					Table* pTable = nullptr;
					double pri;
					if (!availTables.isEmpty()) {
						availTables.dequeue(pTable, pri);
						DineInOrder* dOrd = dynamic_cast<DineInOrder*>(pOrd);
						dOrd->setTable(pTable);
						pOrd->setStatus(IN_SERVICE);
						inServiceOrders.enqueue(pOrd, rand() % 100);
					}
					else readyDineIn.enqueue(pOrd); // if no tables
				}
			}
		}

		// Generate ID
		int cancelId = (rand() % totalOrdersGenerated) + 1;
		Order* pCancelled = nullptr;

		if (pendOVC.CancelOrder(cancelId, pCancelled)) {
			pCancelled->setStatus(CANCELLED);
			cancelledOrders.enqueue(pCancelled);
		}

		else if (readyOVC.CancelOrder(cancelId, pCancelled)) {
			pCancelled->setStatus(CANCELLED);
			cancelledOrders.enqueue(pCancelled);
		}
		else if (cookingOrders.CancelOrder(cancelId, pCancelled)) {
			Chef* c = pCancelled->getAssignedChef();
			if (c) {
				c->setAvailable();
				if (dynamic_cast<CS*>(c)) availCS.enqueue(c);
				else if (dynamic_cast<CN*>(c)) availCN.enqueue(c);
			}
			pCancelled->setStatus(CANCELLED);
			cancelledOrders.enqueue(pCancelled);
		}

		// 25% In-Service -> Finish. Return Scooter/Table.
		if ((rand() % 100) < 25) {
			Order* pOrd = nullptr;
			double pri;
			if (!inServiceOrders.isEmpty()) {
				inServiceOrders.dequeue(pOrd, pri);
				pOrd->setStatus(FINISHED);
				finishedOrders.push(pOrd);

				string type = pOrd->GetType();
				if (type == "OVC" || type == "OVG" || type == "OVN") {
					Scooter* pScooter = dynamic_cast<DeliveryOrder*>(pOrd)->getScooter();
					if (pScooter) scootersBack.enqueue(pScooter, rand() % 100);
				}
				else if (type == "ODG" || type == "ODN") {
					Table* pTable = dynamic_cast<DineInOrder*>(pOrd)->getTable();
					if (pTable) availTables.enqueue(pTable, pTable->getPriority());
				}
			}
		}

		// 50% Scooter Back -> Avail OR Maintenance
		if ((rand() % 100) < 50) {
			Scooter* pScooter = nullptr;
			double pri;
			if (!scootersBack.isEmpty()) {
				scootersBack.dequeue(pScooter, pri);
				if (rand() % 2 == 0) availScooters.enqueue(pScooter, rand() % 100);
				else inMaintenanceScooters.enqueue(pScooter);
			}
		}

		// 50% Maintenance -> Free Scooters
		if ((rand() % 100) < 50) {
			Scooter* pScooter = nullptr;
			if (!inMaintenanceScooters.isEmpty()) {
				pScooter = inMaintenanceScooters.peekFront();
				inMaintenanceScooters.dequeue();
				availScooters.enqueue(pScooter, rand() % 100);
			}
		}
		if (stepByStep) {
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
			pUI->waitForUser();
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

	pUI->printMessage("\nPhase 1 Random Simulation Finished!");
	pUI->printMessage("\nAll " + std::to_string(totalOrdersGenerated) + " Orders have been Processed successfully.");
	pUI->waitForUser();
}