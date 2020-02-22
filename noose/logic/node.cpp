#include "../math/graphOperations.h"
#include "../nodeData.h"
#include "../types.h"

#include "node.h"
#include "connectionSystem.h"

#include <iostream>
#include <cstring>

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
	case NS_TYPE_IMAGE:
		if (defaultValue == nullptr)
			return new sf::RenderTexture();
	}
}

node::node(const void* theNodeData)
{
	nodeData* data = (nodeData*)theNodeData;

	// set functionality
	m_nodeFunctionalityPointer = (void (*)(node* theNode))(data->nodeFunctionality);

	m_inputPinCount = data->inputPinCount;
	m_outputPinCount = data->outputPinCount;

	// get a copy of the types
	m_pinTypes = new int[m_inputPinCount + m_outputPinCount];
	std::memcpy(m_pinTypes, &(data->pinTypes[0]), sizeof(int) * (m_inputPinCount + m_outputPinCount));

	m_pinDataPointers.reserve(m_inputPinCount + m_outputPinCount); // so it doesn't have to reallocate at each iteration

	// data pointers from other nodes
	m_receivedDataPointers.reserve(m_inputPinCount);

	for (int i = 0; i < (m_inputPinCount + m_outputPinCount); i++)
	{
		m_pinDataPointers.push_back(reserveDataForPin(m_pinTypes[i], data->pinDefaultData[i]));
		if (i < m_inputPinCount)
			m_receivedDataPointers.push_back(nullptr);
	}
}

node::~node()
{
	std::cout << "deleting logical node\n";
	for (int i = 0; i < getPinCount(); i++)
	{
		delete m_pinDataPointers[i];
	}
	
	delete[] m_pinTypes;
}

/*
bool node::canConnectToPin(int pin)
{
	// cannot connect if its an input pin and is already connected
	return (pin >= m_inputPinCount || m_receivedDataPointers[pin] == nullptr);
}*/

void node::connect(int lineIndex)
{
	if (connectionSystem::connections[lineIndex].nodeA == (void*)this) // we are the left side node, ours is an output pin
	{
		go::updatePropagationMatrix(m_propagationMatrix, (node*)connectionSystem::connections[lineIndex].nodeB);
		for (node* n : m_leftSideNodes)
			n->propagateMatrix(m_propagationMatrix);

		std::cout << "fdsa\n";
	}
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
		go::removeNodeFromList((node*)connectionSystem::connections[lineIndex].nodeA, m_leftSideNodes);
	}
}

void node::activate()
{
	std::cout << "activating node\n";
	// execute node functionality
	run();

	// run every subsequent node
	for (std::vector<node*>& list : m_propagationMatrix)
	{
		for (node* n : list)
			n->run();
	}
}

void node::run()
{
	std::cout << "running node\n";
	m_nodeFunctionalityPointer(this);
}

const std::vector<std::vector<node*>>& node::getPropagationMatrix()
{
	return m_propagationMatrix;
}

void node::propagateMatrix(std::vector<std::vector<node*>>& m)
{
	//std::cout << "propagating to node " << this->title.getString().toAnsiString() << std::endl;

	// update propagation matrix
	go::matrixPropagation(m_propagationMatrix, m);

	// propagate back to connected nodes no the left side
	for (node* n : m_leftSideNodes)
		n->propagateMatrix(m_propagationMatrix);
}

void node::clearPropagationMatrix()
{
	for (std::vector<node*>& l : m_propagationMatrix)
		l.clear();
	m_propagationMatrix.clear();
}

void node::rebuildMatrices(int lineIndex)
{
	if (connectionSystem::connections[lineIndex].nodeA == (void*)this) // we are the left side node, ours is an output pin
	{
		go::updatePropagationMatrix(m_propagationMatrix, (node*)connectionSystem::connections[lineIndex].nodeB);
		for (node* n : m_leftSideNodes)
			n->propagateMatrix(m_propagationMatrix);
	}
}

int node::getPinType(int pinIndex)
{
	return m_pinTypes[pinIndex];
}

const int& node::getInputPinCount()
{
	return m_inputPinCount;
}

const int& node::getOutputPinCount()
{
	return m_outputPinCount;
}

int node::getPinCount()
{
	return m_inputPinCount + m_outputPinCount;
}

void** node::getDataPointers()
{
	return &(m_pinDataPointers[0]);
}

void* node::getDataPointer(int pinIndex, bool acceptReceivedPointers)
{
	if (!acceptReceivedPointers)
		return m_pinDataPointers[pinIndex];

	if (pinIndex >= m_inputPinCount) // can't receive pointers from other nodes (is an output)
		return m_pinDataPointers[pinIndex];

	if (m_receivedDataPointers[pinIndex] != nullptr)
		return m_receivedDataPointers[pinIndex];

	return m_pinDataPointers[pinIndex];
}

sf::RenderTexture* node::getFirstOutputImage()
{
	for (int i = m_inputPinCount; i < m_inputPinCount + m_outputPinCount; i++)
	{
		if (m_pinTypes[i] == NS_TYPE_IMAGE)
			return (sf::RenderTexture*)getDataPointer(i, false);
	}
	return nullptr;
}