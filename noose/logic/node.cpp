#include "../types.h"

#include "graphOperations.h"
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

node::node(const nodeData* data)
{
	// set functionality
	m_nodeFunctionalityPointer = (void (*)(node* theNode))(data->nodeFunctionality);

	m_inputPinCount = data->inputPinCount;
	m_outputPinCount = data->outputPinCount;

	// get a copy of the types
	m_pinTypes = new int[m_inputPinCount + m_outputPinCount];
	std::memcpy(m_pinTypes, &(data->pinTypes[0]), sizeof(int) * (m_inputPinCount + m_outputPinCount));

	// so it doesn't have to reallocate at each iteration
	m_pinDataPointers.resize(m_inputPinCount + m_outputPinCount);

	// data pointers from other nodes
	m_receivedDataPointers.resize(m_inputPinCount);

	for (int i = 0; i < (m_inputPinCount + m_outputPinCount); i++)
	{
		m_pinDataPointers[i] = reserveDataForPin(m_pinTypes[i], data->pinDefaultData[i]);
		if (i < m_inputPinCount)
			m_receivedDataPointers[i] = nullptr;
	}
}

node::~node()
{
	//std::cout << "deleting logical node\n";
	for (int i = 0; i < getPinCount(); i++)
	{
		delete m_pinDataPointers[i];
	}
	
	delete[] m_pinTypes;
}

void node::connect(int lineIndex)
{
	if (connectionSystem::connections[lineIndex].nodeA == (void*)this) // we are the left side node, ours is an output pin
	{
		go::updatePropagationMatrix(m_propagationMatrix, (node*)connectionSystem::connections[lineIndex].nodeB);
		for (node* n : m_leftSideNodes)
			n->propagateMatrix(m_propagationMatrix);
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
	//std::cout << "activating node\n";
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
	//std::cout << "running node\n";
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

int node::getInputPinCount()
{
	return m_inputPinCount;
}

int node::getOutputPinCount()
{
	return m_outputPinCount;
}

int node::getPinCount()
{
	return m_inputPinCount + m_outputPinCount;
}


const std::vector<void*>& node::getDataPointers()
{
	return m_pinDataPointers;
}

const int* node::getPinTypes()
{
	return m_pinTypes;
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

void node::print()
{
	std::cout << "\tm_inputPinCount: " << m_inputPinCount << std::endl;
	std::cout << "\tm_outputPinCount: " << m_outputPinCount << std::endl;

	std::cout << "\tm_pinTypes: ";
	for (int i = 0; i < m_inputPinCount + m_outputPinCount; i++)
	{
		std::cout << m_pinTypes[i] << ",";
	}
	std::cout << std::endl;

	std::cout << "\tm_pinDataPointers: ";
	for (void* p : m_pinDataPointers)
	{
		std::cout << p << ",";
	}
	std::cout << std::endl;

	std::cout << "\tm_receivedDataPointers: ";
	for (void* p : m_receivedDataPointers)
	{
		std::cout << p << ",";
	}
	std::cout << std::endl;

	std::cout << "\tm_propagationMatrix: ";
	for (std::vector<node*>& l : m_propagationMatrix)
	{
		for (node* n : l)
			std::cout << n << ",";
		std::cout << ";";
	}
	std::cout << std::endl;

	std::cout << "\tm_leftSideNodes: ";
	for (node* n : m_leftSideNodes)
	{
		std::cout << n << ",";
	}
	std::cout << std::endl;

	std::cout << "\tm_nodeFunctionalityPointer: " << m_nodeFunctionalityPointer << std::endl;
}
