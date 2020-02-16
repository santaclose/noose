#pragma once
#include "logicalNode.h"

struct logicalConnection
{
	bool deleted = false;
	// left side node
	logicalNode* nodeA;
	// right side node
	logicalNode* nodeB;
	// left side pin
	int pinA;
	// right side pin
	int pinB;

	int nodeIndexA;
	int nodeIndexB;
};

