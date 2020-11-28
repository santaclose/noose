#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "nodeFunctionality.h"
#include "../nodeData.h"
#include "../types.h"

namespace nodeProvider
{
	extern std::vector<nodeData> nodeDataList;

	// we need to sort nodes by name length for the search engine to work correctly
	extern std::vector<int> sortedByLength;

	extern std::vector<std::string> categoryNames;
	extern std::vector<int> categoryStartIndex;
	extern std::vector<std::vector<std::string>> nodeNamesByCategory;

	void initialize();
	//void print();
	void terminate();
}

