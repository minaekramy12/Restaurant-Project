#include "Action.h"
#include "Order.h"
#include "Restaurant.h"

void RequestAction::Execute(Restaurant* pRest) {
	Order* pOrd = nullptr;
	if (orderType == "ODG") {
		pOrd = new ODGOrder(orderID, timestep, size, price, seats, duration, canShare);
	}
	else if (orderType == "ODN") {
		pOrd = new ODNOrder(orderID, timestep, size, price, seats, duration, canShare);
	}
	else if (orderType == "OT") {
		pOrd = new TakeawayOrder(orderID, timestep, size, price);
	}
	else if (orderType == "OVC") {
		pOrd = new OVC(orderID, timestep, size, price, distance);
	}
	else if (orderType == "OVG") {
		pOrd = new OVG(orderID, timestep, size, price, distance);
	}
	else if (orderType == "OVN") {
		pOrd = new OVN(orderID, timestep, size, price, distance);
	}
	if (pOrd != nullptr) {
		pRest->AddToPending(pOrd);
	}
}

void CancelAction::Execute(Restaurant* pRest) {
	pRest->CancelOVC(orderID);
}