#include "searcher.h"
#include "nodeData.h"
#include "nodeProvider/nodeProvider.h"

std::vector<std::string*> searcher::searchResults;

int searcher::search(const char* searchBuffer, int bufferSize, int maxResults)
{
	searchResults.clear();
	for (nodeData& n : nodeProvider::nodeDataList)
	{
		if (searchCheck(searchBuffer, bufferSize, n.nodeName))
		{
			//std::cout << "pushing to search results\n";
			searchResults.push_back(&n.nodeName);
			if (searchResults.size() == maxResults)
				return maxResults;
		}
	}
	return searchResults.size();
}

void* searcher::getDataFor(int searchResultIndex)
{
	for (nodeData& n : nodeProvider::nodeDataList)
	{
		if (&(n.nodeName) == searchResults[searchResultIndex])
		{
			return &n;
		}
	}
	return nullptr;
}

bool searcher::searchCheck(const char* searchBuffer, int bufferSize, std::string& nodeName)
{
	//std::cout << "Comparing with " << nodeName << std::endl;
	for (int i = 0; searchBuffer[i] != '\0'; i++)
	{
		if (i == nodeName.length())
			break;
		if (tolower(nodeName[i]) != tolower(searchBuffer[i])) {
			//std::cout << tolower(nodeName[i]) << " | " << tolower(searchBuffer[i]) << std::endl;
			return false;
		}
	}
	return true;
}
