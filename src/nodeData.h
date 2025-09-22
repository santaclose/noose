#pragma once
#include <vector>
#include <string>

struct nodeData
{
	std::string nodeName;
	std::vector<int> pinTypes;
	std::vector<float> pinSensitivities;
	std::vector<std::string> pinNames;
	std::vector<void*> pinDefaultData;
	std::vector<std::vector<std::string>> pinEnumOptions;

	int inputPinCount;
	int outputPinCount;
	void* nodeFunctionality = nullptr;
	int customNodeId = -1;
};