#include "nodeSystem.h"
#include "connectionSystem.h"
#include "node.h"
#include "graphOperations.h"
#include "../nodeData.h"
#include <iostream>

std::vector<node*> nodeList;

void printSystemStatus()
{
	std::cout << "--nodeList\n";
	for (node* n : nodeList)
	{
		if (n != nullptr)
		{
			std::cout << "-\n";
			n->print();
		}
		else
			std::cout << "-\n\tnullptr\n";
	}
	connectionSystem::print();
}

void insertNode(int slot, nodeData* data)
{
	if (slot >= nodeList.size())
		nodeList.resize(slot + 1);

	nodeList[slot] = new node(data);
}

void recalculatePropagationMatrices()
{
	for (node* n : nodeList)
	{
		if (n != nullptr)
			n->clearPropagationMatrix();
	}

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
	std::cout << "[Node system] Initializing\n";
}

void nodeSystem::terminate()
{
	std::cout << "[Node system] Terminating\n";
	for (node* n : nodeList)
	{
		if (n != nullptr)
			delete n;
	}
}

void nodeSystem::onNodeCreated(int n, const void* data)
{
	std::cout << "[Node system] Node created\n\tid: " << n << std::endl;
	insertNode(n, (nodeData*)data);
}

void nodeSystem::onNodeDeleted(int n, const std::vector<int>& connections)//int* ci, int cc)
{
	std::cout << "[Node system] Node deleted\n\tid: " << n << std::endl;

	// delete all connections to the node
	for (int c : connections)
		connectionSystem::deleteConnection(c);

	delete nodeList[n];
	nodeList[n] = nullptr;

	recalculatePropagationMatrices();
}

void nodeSystem::onNodeChanged(int n)
{
	//std::cout << "[Node system] Node changed\n\tid: " << n << std::endl;
	nodeList[n]->activate();
}

void nodeSystem::onNodesConnected(int nA, int nB, int pA, int pB, int c)
{
	std::cout << "[Node system] Nodes connected\n\tnodeA: " << nA << "\n\tnodeB: " << nB << "\n\tpinA: " << pA << "\n\tpinB: " << pB << "\n\tconnection: " << c << std::endl;

	connectionSystem::connect(c, nodeList, nA, nB, pA, pB);
	nodeList[nB]->activate();
}

void nodeSystem::onNodesDisconnected(int nA, int nB, int pA, int pB, int c)
{
	std::cout << "[Node system] Nodes disconnected\n\tnodeA: " << nA << "\n\tnodeB: " << nB << "\n\tpinA: " << pA << "\n\tpinB: " << pB << "\n\tconnection: " << c << std::endl;
	connectionSystem::deleteConnection(c);

	recalculatePropagationMatrices();
	nodeList[nB]->activate();
}

bool nodeSystem::isConnectionValid(int nA, int nB, int pinA, int pinB)
{
	int nodeIndex, nodeSubIndex;
	go::nodePositionInMatrix(nodeList[nA], nodeList[nB]->getPropagationMatrix(), nodeIndex, nodeSubIndex);
	return
		nA != nB && // can't connect a node to itself
		nodeList[nA]->getPinType(pinA) == nodeList[nB]->getPinType(pinB) && // both pins must be of the same type
		nodeIndex == -1; // avoid cycles
		//(pinA < nodeList[nA]->getInputPinCount()) != (pinB < nodeList[nB]->getInputPinCount()) && // can't be both output or input (handled by the ui already)
}

const std::vector<void*>& nodeSystem::getDataPointersForNode(int n)
{
	return nodeList[n]->getDataPointers();
}
const int* nodeSystem::getPinTypesForNode(int n)
{
	return nodeList[n]->getPinTypes();
}
int nodeSystem::getOutputPinCountForNode(int n)
{
	return nodeList[n]->getOutputPinCount();
}
