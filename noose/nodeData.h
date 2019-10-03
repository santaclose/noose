#pragma once
#include <vector>
#include <string>
#include "uiNodeSystem.h"

struct nodeData
{
	std::string nodeName;
	std::vector<uiNodeSystem::Types> pinTypes;
	std::vector<std::string> pinNames;
	std::vector<void*> pinDefaultData;

	int inputPinCount;
	int outputPinCount;
	void* nodeFunctionality;
};