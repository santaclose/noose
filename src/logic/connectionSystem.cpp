#include "connectionSystem.h"
#include <iostream>

namespace connectionSystem {

	std::vector<connection> connections;
}

void connectionSystem::initialize()
{
	connections.reserve(64);
}

int connectionSystem::connect(const std::vector<node*>& nodeList, int nodeA, int nodeB, int pinA, int pinB)
{
	int connectionId = connections.size();
	connections.emplace_back();
	connections[connectionId].nodeA = nodeList[nodeA];
	connections[connectionId].nodeB = nodeList[nodeB];
	connections[connectionId].nodeIndexA = nodeA;
	connections[connectionId].nodeIndexB = nodeB;
	connections[connectionId].pinA = pinA;
	connections[connectionId].pinB = pinB;
	connections[connectionId].deleted = false;

	//connect right side node before
	nodeList[nodeB]->connect(connectionId);
	nodeList[nodeA]->connect(connectionId);

	return connectionId;
}

void connectionSystem::deleteConnection(int connectionId)
{
	connections[connectionId].nodeA->disconnect(connectionId);
	connections[connectionId].nodeB->disconnect(connectionId);

	connections[connectionId].deleted = true;
}

void connectionSystem::clearEverything()
{
	connections.clear();
}
