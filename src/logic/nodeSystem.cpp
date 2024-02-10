#include "nodeSystem.h"
#include "connectionSystem.h"
#include "node.h"
#include "../utils.h"
#include "../nodeData.h"
#include "../serializer.h"
#include "../nodeProvider/nodeProvider.h"
#include <iostream>

namespace nodeSystem {
	struct nodePinPair { int n, p; };
	struct customNodeData
	{
		const nodeData* nodeInterfaceData;
		int subgraphNodeCount;
		std::vector<nodePinPair> bindings;
		std::vector<int> subgraphConnections;
		std::vector<void*> dataPointers;
		void initializeVectors()
		{
			bindings.resize(nodeInterfaceData->inputPinCount + nodeInterfaceData->outputPinCount, { -1, -1 });
			dataPointers.resize(nodeInterfaceData->inputPinCount + nodeInterfaceData->outputPinCount, nullptr);
		}
	};

	std::vector<node*> nodeList;

	std::unordered_map<int, customNodeData> customNodes;
	std::vector<int> loadCustomNodeBaseIndexStack;
}

void nodeSystem::initialize()
{
	std::cout << "[Node system] Initializing\n";
	nodeList.reserve(64);
	loadCustomNodeBaseIndexStack.reserve(8);
	connectionSystem::initialize();
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

int nodeSystem::onNodeCreated(const void* data)
{
	//std::cout << "[Node system] Node created\n\tid: " << n << std::endl;

	nodeData* nd = (nodeData*)data;
	int newNodeId = nodeList.size();

	if (nd->customNodeId == -1)
		nodeList.push_back(new node(nd));
	else
	{
		serializer::ParsingCallbacks parsingCallbacks;
		parsingCallbacks.OnStartParsing = nodeSystem::onCustomNodeFileStartParsing;
		parsingCallbacks.OnParseNode = nodeSystem::onCustomNodeFileParseNode;
		parsingCallbacks.OnParseNodeInput = nodeSystem::onCustomNodeFileParseNodeInput;
		parsingCallbacks.OnParseConnection = nodeSystem::onCustomNodeFileParseConnection;
		parsingCallbacks.OnParseCustomNodeInput = nodeSystem::onCustomNodeFileParseCustomNodeInput;
		parsingCallbacks.OnParseCustomNodeOutput = nodeSystem::onCustomNodeFileParseCustomNodeOutput;
		parsingCallbacks.OnFinishParsing = nodeSystem::onCustomNodeFileFinishParsing;
		customNodes[newNodeId].nodeInterfaceData = nd;
		customNodes[newNodeId].initializeVectors();
		serializer::LoadFromFile(nodeProvider::getCustomNodeFilePath(nd->customNodeId), parsingCallbacks);
	}

	return newNodeId;
}

void nodeSystem::onNodeDeleted(int n, const std::vector<int>& connections)//int* ci, int cc)
{
	//std::cout << "[Node system] Node deleted\n\tid: " << n << std::endl;

	// delete all connections to the node
	for (int c : connections)
		connectionSystem::deleteConnection(c);

	if (customNodes.find(n) != customNodes.end()) // is custom node
	{
		for (int c : customNodes[n].subgraphConnections)
			connectionSystem::deleteConnection(c);
		for (int i = n + customNodes[n].subgraphNodeCount - 1; i > n - 1; i--)
		{
			delete nodeList[i];
			nodeList[i] = nullptr;
		}
		customNodes.erase(n);
	}
	else // is normal node
	{
		delete nodeList[n];
		nodeList[n] = nullptr;
	}
}

void nodeSystem::onNodeChanged(int n, int p)
{
	//std::cout << "[Node system] Node changed\n\tid: " << n << std::endl;
	if (customNodes.find(n) != customNodes.end() && customNodes[n].bindings[p].n > -1)
		nodeList[customNodes[n].bindings[p].n]->activate();
	else
		nodeList[n]->activate();
}

int nodeSystem::onNodesConnected(int nA, int nB, int pA, int pB, bool activateNodeB)
{
	//std::cout << "[Node system] Nodes connected\n\tnodeA: " << nA << "\n\tnodeB: " << nB << "\n\tpinA: " << pA << "\n\tpinB: " << pB << "\n\tconnection: " << c << std::endl;
	if (customNodes.find(nA) != customNodes.end() && customNodes[nA].bindings[pA].n > -1)
	{
		nodePinPair temp = customNodes[nA].bindings[pA];
		nA = temp.n;
		pA = temp.p;
	}
	if (customNodes.find(nB) != customNodes.end() && customNodes[nB].bindings[pB].n > -1)
	{
		nodePinPair temp = customNodes[nB].bindings[pB];
		nB = temp.n;
		pB = temp.p;
	}
	int connectionIndex = connectionSystem::connect(nodeList, nA, nB, pA, pB);
	if (activateNodeB)
		nodeList[nB]->activate();
	return connectionIndex;
}

void nodeSystem::onNodesDisconnected(int c)
{
	//std::cout << "[Node system] Nodes disconnected\n\tnodeA: " << nA << "\n\tnodeB: " << nB << "\n\tpinA: " << pA << "\n\tpinB: " << pB << "\n\tconnection: " << c << std::endl;
	int nB = connectionSystem::connections[c].nodeIndexB;
	int pB = connectionSystem::connections[c].pinB;
	connectionSystem::deleteConnection(c);

	if (customNodes.find(nB) != customNodes.end() && customNodes[nB].bindings[pB].n > -1)
		nodeList[customNodes[nB].bindings[pB].n]->activate();
	else
		nodeList[nB]->activate();
}

bool nodeSystem::isConnectionValid(int nA, int nB, int pinA, int pinB)
{
	int typeA = customNodes.find(nA) == customNodes.end() ?
		nodeList[nA]->getPinType(pinA) :
		customNodes[nA].nodeInterfaceData->pinTypes[pinA];
	int typeB = customNodes.find(nB) == customNodes.end() ?
		nodeList[nB]->getPinType(pinB) :
		customNodes[nB].nodeInterfaceData->pinTypes[pinB];

	bool createsCycle = nodeList[nB]->findNodeToTheRightRecursive(nodeList[nA]); // assume subgraph is fully connected so doesn't matter from which node we look
	return
		nA != nB && // can't connect a node to itself
		typeA == typeB && // both pins must be of the same type
		!createsCycle; // avoid cycles
	//(pinA < nodeList[nA]->getInputPinCount()) != (pinB < nodeList[nB]->getInputPinCount()) && // can't be both output or input (handled by the ui already)
}

const std::vector<void*>& nodeSystem::getDataPointersForNode(int n)
{
	if (customNodes.find(n) != customNodes.end())
		return customNodes[n].dataPointers;
	return nodeList[n]->getDataPointers();
}
const int* nodeSystem::getPinTypesForNode(int n)
{
	if (customNodes.find(n) != customNodes.end())
		return customNodes[n].nodeInterfaceData->pinTypes.data();
	return nodeList[n]->getPinTypes();
}
int nodeSystem::getOutputPinCountForNode(int n)
{
	if (customNodes.find(n) != customNodes.end())
		return customNodes[n].nodeInterfaceData->outputPinCount;
	return nodeList[n]->getOutputPinCount();
}

void nodeSystem::clearEverything()
{
	nodeList.clear();
	connectionSystem::clearEverything();

	customNodes.clear();
}

#ifdef TEST
void* nodeSystem::getNodeList()
{
	return &nodeList;
}
#endif

const void* nodeSystem::getData(int n, int p, int& type)
{
	if (customNodes.find(n) != customNodes.end() && customNodes[n].bindings[p].n > -1)
	{
		nodePinPair temp = customNodes[n].bindings[p];
		n = temp.n;
		p = temp.p;
	}

	int targetNodeIndex = n < 0 ? (nodeList.size() + n) : n;
	type = nodeList[targetNodeIndex]->getPinType(p);
	return nodeList[targetNodeIndex]->getDataPointer(p, false);
}

// ---------------
// project loading
// ---------------

void nodeSystem::onProjectFileParseNode(const std::string& nodeName, float coordinatesX, float coordinatesY)
{
	const nodeData* dataForNewNode = nodeProvider::getNodeDataByName(nodeName);
	if (dataForNewNode == nullptr)
	{
		std::cout << "[Node system] Node not found for name: " + nodeName + "\n";
		return;
	}
	onNodeCreated(dataForNewNode);
}
void nodeSystem::onProjectFileParseNodeInput(int nodeIndex, int pinIndex, void* data, int flags)
{
	switch (nodeList[nodeIndex]->getPinType(pinIndex))
	{
	case NS_TYPE_COLOR:
	{
		sf::Color* targetPointer = (sf::Color*)nodeList[nodeIndex]->getDataPointer(pinIndex, false);
		*targetPointer = *((sf::Color*)data);
		break;
	}
	case NS_TYPE_FLOAT:
	{
		float* targetPointer = (float*)nodeList[nodeIndex]->getDataPointer(pinIndex, false);
		*targetPointer = *((float*)data);
		break;
	}
	case NS_TYPE_FONT:
	{
		sf::Font* targetPointer = (sf::Font*)nodeList[nodeIndex]->getDataPointer(pinIndex, false);
		const std::string& filePath = *((const std::string*)data);
		if (!targetPointer->loadFromFile(filePath))
			std::cout << "[Node system] Failed to open font file\n";
		break;
	}
	case NS_TYPE_IMAGE:
	{
		sf::RenderTexture* targetPointer = (sf::RenderTexture*)nodeList[nodeIndex]->getDataPointer(pinIndex, false);
		if (flags == 0) // file path
			utils::drawImageToRenderTexture(*((std::string*)data), *targetPointer);
		else // in memory
			utils::drawImageToRenderTexture(*((sf::Image*)data), *targetPointer);
		break;
	}
	case NS_TYPE_INT:
	{
		int* targetPointer = (int*)nodeList[nodeIndex]->getDataPointer(pinIndex, false);
		*targetPointer = *((int*)data);
		break;
	}
	case NS_TYPE_STRING:
	{
		std::string* targetPointer = (std::string*)nodeList[nodeIndex]->getDataPointer(pinIndex, false);
		*targetPointer = *((std::string*)data);
		break;
	}
	case NS_TYPE_VECTOR2I:
	{
		sf::Vector2i* targetPointer = (sf::Vector2i*)nodeList[nodeIndex]->getDataPointer(pinIndex, false);
		*targetPointer = *((sf::Vector2i*)data);
		break;
	}
	}
}
void nodeSystem::onProjectFileParseConnection(int nodeAIndex, int pinAIndex, int nodeBIndex, int pinBIndex)
{
	int newConnection = onNodesConnected(nodeAIndex, nodeBIndex, pinAIndex, pinBIndex, false);
}

void nodeSystem::onProjectFileFinishParsing()
{
	for (node* n : nodeList)
	{
		if (n != nullptr && !n->isConnectedToTheLeft())
			n->activate();
	}
}

// ---------------
// custom nodes
// ---------------

void nodeSystem::onCustomNodeFileStartParsing()
{
	loadCustomNodeBaseIndexStack.push_back(nodeList.size());
}

void nodeSystem::onCustomNodeFileParseNode(const std::string& nodeName, float coordinatesX, float coordinatesY)
{
	onProjectFileParseNode(nodeName, coordinatesX, coordinatesY);
}

void nodeSystem::onCustomNodeFileParseNodeInput(int nodeIndex, int pinIndex, void* data, int flags)
{
	onProjectFileParseNodeInput(nodeIndex += nodeIndex > -1 ? loadCustomNodeBaseIndexStack.back() : nodeList.size(), pinIndex, data, flags);
}

void nodeSystem::onCustomNodeFileParseConnection(int nodeAIndex, int pinAIndex, int nodeBIndex, int pinBIndex)
{
	int customNodeBase = loadCustomNodeBaseIndexStack.back();
	nodeAIndex += nodeAIndex > -1 ? customNodeBase : nodeList.size();
	nodeBIndex += nodeBIndex > -1 ? customNodeBase : nodeList.size();

	int connectionIndex = connectionSystem::connect(nodeList, nodeAIndex, nodeBIndex, pinAIndex, pinBIndex);
	customNodes[customNodeBase].subgraphConnections.push_back(connectionIndex);
}

void nodeSystem::onCustomNodeFileParseCustomNodeInput(int inPin, int subgraphNode, int subgraphPin)
{
	int customNodeBase = loadCustomNodeBaseIndexStack.back();
	subgraphNode += subgraphNode > -1 ? customNodeBase : nodeList.size();
	customNodes[customNodeBase].bindings[inPin] = { subgraphNode, subgraphPin };
}

void nodeSystem::onCustomNodeFileParseCustomNodeOutput(int outPin, int subgraphNode, int subgraphPin)
{
	int customNodeBase = loadCustomNodeBaseIndexStack.back();
	subgraphNode += subgraphNode > -1 ? customNodeBase : nodeList.size();
	customNodes[customNodeBase].bindings[outPin] = { subgraphNode, subgraphPin };
}

void nodeSystem::onCustomNodeFileFinishParsing()
{
	int customNodeBase = loadCustomNodeBaseIndexStack.back();
	int inputCount = customNodes[customNodeBase].nodeInterfaceData->inputPinCount;
	int outputCount = customNodes[customNodeBase].nodeInterfaceData->outputPinCount;
	for (int i = 0; i < inputCount; i++)
	{
		node* subgraphNode = nodeList[customNodes[customNodeBase].bindings[i].n];
		int subgraphPin = customNodes[customNodeBase].bindings[i].p;
		void* subgraphPinDataPointer = subgraphNode->getDataPointer(subgraphPin, false);
		if (customNodes[customNodeBase].nodeInterfaceData->pinDefaultData[i] != nullptr)
			subgraphNode->setDefaultValue(subgraphPin, customNodes[customNodeBase].nodeInterfaceData->pinDefaultData[i]);
		customNodes[customNodeBase].dataPointers[i] = subgraphPinDataPointer;
	}
	for (int i = 0; i < outputCount; i++)
		customNodes[customNodeBase].dataPointers[inputCount + i] =
		nodeList[customNodes[customNodeBase].bindings[inputCount + i].n]->getDataPointer(
			customNodes[customNodeBase].bindings[inputCount + i].p, false);
	customNodes[customNodeBase].subgraphNodeCount = nodeList.size() - customNodeBase;
	loadCustomNodeBaseIndexStack.pop_back();
}
