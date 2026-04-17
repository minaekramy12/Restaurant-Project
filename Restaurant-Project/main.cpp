#include "Restaurant.h"

int main() {
	char x;
	do {
		Restaurant rest;
		rest.RunSimulation();
		std::cout << "\n\n\n\n\n\npress Y to continue in the simulation mode: ";
		std::cin >> x;
	} while (x == 'y' || x == 'Y');
	return 0;
}