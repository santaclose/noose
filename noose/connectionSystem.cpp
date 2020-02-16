#include "connectionSystem.h"

std::vector<logicalConnection> connectionSystem::connections;

void connectionSystem::connect(int connectionIndex, const std::vector<logicalNode*>& nodeList, int nodeA, int nodeB, int pinA, int pinB)
{
	if (connectionIndex >= connections.size())
		connections.resize(connectionIndex + 1);
	connections[connectionIndex].nodeA = nodeList[nodeA];
	connections[connectionIndex].nodeB = nodeList[nodeB];
	connections[connectionIndex].nodeIndexA = nodeA;
	connections[connectionIndex].nodeIndexB = nodeB;
	connections[connectionIndex].pinA = pinA;
	connections[connectionIndex].pinB = pinB;
	connections[connectionIndex].deleted = false;
}

void connectionSystem::deleteConnection(int lineIndex)
{
	connections[lineIndex].deleted = true;
}