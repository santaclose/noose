#pragma once
#include <vector>
#include <string>
#include "nodeData.h"

namespace searcher
{
	extern std::vector<const std::string*> searchResults;
	int search(const char* searchBuffer, int bufferSize, int maxResults);
	const nodeData* getDataFor(int searchResultIndex);
}

