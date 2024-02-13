#pragma once
#include <vector>
#include "connection.h"
#include "node.h"

namespace connectionSystem
{
	extern std::vector<connection> connections;
	void initialize();
	// nodeA is always the node that has the output pin
	int connect(const std::vector<node*>& nodeList, int nodeA, int nodeB, int pinA, int pinB);
	void deleteConnection(int connectionId);
	void clearEverything();
}