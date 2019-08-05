#pragma once

#include "uiNodeSystem.h"

namespace dataController
{
	extern std::vector<std::string*> searchResults;
	void prepare();
	int search(const char* searchBuffer, int bufferSize, int maxResults);
	void* getDataFor(int searchResultIndex);
};