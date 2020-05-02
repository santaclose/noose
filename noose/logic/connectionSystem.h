#pragma once
#include <vector>
#include "connection.h"
#include "node.h"

class connectionSystem
{
public:
	static std::vector<connection> connections;
	static void connect(int connectionIndex, const std::vector<node*>& nodeList, int nodeA, int nodeB, int pinA, int pinB);
	static void deleteConnection(int lineIndex);
};