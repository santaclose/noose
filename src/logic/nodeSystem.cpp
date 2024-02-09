#include "nodeSystem.h"
#include "connectionSystem.h"
#include "node.h"
#include "../utils.h"
#include "../nodeData.h"
#include "../serializer.h"
#include "../nodeProvider/nodeProvider.h"
#include <iostream>
#include <functional>

namespace nodeSystem {
	struct nodePinPair
	{
		int n, p;
		bool operator==(const nodePinPair& other) const { return other.n == n && other.p == p; }
		bool operator<(const nodePinPair& other) const { if (other.n == n) return other.p < p; return other.n < n; }
	};
	struct nodePinPairHasher
	{
		std::size_t operator()(const nodePinPair& t) const noexcept
		{
			return (std::hash<int>()(t.n)
				^ (std::hash<int>()(t.p) << 1));
		}
	};

	std::vector<node*> nodeList;

	std::unordered_map<nodePinPair, nodePinPair, nodePinPairHasher> customNodeInputBindings;
	std::unordered_map<nodePinPair, nodePinPair, nodePinPairHasher> customNodeOutputBindings;
	std::unordered_map<int, std::vector<int>> customNodeConnections;
	std::unordered_map<int, const nodeData* const> customNodeData;
	std::unordered_map<int, std::vector<void*>> customNodeDataPointers;
	std::unordered_map<int, int> customNodeIntervals;
	int lastParsedCustomNodePin;
	int loadCustomNodeBaseIndex = 0;
}

