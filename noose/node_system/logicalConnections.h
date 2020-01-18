#pragma once
#include "logicalConnection.h"
#include "logicalNode.h"

class logicalConnections
{
public:
	static std::vector<logicalConnection> connections;

	// returns the index of the connection or -1 if not connected
	static int tryToConnect(logicalNode* nodeA, int& pinA, logicalNode* nodeB, int& pinB, int& nodeIndexA, int& nodeIndexB);
	static int getNodeA(int lineIndex);
	static int getNodeB(int lineIndex);
	static void deleteConnection(int lineIndex);
};