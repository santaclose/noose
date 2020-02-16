#pragma once
#include <vector>
#include "node_system/logicalConnection.h"
#include "node_system/logicalNode.h"

class connectionSystem
{
public:
	static std::vector<logicalConnection> connections;
	static void connect(int connectionIndex, const std::vector<logicalNode*>& nodeList, int nodeA, int nodeB, int pinA, int pinB);
	static void deleteConnection(int lineIndex);
};

