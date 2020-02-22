#pragma once
#include <vector>
#include <string>

class searcher
{
public:
	static std::vector<std::string*> searchResults;
	static int search(const char* searchBuffer, int bufferSize, int maxResults);
	static void* getDataFor(int searchResultIndex);
private:
	static bool searchCheck(const char* searchBuffer, int bufferSize, std::string& nodeName);
};

