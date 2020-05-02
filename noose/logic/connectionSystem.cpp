#include "connectionSystem.h"
#include <iostream>

std::vector<connection> connectionSystem::connections;

void connectionSystem::connect(int connectionIndex, const std::vector<node*>& nodeList, int nodeA, int nodeB, int pinA, int pinB)
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

	//connect right side node before
	nodeList[nodeB]->connect(connectionIndex);
	nodeList[nodeA]->connect(connectionIndex);
}

void connectionSystem::deleteConnection(int lineIndex)
{
	connections[lineIndex].nodeA->disconnect(lineIndex);
	connections[lineIndex].nodeB->disconnect(lineIndex);

	connections[lineIndex].deleted = true;
}