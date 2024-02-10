#include "../types.h"

#include "node.h"
#include "connectionSystem.h"

#include <iostream>
#include <cstring>

void deletePinData(int type, void* pointer)
{
	switch (type)
	{
	case NS_TYPE_INT:
		delete (int*)pointer;
		break;
	case NS_TYPE_FLOAT:
		delete (float*)pointer;
		break;
	case NS_TYPE_VECTOR2I:
		delete (sf::Vector2i*)pointer;
		break;
	case NS_TYPE_COLOR:
		delete (sf::Color*)pointer;
		break;
	case NS_TYPE_STRING:
		delete (std::string*)pointer;
		break;
	case NS_TYPE_IMAGE:
		delete (sf::RenderTexture*)pointer;
		break;
	case NS_TYPE_FONT:
		delete (sf::Font*)pointer;
		break;
	}
}

void* reserveDataForPin(int type, void* defaultValue)
{
	switch (type)
	{
	case NS_TYPE_INT:
		if (defaultValue == nullptr)
			return new int(0);
		else
			return new int(*((int*)defaultValue));
	case NS_TYPE_FLOAT:
		if (defaultValue == nullptr)
			return new float(0.0f);
		else
			return new float(*((float*)defaultValue));
	case NS_TYPE_VECTOR2I:
		if (defaultValue == nullptr)
			return new sf::Vector2i(0, 0);
		else
			return new sf::Vector2i(*((sf::Vector2i*)defaultValue));
	case NS_TYPE_COLOR:
		if (defaultValue == nullptr)
			return new sf::Color(255, 0, 255, 255);
		else
			return new sf::Color(*((sf::Color*)defaultValue));
	case NS_TYPE_STRING:
		if (defaultValue == nullptr)
			return new std::string("");
		else
			return new std::string(*((std::string*)defaultValue));
	case NS_TYPE_IMAGE:
		if (defaultValue == nullptr)
			return new sf::RenderTexture();
		break; // can't set default from nodes.dat
	case NS_TYPE_FONT:
		if (defaultValue == nullptr)
			return new sf::Font();
		break; // can't set default from nodes.dat
	}
	std::cout << "[Node system] COULD NOT RESERVE DATA FOR PIN\n";
	return nullptr;
}

void node::setDefaultValue(int pin, const void* defaultValue)
{
	int type = m_nodeData->pinTypes[pin];
	switch (type)
	{
	case NS_TYPE_INT:
		*((int*)m_pinDataPointers[pin]) = int(*((int*)defaultValue));
		return;
	case NS_TYPE_FLOAT:
		*((float*)m_pinDataPointers[pin]) = float(*((float*)defaultValue));
		return;
	case NS_TYPE_VECTOR2I:
		*((sf::Vector2i*)m_pinDataPointers[pin]) = sf::Vector2i(*((sf::Vector2i*)defaultValue));
		return;
	case NS_TYPE_COLOR:
		*((sf::Color*)m_pinDataPointers[pin]) = sf::Color(*((sf::Color*)defaultValue));
		return;
	case NS_TYPE_STRING:
		*((std::string*)m_pinDataPointers[pin]) = std::string(*((std::string*)defaultValue));
		return;
	case NS_TYPE_IMAGE:
		break; // can't set default from nodes.dat
	case NS_TYPE_FONT:
		break; // can't set default from nodes.dat
	}
	std::cout << "[Node system] COULD NOT SET DEFAULT DATA FOR PIN\n";
}

node::node(const nodeData* data)
{
	m_nodeData = data;
	m_pinDataPointers.resize(m_nodeData->inputPinCount + m_nodeData->outputPinCount);

	// data pointers from other nodes
	m_receivedDataPointers.resize(m_nodeData->inputPinCount);

	for (int i = 0; i < (m_nodeData->inputPinCount + m_nodeData->outputPinCount); i++)
	{
		m_pinDataPointers[i] = reserveDataForPin(m_nodeData->pinTypes[i], data->pinDefaultData[i]);
		if (i < m_nodeData->inputPinCount)
			m_receivedDataPointers[i] = nullptr;
	}
}

node::~node()
{
	//std::cout << "deleting logical node\n";
	for (int i = 0; i < getPinCount(); i++)
		deletePinData(m_nodeData->pinTypes[i], m_pinDataPointers[i]);
}

void node::connect(int lineIndex)
{
	if (connectionSystem::connections[lineIndex].nodeA == (void*)this) // we are the left side node, ours is an output pin
		m_rightSideNodes.push_back((node*)(connectionSystem::connections[lineIndex].nodeB));
	else // we are the right side node
	{
		m_receivedDataPointers[connectionSystem::connections[lineIndex].pinB] =
			((node*)(connectionSystem::connections[lineIndex].nodeA))->getDataPointer(connectionSystem::connections[lineIndex].pinA, false);
		m_leftSideNodes.push_back((node*)(connectionSystem::connections[lineIndex].nodeA));
	}
}

void node::disconnect(int lineIndex)
{
	if (connectionSystem::connections[lineIndex].nodeA != (void*)this) // we are the right side node, ours is an input pin
	{
		m_receivedDataPointers[connectionSystem::connections[lineIndex].pinB] = nullptr;
		removeFromList((node*)connectionSystem::connections[lineIndex].nodeA, m_leftSideNodes);
	}
	else
		removeFromList((node*)connectionSystem::connections[lineIndex].nodeB, m_rightSideNodes);
}

static std::vector<node*> activationQueue;
void node::activate()
{
	activationQueue.clear();
	int currentQueueIndex = 0;
	node* currentNode;
	activationQueue.push_back(this);
	while (currentQueueIndex < activationQueue.size())
	{
		currentNode = activationQueue[currentQueueIndex];
		currentQueueIndex++;
		currentNode->run();
		for (node* n : currentNode->m_rightSideNodes)
			activationQueue.push_back(n);
	}
}

void node::run()
{
	//std::cout << "running node\n";
	((void (*)(node * theNode))(m_nodeData->nodeFunctionality))(this); // execute function pointer with this node as the argument
}

int node::getPinType(int pinIndex)
{
	return m_nodeData->pinTypes[pinIndex];
}

int node::getInputPinCount()
{
	return m_nodeData->inputPinCount;
}

int node::getOutputPinCount()
{
	return m_nodeData->outputPinCount;
}

int node::getPinCount()
{
	return m_nodeData->inputPinCount + m_nodeData->outputPinCount;
}


const std::vector<void*>& node::getDataPointers()
{
	return m_pinDataPointers;
}

const int* node::getPinTypes()
{
	return m_nodeData->pinTypes.data();
}

void* node::getDataPointer(int pinIndex, bool acceptReceivedPointers)
{
	if (!acceptReceivedPointers)
		return m_pinDataPointers[pinIndex];

	if (pinIndex >= m_nodeData->inputPinCount) // can't receive pointers from other nodes (is an output)
		return m_pinDataPointers[pinIndex];

	if (m_receivedDataPointers[pinIndex] != nullptr)
		return m_receivedDataPointers[pinIndex];

	return m_pinDataPointers[pinIndex];
}

bool node::findNodeToTheRightRecursive(const node* toFind) const
{
	for (const node* n : m_rightSideNodes)
	{
		if (n == toFind || n->findNodeToTheRightRecursive(toFind))
			return true;
	}
	return false;
}
