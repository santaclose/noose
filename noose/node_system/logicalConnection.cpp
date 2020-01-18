#include "logicalConnection.h"

logicalConnection::logicalConnection(logicalNode* nodeA, int pinA, logicalNode* nodeB, int pinB, int nodeIndexA, int nodeIndexB)
{
	this->nodeA = nodeA;
	this->nodeB = nodeB;
	this->pinA = pinA;
	this->pinB = pinB;
	this->nodeIndexA = nodeIndexA;
	this->nodeIndexB = nodeIndexB;
}