void nodeSystem::initialize()
{
	std::cout << "[Node system] Initializing\n";
	nodeList.reserve(64);
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
		parsingCallbacks.OnParseNode = nodeSystem::onProjectFileLoadingAddNode;
		parsingCallbacks.OnParseNodeInput = nodeSystem::onProjectFileLoadingSetNodeInput;
		parsingCallbacks.OnParseConnection = nodeSystem::onReadCustomNodeConnection;
		parsingCallbacks.OnParseCustomNodeInput = nodeSystem::onReadCustomNodeInput;
		parsingCallbacks.OnParseCustomNodeOutput = nodeSystem::onReadCustomNodeOutput;
		parsingCallbacks.OnFinishParsing = nodeSystem::onFinishParsingCustomNode;
		loadCustomNodeBaseIndex = nodeList.size();
		lastParsedCustomNodePin = -1;
		customNodeData.insert({ newNodeId , nd });
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

	if (customNodeConnections.find(n) != customNodeConnections.end()) // is custom node
	{
		for (int c : customNodeConnections[n])
			connectionSystem::deleteConnection(c);
		int inputCount = customNodeData[n]->inputPinCount;
		int outputCount = customNodeData[n]->outputPinCount;
		for (int i = 0; i < inputCount; i++)
			customNodeInputBindings.erase({ n, i });
		for (int i = 0; i < outputCount; i++)
			customNodeOutputBindings.erase({ n, inputCount + i });
		customNodeData.erase(n);
		customNodeDataPointers.erase(n);
		for (int i = customNodeIntervals[n] - 1; i > n - 1; i--)
		{
			delete nodeList[i];
			nodeList[i] = nullptr;
		}
		customNodeIntervals.erase(n);
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
	if (customNodeInputBindings.find({ n, p }) != customNodeInputBindings.end())
		nodeList[customNodeInputBindings[{n, p}].n]->activate();
	else
		nodeList[n]->activate();
}

int nodeSystem::onNodesConnected(int nA, int nB, int pA, int pB, bool activateNodeB)
{
	//std::cout << "[Node system] Nodes connected\n\tnodeA: " << nA << "\n\tnodeB: " << nB << "\n\tpinA: " << pA << "\n\tpinB: " << pB << "\n\tconnection: " << c << std::endl;
	if (customNodeOutputBindings.find({ nA, pA }) != customNodeOutputBindings.end())
	{
		nodePinPair temp = customNodeOutputBindings[{nA, pA}];
		nA = temp.n;
		pA = temp.p;
	}
	if (customNodeInputBindings.find({ nB, pB }) != customNodeInputBindings.end())
	{
		nodePinPair temp = customNodeInputBindings[{nB, pB}];
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

	if (customNodeInputBindings.find({ nB, pB }) != customNodeInputBindings.end())
		nodeList[customNodeInputBindings[{nB, pB}].n]->activate();
	else
		nodeList[nB]->activate();
}

bool nodeSystem::isConnectionValid(int nA, int nB, int pinA, int pinB)
{
	int typeA = customNodeData.find(nA) == customNodeData.end() ?
		nodeList[nA]->getPinType(pinA) :
		customNodeData[nA]->pinTypes[pinA];
	int typeB = customNodeData.find(nB) == customNodeData.end() ?
		nodeList[nB]->getPinType(pinB) :
		customNodeData[nB]->pinTypes[pinB];

	bool createsCycle = nodeList[nB]->findNodeToTheRightRecursive(nodeList[nA]); // assume subgraph is fully connected so doesn't matter from which node we look
	return
		nA != nB && // can't connect a node to itself
		typeA == typeB && // both pins must be of the same type
		!createsCycle; // avoid cycles
	//(pinA < nodeList[nA]->getInputPinCount()) != (pinB < nodeList[nB]->getInputPinCount()) && // can't be both output or input (handled by the ui already)
}

const std::vector<void*>& nodeSystem::getDataPointersForNode(int n)
{
	if (customNodeDataPointers.find(n) != customNodeDataPointers.end())
		return customNodeDataPointers[n];
	return nodeList[n]->getDataPointers();
}
const int* nodeSystem::getPinTypesForNode(int n)
{
	if (customNodeData.find(n) != customNodeData.end())
		return customNodeData[n]->pinTypes.data();
	return nodeList[n]->getPinTypes();
}
int nodeSystem::getOutputPinCountForNode(int n)
{
	if (customNodeData.find(n) != customNodeData.end())
		return customNodeData[n]->outputPinCount;
	return nodeList[n]->getOutputPinCount();
}

void nodeSystem::clearEverything()
{
	nodeList.clear();
	connectionSystem::clearEverything();

	customNodeConnections.clear();
	customNodeData.clear();
	customNodeDataPointers.clear();
	customNodeInputBindings.clear();
	customNodeOutputBindings.clear();
}

#ifdef TEST
void* nodeSystem::getNodeList()
{
	return &nodeList;
}
#endif

const void* nodeSystem::getData(int n, int p, int& type)
{
	if (customNodeInputBindings.find({ n, p }) != customNodeInputBindings.end())
	{
		nodePinPair temp = customNodeInputBindings[{n, p}];
		n = temp.n;
		p = temp.p;
	}
	if (customNodeOutputBindings.find({ n, p }) != customNodeOutputBindings.end())
	{
		nodePinPair temp = customNodeOutputBindings[{n, p}];
		n = temp.n;
		p = temp.p;
	}

	int targetNodeIndex = n < 0 ? (nodeList.size() + n) : n;
	type = nodeList[targetNodeIndex]->getPinType(p);
	return nodeList[targetNodeIndex]->getDataPointer(p, false);
}

// ---------------
// project loading
void nodeSystem::onProjectFileLoadingAddNode(const std::string& nodeName, float coordinatesX, float coordinatesY)
{
	const nodeData* dataForNewNode = nodeProvider::getNodeDataByName(nodeName);
	if (dataForNewNode == nullptr)
	{
		std::cout << "[Node system] Node not found for name: " + nodeName + "\n";
		return;
	}
	onNodeCreated(dataForNewNode);
}
void nodeSystem::onProjectFileLoadingSetNodeInput(int nodeIndex, int pinIndex, void* data, int flags)
{
	nodeIndex += nodeIndex > -1 ? loadCustomNodeBaseIndex : nodeList.size();
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
void nodeSystem::onProjectFileLoadingAddConnection(int nodeAIndex, int pinAIndex, int nodeBIndex, int pinBIndex)
{
	nodeAIndex += nodeAIndex > -1 ? loadCustomNodeBaseIndex : nodeList.size();
	nodeBIndex += nodeBIndex > -1 ? loadCustomNodeBaseIndex : nodeList.size();
	int newConnection = onNodesConnected(nodeAIndex, nodeBIndex, pinAIndex, pinBIndex, false);
}

void nodeSystem::onProjectFileLoadingFinish()
{
	for (node* n : nodeList)
	{
		if (n != nullptr && !n->isConnectedToTheLeft())
			n->activate();
	}
}

void nodeSystem::onReadCustomNodeConnection(int nodeAIndex, int pinAIndex, int nodeBIndex, int pinBIndex)
{
	nodeAIndex += nodeAIndex > -1 ? loadCustomNodeBaseIndex : nodeList.size();
	nodeBIndex += nodeBIndex > -1 ? loadCustomNodeBaseIndex : nodeList.size();

	int connectionIndex = connectionSystem::connect(nodeList, nodeAIndex, nodeBIndex, pinAIndex, pinBIndex);
	customNodeConnections[loadCustomNodeBaseIndex].push_back(connectionIndex);
}

void nodeSystem::onReadCustomNodeInput(int inPin, int subgraphNode, int subgraphPin)
{
	subgraphNode += subgraphNode > -1 ? loadCustomNodeBaseIndex : nodeList.size();
	customNodeInputBindings.insert({ {loadCustomNodeBaseIndex, inPin}, {subgraphNode, subgraphPin} });
}

void nodeSystem::onReadCustomNodeOutput(int outPin, int subgraphNode, int subgraphPin)
{
	subgraphNode += subgraphNode > -1 ? loadCustomNodeBaseIndex : nodeList.size();
	customNodeOutputBindings.insert({ {loadCustomNodeBaseIndex, outPin}, {subgraphNode, subgraphPin} });
}

void nodeSystem::onFinishParsingCustomNode()
{
	int inputCount = customNodeData[loadCustomNodeBaseIndex]->inputPinCount;
	int outputCount = customNodeData[loadCustomNodeBaseIndex]->outputPinCount;
	for (int i = 0; i < inputCount; i++)
	{
		node* subgraphNode = nodeList[customNodeInputBindings[{loadCustomNodeBaseIndex, i}].n];
		int subgraphPin = customNodeInputBindings[{loadCustomNodeBaseIndex, i}].p;
		void* subgraphPinDataPointer = subgraphNode->getDataPointer(subgraphPin, false);
		if (customNodeData[loadCustomNodeBaseIndex]->pinDefaultData[i] != nullptr)
			subgraphNode->setDefaultValue(subgraphPin, customNodeData[loadCustomNodeBaseIndex]->pinDefaultData[i]);
		customNodeDataPointers[loadCustomNodeBaseIndex].push_back(subgraphPinDataPointer);
	}
	for (int i = 0; i < outputCount; i++)
		customNodeDataPointers[loadCustomNodeBaseIndex].push_back(
			nodeList[customNodeOutputBindings[{loadCustomNodeBaseIndex, inputCount + i}].n]->getDataPointer(
				customNodeOutputBindings[{loadCustomNodeBaseIndex, inputCount + i}].p, false)
		);
	customNodeIntervals.insert({ loadCustomNodeBaseIndex, nodeList.size() });
}
