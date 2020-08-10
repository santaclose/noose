#pragma once
#include <vector>
#include <string>
#include "nodeData.h"

class searcher
{
public:
	static std::vector<std::string*> searchResults;
	static int search(const char* searchBuffer, int bufferSize, int maxResults);
	static const nodeData* getDataFor(int searchResultIndex);
private:
	static bool searchCheck(const char* searchBuffer, int bufferSize, std::string& nodeName);
};

