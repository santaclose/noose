#include "searcher.h"
#include "nodeProvider/nodeProvider.h"

//#include <string.h>

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

const nodeData* searcher::getDataFor(int searchResultIndex)
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
	if (searchBuffer[0] == '\0')
		return true;

	int offset = 0;
	while (true)
	{
		if (nodeName[offset] == '\0')
			return false;
		if (tolower(nodeName[offset]) == tolower(searchBuffer[0]))
		{
			for (int i = 0; searchBuffer[i] != '\0'; i++)
			{
				if (i == nodeName.length())
					return searchBuffer[i] == '\0';
				if (tolower(nodeName[i + offset]) != tolower(searchBuffer[i]))
					goto l;
			}
			return true;
		}
		l:
		offset++;
	}
}