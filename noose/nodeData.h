#pragma once
#include <vector>
#include <string>
#include "uiNodeSystem.h"

struct nodeData
{
	std::string nodeName;
	std::vector<uiNodeSystem::Types> pinTypes;
	std::vector<std::string> pinNames;
	int inputPinCount;
	int outputPinCount;
	void* nodeFunctionality;
};