#pragma once
#include <vector>
#include <string>

struct nodeData
{
	unsigned int nodeId;
	std::string nodeName;
	std::vector<int> pinTypes;
	std::vector<std::string> pinNames;
	std::vector<void*> pinDefaultData;
	std::vector<std::vector<std::string>> pinEnumOptions;

	int inputPinCount;
	int outputPinCount;
	void* nodeFunctionality;
};