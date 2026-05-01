#include "Restaurant.h"

int main() {
	char x;
	do {
		Restaurant rest;
		rest.RunSimulation();
		std::cout << "\n\npress Y to continue in the simulation: ";
		std::cin >> x;
	} while (x == 'y' || x == 'Y');
	return 0;
}