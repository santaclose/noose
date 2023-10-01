#include "nodeSystem.h"
#include "connectionSystem.h"
#include "node.h"
#include "graphOperations.h"
#include "../utils.h"
#include "../nodeData.h"
#include "../nodeProvider/nodeProvider.h"
#include <iostream>

namespace nodeSystem {

	std::vector<node*> nodeList;

	void insertNode(int slot, nodeData* data);
	void recalculatePropagationMatrices();
}

void nodeSystem::insertNode(int slot, nodeData* data)
{
	if (slot >= nodeList.size())
		nodeList.resize(slot + 1);

	nodeList[slot] = new node(data);
}

void nodeSystem::recalculatePropagationMatrices()
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

void nodeSystem::onNodesConnected(int nA, int nB, int pA, int pB, int c, bool activate)
{
	std::cout << "[Node system] Nodes connected\n\tnodeA: " << nA << "\n\tnodeB: " << nB << "\n\tpinA: " << pA << "\n\tpinB: " << pB << "\n\tconnection: " << c << std::endl;

	connectionSystem::connect(c, nodeList, nA, nB, pA, pB);
	if (activate)
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

void nodeSystem::clearEverything()
{
	nodeList.clear();
	connectionSystem::clearEverything();
}

#ifdef TEST
void* nodeSystem::getNodeList()
{
	return &nodeList;
}
#endif

const void* nodeSystem::getData(int n, int p, int& type)
{
	int targetNodeIndex = n < 0 ? (nodeList.size() + n) : n;
	type = nodeList[targetNodeIndex]->getPinType(p);
	return nodeList[targetNodeIndex]->getDataPointer(p, false);
}

// project loading
static int projectLoadingNodeCounter;
static int projectLoadingConnectionCounter;
void nodeSystem::onProjectFileLoadingStart()
{
	projectLoadingNodeCounter = 0;
	projectLoadingConnectionCounter = 0;
}
void nodeSystem::onProjectFileLoadingAddNode(const std::string& nodeName, float coordinatesX, float coordinatesY)
{
	const nodeData* dataForNewNode = nodeProvider::getNodeDataByName(nodeName);
	if (dataForNewNode == nullptr)
	{
		std::cout << "[Node system] Node not found for name: " + nodeName + "\n";
		return;
	}
	onNodeCreated(projectLoadingNodeCounter, dataForNewNode);
	projectLoadingNodeCounter++;
}
void nodeSystem::onProjectFileLoadingSetNodeInput(int nodeIndex, int pinIndex, void* data, int flags)
{
	int targetNodeIndex = nodeIndex < 0 ? (nodeList.size() + nodeIndex) : nodeIndex;
	switch (nodeList[targetNodeIndex]->getPinType(pinIndex))
	{
	case NS_TYPE_COLOR:
	{
		sf::Color* targetPointer = (sf::Color*)nodeList[targetNodeIndex]->getDataPointer(pinIndex, false);
		*targetPointer = *((sf::Color*)data);
		break;
	}
	case NS_TYPE_FLOAT:
	{
		float* targetPointer = (float*)nodeList[targetNodeIndex]->getDataPointer(pinIndex, false);
		*targetPointer = *((float*)data);
		break;
	}
	case NS_TYPE_FONT:
	{
		sf::Font* targetPointer = (sf::Font*)nodeList[targetNodeIndex]->getDataPointer(pinIndex, false);
		const std::string& filePath = *((const std::string*)data);
		if (!targetPointer->loadFromFile(filePath))
			std::cout << "[Node system] Failed to open font file\n";
		break;
	}
	case NS_TYPE_IMAGE:
	{
		sf::RenderTexture* targetPointer = (sf::RenderTexture*)nodeList[targetNodeIndex]->getDataPointer(pinIndex, false);
		if (flags == 0) // file path
			utils::drawImageToRenderTexture(*((std::string*)data), *targetPointer);
		else // in memory
			utils::drawImageToRenderTexture(*((sf::Image*)data), *targetPointer);
		break;
	}
	case NS_TYPE_INT:
	{
		int* targetPointer = (int*)nodeList[targetNodeIndex]->getDataPointer(pinIndex, false);
		*targetPointer = *((int*)data);
		break;
	}
	case NS_TYPE_STRING:
	{
		std::string* targetPointer = (std::string*)nodeList[targetNodeIndex]->getDataPointer(pinIndex, false);
		*targetPointer = *((std::string*)data);
		break;
	}
	case NS_TYPE_VECTOR2I:
	{
		sf::Vector2i* targetPointer = (sf::Vector2i*)nodeList[targetNodeIndex]->getDataPointer(pinIndex, false);
		*targetPointer = *((sf::Vector2i*)data);
		break;
	}
	}
}
void nodeSystem::onProjectFileLoadingAddConnection(int nodeAIndex, int pinAIndex, int nodeBIndex, int pinBIndex)
{
	onNodesConnected(nodeAIndex, nodeBIndex, pinAIndex, pinBIndex, projectLoadingConnectionCounter, false);
	projectLoadingConnectionCounter++;
}

void nodeSystem::onProjectFileLoadingFinish()
{
	for (node* n : nodeList)
	{
		if (n != nullptr && !n->isConnectedToTheLeft())
			n->activate();
	}
}