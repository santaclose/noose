#include "nodeSystem.h"
#include "connectionSystem.h"
#include "node_system/logicalNode.h"
#include "nodeData.h"
#include <iostream>

std::vector<logicalNode*> nodeList;

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
	for (logicalConnection& l : connectionSystem::connections)
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

void nodeSystem::onNodeDeleted(int n, int* ci, int cc)
{
	std::cout << "[NODE SYSTEM] node deleted\n\tid: " << n << std::endl;
	for (int i = 0; i < cc; i++)
	{
		connectionSystem::deleteConnection(ci[i]);
	}
	recalculatePropagationMatrices();
}

void nodeSystem::onNodeChanged(int n)
{
	std::cout << "[NODE SYSTEM] node changed\n\tid: " << n << std::endl;
	nodeList[n]->activate();
}

void nodeSystem::onNodesConnected(int nA, int nB, int pA, int pB, int c)
{
	std::cout << "[NODE SYSTEM] nodes connected\n\tnodeA: " << nA << "\n\tnodeB: " << nB << "\n\tpinA: " << pA << "\n\tpinB: " << pB << "\n\tconnection: " << c << std::endl;

	connectionSystem::connect(c, nodeList, nA, nB, pA, pB);

	//connect right side node before
	nodeList[nB]->connect(c);
	nodeList[nA]->connect(c);
	nodeList[nA]->activate();
}

void nodeSystem::onNodesDisconnected(int nA, int nB, int pA, int pB, int c)
{
	std::cout << "[NODE SYSTEM] nodes disconnected\n\tnodeA: " << nA << "\n\tnodeB: " << nB << "\n\tpinA: " << pA << "\n\tpinB: " << pB << "\n\tconnection: " << c << std::endl;

	nodeList[nA]->disconnect(c);
	nodeList[nB]->disconnect(c);

	connectionSystem::deleteConnection(c);

	recalculatePropagationMatrices();
}

bool nodeSystem::isConnectionValid(int nA, int nB, int pinA, int pinB)
{
	return
		nA != nB && // can't connect a node to itself
		nodeList[nA]->getPinType(pinA) == nodeList[nB]->getPinType(pinB); // both pins must be of the same type
		//(pinA < nodeList[nA]->getInputPinCount()) != (pinB < nodeList[nB]->getInputPinCount()) && // can't be both output or input
}

void** nodeSystem::getDataPointersForNode(int n)
{
	return nodeList[n]->getDataPointers();
}

sf::RenderTexture* nodeSystem::getFirstOutputImageForNode(int n)
{
	return nodeList[n]->getFirstOutputImage();
}
