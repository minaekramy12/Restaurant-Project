#include "UI.h"
#include <conio.h>
#include <cstdlib>

string UI::readInputFileName() {
	string filename;
	cout << "\nPlease enter the input file name: ";
	cin >> filename;
	return filename;
}

string UI::readOutputFileName() {
	string filename;
	cout << "\nPlease enter the output file name: ";
	cin >> filename;
	return filename;
}

SimulationMode UI::readSimulationMode() {
	int mode = -1;
	while (mode < 0 || mode > 2) {
		cout << "\nSelect Simulation Mode:"
			<< "\n0. Random"
			<< "\n1. Interactive Mode"
			<< "\n2. Silent Mode"
			<< "\nEnter choice (0, 1 OR 2): ";
		cin >> mode;
	}
	return static_cast<SimulationMode>(mode);
}

void UI::printTimestep(int timestep) {
	cout << "\n================================= Current Timestep: " << timestep << " =================================" << endl;
}

void UI::printMessage(string msg) {
	cout << msg << endl;
}

void UI::waitForUser() {
	cout << "PRESS ANY KEY TO MOVE TO NEXT STEP..." << endl;
	_getch();
}

void UI::clearScreen() {
	std::system("cls");
	//cout << "\033[2J\033[H" << std::flush;
}

void UI::printSystemStatus(
	LinkedQueue<Action*>* actionList,
	LinkedQueue<Order*>* pendTakeaway, LinkedQueue<Order*>* pendODN, LinkedQueue<Order*>* pendODG,
	CancellableQueue<Order*>* pendOVC, priQueue<Order*>* pendOVG, LinkedQueue<Order*>* pendOVN,
	LinkedQueue<Chef*>* availCS, LinkedQueue<Chef*>* availCN,
	CancellablePriQueue<Order*>* cookingOrders,
	LinkedQueue<Order*>* readyTakeaway, LinkedQueue<Order*>* readyDineIn, CancellableQueue<Order*>* readyDelivery,
	priQueue<Scooter*>* availScooters,
	TablePriQueue<Table*>* availTables, TablePriQueue<Table*>* busySharable, TablePriQueue<Table*>* busyNoShare,
	priQueue<Order*>* inServiceOrders,
	LinkedQueue<Scooter*>* inMaintenanceScooters,
	priQueue<Scooter*>* scootersBack,
	LinkedQueue<Order*>* cancelledOrders,
	LinkedStack<Order*>* finishedOrders)
{
	cout << actionList->getCount() << " actions remaining: ";
	actionList->Print(10);
	cout << endl;

	cout << "Pending Orders IDs" << endl;
	cout << pendTakeaway->getCount() << " OT: "; pendTakeaway->Print(); cout << endl;
	cout << pendODN->getCount() << " ODN: "; pendODN->Print(); cout << endl;
	cout << pendODG->getCount() << " ODG: "; pendODG->Print(); cout << endl;
	cout << pendOVC->getCount() << " OVC: "; pendOVC->Print(); cout << endl;
	cout << pendOVG->getCount() << " OVG: "; pendOVG->Print(); cout << endl;
	cout << pendOVN->getCount() << " OVN: "; pendOVN->Print(); cout << endl;
	cout << "---------------------------------" << endl;

	cout << "Available chefs IDs" << endl;
	cout << availCS->getCount() << " CS: "; availCS->Print(); cout << endl;
	cout << availCN->getCount() << " CN: "; availCN->Print(); cout << endl;
	cout << "---------------------------------" << endl;

	cout << "Cooking orders [Orders ID, chef ID]" << endl;
	cout << cookingOrders->getCount() << " cooking orders: "; cookingOrders->Print(); cout << endl;
	cout << "---------------------------------" << endl;

	cout << "Ready Orders IDs" << endl;
	cout << readyTakeaway->getCount() << " Takeaway: "; readyTakeaway->Print(); cout << endl;
	cout << readyDineIn->getCount() << " Dine-In: "; readyDineIn->Print(); cout << endl;
	cout << readyDelivery->getCount() << " Delivery: "; readyDelivery->Print(); cout << endl;
	cout << "---------------------------------" << endl;

	cout << "Available scooters IDs" << endl;
	cout << availScooters->getCount() << " Scooters: "; availScooters->Print(); cout << endl;
	cout << "---------------------------------" << endl;

	cout << "Available tables [ID, capacity, free seats]" << endl;
	cout << availTables->getCount() << " tables: "; availTables->Print(); cout << endl;
	cout << "---------------------------------" << endl;

	cout << "Busy Sharable tables [ID, capacity, free seats]" << endl;
	cout << busySharable->getCount() << " tables: "; busySharable->Print(); cout << endl;
	cout << "---------------------------------" << endl;

	cout << "Busy Non-Sharable tables [ID, capacity, free seats]" << endl;
	cout << busyNoShare->getCount() << " tables: "; busyNoShare->Print(); cout << endl;
	cout << "---------------------------------" << endl;

	cout << "In-Service orders [order ID, scooter/Table ID]" << endl;
	cout << inServiceOrders->getCount() << " Orders: "; inServiceOrders->Print(); cout << endl;
	cout << "---------------------------------" << endl;

	cout << "In-maintainance scooters IDs" << endl;
	cout << inMaintenanceScooters->getCount() << " scooters: "; inMaintenanceScooters->Print(); cout << endl;
	cout << "---------------------------------" << endl;

	cout << "Scooters Back to Restaurant IDs" << endl;
	cout << scootersBack->getCount() << " scooters: "; scootersBack->Print(); cout << endl;
	cout << "---------------------------------" << endl;

	cout << "Cancelled Orders IDs" << endl;
	cout << cancelledOrders->getCount() << " cancelled: "; cancelledOrders->Print(); cout << endl;
	cout << "---------------------------------" << endl;

	cout << "Finished orders IDs----" << endl;
	cout << finishedOrders->getCount() << " Orders: "; finishedOrders->Print(); cout << endl;
}