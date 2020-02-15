#include "nodeSystem.h"
#include "node_system/logicalConnections.h"
#include "node_system/logicalNode.h"
#include "nodeData.h"
#include <iostream>
#include <cstdint>
#include <map>
#include <utility>

std::map<std::pair<uint64_t, uint64_t>, int> connections;
std::vector<logicalNode*> nodeList;

inline void insertConnectionWithNodeInfo(int nA, int nB, int pA, int pB, int connectionIndex)
{
	// insert in map
	uint64_t nkey = 0;
	nkey += (uint64_t)nA;
	nkey << 32;
	nkey |= (uint64_t)nB;
	uint64_t pkey = 0;
	pkey += (uint64_t)pA;
	pkey << 32;
	pkey |= (uint64_t)pB;
	connections[std::pair<uint64_t, uint64_t>(nkey, pkey)] = connectionIndex;
}

inline int getConnectionFromNodeInfo(int nA, int nB, int pA, int pB)
{
	// read from map
	uint64_t nkey = 0;
	nkey += (uint64_t)nA;
	nkey << 32;
	nkey |= (uint64_t)nB;
	uint64_t pkey = 0;
	pkey += (uint64_t)pA;
	pkey << 32;
	pkey |= (uint64_t)pB;
	return connections[std::pair<uint64_t, uint64_t>(nkey, pkey)];
}

void insertNode(int slot, nodeData* data)
{
	if (slot >= nodeList.size())
		nodeList.resize(slot + 1);

	nodeList[slot] = new logicalNode(data);
}

void recalculatePropagationMatrices()
{
	for (logicalNode* n : nodeList)
		n->clearPropagationMatrix();

	int i = 0;
	for (logicalConnection& l : logicalConnections::connections)
	{
		if (!l.deleted) // not deleted
		{
			l.nodeA->rebuildMatrices(i);
			l.nodeB->rebuildMatrices(i);
		}
		i++;
	}
}


void nodeSystem::initialize()
{
	std::cout << "[NODE SYSTEM] node system initialized\n";
}

void nodeSystem::onNodeCreated(int n, const void* data)
{
	std::cout << "[NODE SYSTEM] node created\n\tid: " << n << std::endl;
	insertNode(n, (nodeData*)data);
}

void nodeSystem::onNodeDeleted(int n)
{
	std::cout << "[NODE SYSTEM] node deleted\n\tid: " << n << std::endl;
}

void nodeSystem::onNodeChanged(int n)
{
	std::cout << "[NODE SYSTEM] node changed\n\tid: " << n << std::endl;
}

void nodeSystem::onNodesConnected(int nA, int nB, int pA, int pB, int c)
{
	std::cout << "[NODE SYSTEM] nodes connected\n\tnodeA: " << nA << "\n\tnodeB: " << nB << "\n\tpinA: " << pA << "\n\tpinB: " << pB << "\n\tconnection: " << c << std::endl;
	return;////////////////////////////////////

	int connectionIndex = logicalConnections::connect(nodeList[nA], pA, nodeList[nB], pB, nA, nB);

	insertConnectionWithNodeInfo(nA, nB, pA, pB, connectionIndex);

	//connect right side node before
	nodeList[nB]->connect(connectionIndex);
	nodeList[nA]->connect(connectionIndex);
	nodeList[nA]->activate();
}

void nodeSystem::onNodesDisconnected(int nA, int nB, int pA, int pB, int c)
{
	std::cout << "[NODE SYSTEM] nodes disconnected\n\tnodeA: " << nA << "\n\tnodeB: " << nB << "\n\tpinA: " << pA << "\n\tpinB: " << pB << "\n\tconnection: " << c << std::endl;
	return;////////////////////////////////////

	int lineToDelete = getConnectionFromNodeInfo(nA, nB, pA, pB);

	std::cout << "[NODE SYSTEM] disconnecting nodes from line " << lineToDelete << std::endl;
	nodeList[nA]->disconnect(lineToDelete);
	nodeList[nB]->disconnect(lineToDelete);
	logicalConnections::deleteConnection(lineToDelete);

	recalculatePropagationMatrices();
}

/*bool nodeSystem::canConnectToPin(int n, int pin)
{
	//std::cout << "[NODE SYSTEM] valid pin\n";
	return true;////////////////////////////////////
	return nodeList[n]->canConnectToPin(pin);
}*/

bool nodeSystem::isConnectionValid(int nA, int nB, int pinA, int pinB)
{
	return true;////////////////////////////////////
	return
		nA != nB && // can't connect a node to itself
		//(pinA < nodeList[nA]->getInputPinCount()) != (pinB < nodeList[nB]->getInputPinCount()) && // can't be both output or input
		nodeList[nA]->getPinType(pinA) == nodeList[nB]->getPinType(pinB); // both pins must be of the same type
}

void** nodeSystem::getDataPointersForNode(int n)
{
	return nodeList[n]->getDataPointers();
}
