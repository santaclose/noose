#pragma once
#include "node.h"

struct connection
{
	bool deleted = false;
	// left side node
	node* nodeA;
	// right side node
	node* nodeB;
	// left side pin
	int pinA;
	// right side pin
	int pinB;

	int nodeIndexA;
	int nodeIndexB;
};