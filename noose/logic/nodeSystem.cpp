#include "nodeSystem.h"
#include "connectionSystem.h"
#include "node.h"
#include "../nodeData.h"
#include <iostream>

std::vector<node*> nodeList;

void insertNode(int slot, nodeData* data)
{
	if (slot >= nodeList.size())
		nodeList.resize(slot + 1);

	nodeList[slot] = new node(data);
}

void recalculatePropagationMatrices()
{
	for (node* n : nodeList)
		n->clearPropagationMatrix();

	int i = 0;
	for (connection& l : connectionSystem::connections)
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
	std::cout << "[Node system] node system initialized\n";
}

void nodeSystem::onNodeCreated(int n, const void* data)
{
	std::cout << "[Node system] node created\n\tid: " << n << std::endl;
	insertNode(n, (nodeData*)data);
}

void nodeSystem::onNodeDeleted(int n, const std::vector<int>& connections)//int* ci, int cc)
{
	std::cout << "[Node system] node deleted\n\tid: " << n << std::endl;
	// delete all connections to the node
	for (int c : connections)
		connectionSystem::deleteConnection(c);
	recalculatePropagationMatrices();
}

void nodeSystem::onNodeChanged(int n)
{
	std::cout << "[Node system] node changed\n\tid: " << n << std::endl;
	nodeList[n]->activate();
}

void nodeSystem::onNodesConnected(int nA, int nB, int pA, int pB, int c)
{
	std::cout << "[Node system] nodes connected\n\tnodeA: " << nA << "\n\tnodeB: " << nB << "\n\tpinA: " << pA << "\n\tpinB: " << pB << "\n\tconnection: " << c << std::endl;

	connectionSystem::connect(c, nodeList, nA, nB, pA, pB);

	//connect right side node before
	nodeList[nB]->connect(c);
	nodeList[nA]->connect(c);
	nodeList[nA]->activate();
}

void nodeSystem::onNodesDisconnected(int nA, int nB, int pA, int pB, int c)
{
	std::cout << "[Node system] nodes disconnected\n\tnodeA: " << nA << "\n\tnodeB: " << nB << "\n\tpinA: " << pA << "\n\tpinB: " << pB << "\n\tconnection: " << c << std::endl;

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
