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

#ifdef TEST
void connectionSystem::print()
{
	std::cout << "--connections\n";
	for (const connection& c : connections)
	{
		std::cout << "-\n\tdeleted: " << c.deleted << std::endl;
		std::cout << "\tnodeA: " << c.nodeA << std::endl;
		std::cout << "\tnodeB: " << c.nodeB << std::endl;
		std::cout << "\tpinA: " << c.pinA << std::endl;
		std::cout << "\tpinB: " << c.pinB << std::endl;
		std::cout << "\tnodeIndexA: " << c.nodeIndexA << std::endl;
		std::cout << "\tnodeIndexB: " << c.nodeIndexB << std::endl;
	}
}
#endif