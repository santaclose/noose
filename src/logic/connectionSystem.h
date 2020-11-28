#pragma once
#include <vector>
#include "connection.h"
#include "node.h"

namespace connectionSystem
{
	extern std::vector<connection> connections;
	void connect(int connectionIndex, const std::vector<node*>& nodeList, int nodeA, int nodeB, int pinA, int pinB);
	void deleteConnection(int lineIndex);
	void clearEverything();
}