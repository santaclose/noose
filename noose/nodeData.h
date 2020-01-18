#pragma once
#include <vector>
#include <string>
//#include "logicalNode.h"

struct nodeData
{
	std::string nodeName;
	//std::vector<logicalNode::PinType> pinTypes;
	std::vector<int> pinTypes;
	std::vector<std::string> pinNames;
	std::vector<void*> pinDefaultData;

	int inputPinCount;
	int outputPinCount;
	void* nodeFunctionality;
};