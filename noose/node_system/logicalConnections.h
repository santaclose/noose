#pragma once
#include "logicalConnection.h"
#include "logicalNode.h"

namespace logicalConnections
{
	extern std::vector<logicalConnection> connections;

	// returns the index of the connection or -1 if not connected
	//static int tryToConnect(logicalNode* nodeA, int& pinA, logicalNode* nodeB, int& pinB, int& nodeIndexA, int& nodeIndexB);
	//static int connect(logicalNode* nA, int& pinA, logicalNode* nB, int& pinB, int& nIndexA, int& nIndexB);
	void connect(int connectionIndex, const std::vector<logicalNode*>& nodeList, int nodeA, int nodeB, int pinA, int pinB);
	void deleteConnection(int connectionIndex);
}