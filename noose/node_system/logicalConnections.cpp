#include "logicalConnections.h"

std::vector<logicalConnection> logicalConnections::connections;

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
}

int logicalConnections::getNodeA(int lineIndex)
{
	return connections[lineIndex].nodeIndexA;
}

int logicalConnections::getNodeB(int lineIndex)
{
	return connections[lineIndex].nodeIndexB;
}

void logicalConnections::deleteConnection(int lineIndex)
{
	connections[lineIndex].deleted = true;
}
