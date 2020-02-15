#pragma once
#include "logicalConnection.h"
#include "logicalNode.h"

class logicalConnections
{
public:
	static std::vector<logicalConnection> connections;

	// returns the index of the connection or -1 if not connected
	//static int tryToConnect(logicalNode* nodeA, int& pinA, logicalNode* nodeB, int& pinB, int& nodeIndexA, int& nodeIndexB);
	static int connect(logicalNode* nA, int& pinA, logicalNode* nB, int& pinB, int& nIndexA, int& nIndexB);
	static void deleteConnection(int lineIndex);
};