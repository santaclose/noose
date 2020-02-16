#include "logicalConnections.h"

std::vector<logicalConnection> logicalConnections::connections;

/*
int logicalConnections::tryToConnect(logicalNode* nodeA, int& pinA, logicalNode* nodeB, int& pinB, int& nodeIndexA, int& nodeIndexB)
{
	// some checks
	if (nodeA == nodeB || // both pins are in the same node
		(pinA < nodeA->getInputPinCount()) == (pinB < nodeB->getInputPinCount()) || // both are output or input
		nodeA->getPinType(pinA) != nodeB->getPinType(pinB)) // not the same type (compare colors)
	{
		return -1;
	}

	// connect
	//    find solt
	int newLineIndex = 0;
	for (const logicalConnection& c : connections)
	{
		if (c.deleted)
			break;
		newLineIndex++;
	}
	//    insert in list
	bool flipped = pinA < nodeA->getInputPinCount(); // pin a is an input pin, need to flip
	if (newLineIndex < connections.size())
	{
		connections[newLineIndex].deleted = false;
		connections[newLineIndex].nodeA = flipped ? nodeB : nodeA;
		connections[newLineIndex].pinA = flipped ? pinB : pinA;
		connections[newLineIndex].nodeB = flipped ? nodeA : nodeB;
		connections[newLineIndex].pinB = flipped ? pinA : pinB;
		connections[newLineIndex].nodeIndexA = flipped ? nodeIndexB : nodeIndexA;
		connections[newLineIndex].nodeIndexB = flipped ? nodeIndexA : nodeIndexB;
	}
	else
	{
		connections.emplace_back(
			flipped ? nodeB : nodeA,
			flipped ? pinB : pinA,
			flipped ? nodeA : nodeB,
			flipped ? pinA : pinB,
			flipped ? nodeIndexB : nodeIndexA,
			flipped ? nodeIndexA : nodeIndexB);
	}
	if (flipped)
	{
		// return arguments flipped
		int temp = nodeIndexB;
		nodeIndexB = nodeIndexA;
		nodeIndexA = temp;
		temp = pinB;
		pinB = pinA;
		pinA = temp;
	}

	return newLineIndex;
}*/

/*int logicalConnections::connect(logicalNode* nA, int& pinA, logicalNode* nB, int& pinB, int& nIndexA, int& nIndexB)
{
	int newLineIndex = 0;
	for (const logicalConnection& c : connections)
	{
		if (c.deleted)
			break;
		newLineIndex++;
	}
	//    insert in list
	bool flipped = pinA < nA->getInputPinCount(); // pin a is an input pin, need to flip
	if (newLineIndex < connections.size())
	{
		connections[newLineIndex].deleted = false;
		connections[newLineIndex].nodeA = flipped ? nB : nA;
		connections[newLineIndex].pinA = flipped ? pinB : pinA;
		connections[newLineIndex].nodeB = flipped ? nA : nB;
		connections[newLineIndex].pinB = flipped ? pinA : pinB;
		connections[newLineIndex].nodeIndexA = flipped ? nIndexB : nIndexA;
		connections[newLineIndex].nodeIndexB = flipped ? nIndexA : nIndexB;
	}
	else
	{
		connections.emplace_back(
			flipped ? nB : nA,
			flipped ? pinB : pinA,
			flipped ? nA : nB,
			flipped ? pinA : pinB,
			flipped ? nIndexB : nIndexA,
			flipped ? nIndexA : nIndexB);
	}
	if (flipped)
	{
		// return arguments flipped
		int temp = nIndexB;
		nIndexB = nIndexA;
		nIndexA = temp;
		temp = pinB;
		pinB = pinA;
		pinA = temp;
	}

	return newLineIndex;
}*/

void logicalConnections::connect(int connectionIndex, const std::vector<logicalNode*>& nodeList, int nodeA, int nodeB, int pinA, int pinB)
{
	if (connectionIndex >= connections.size())
		connections.resize(connectionIndex + 1);
	connections[connectionIndex].nodeA = nodeList[nodeA];
	connections[connectionIndex].nodeB = nodeList[nodeB];
	connections[connectionIndex].nodeIndexA = nodeA;
	connections[connectionIndex].nodeIndexB = nodeB;
	connections[connectionIndex].pinA = pinA;
	connections[connectionIndex].pinB = pinB;
	connections[connectionIndex].deleted = false;
}

void logicalConnections::deleteConnection(int lineIndex)
{
	connections[lineIndex].deleted = true;
}
