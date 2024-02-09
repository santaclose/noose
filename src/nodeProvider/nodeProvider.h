#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "nodeFunctionality.h"
#include "../nodeData.h"
#include "../types.h"

namespace nodeProvider
{
	void initialize();
	void terminate();
	const nodeData* getNodeDataByName(const std::string& name);
	// we need to sort nodes by name length for the search engine to work correctly
	const std::vector<std::string>& getNodeNamesSortedByLength();
	const std::vector<std::string>& getCategories();
	const std::vector<std::string>* getNodesForCategory(const std::string& name);
	const std::string& getCustomNodeFilePath(int customNodeId);
}

