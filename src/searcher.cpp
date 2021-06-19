#include "searcher.h"
#include "nodeProvider/nodeProvider.h"

namespace searcher {

	std::vector<const std::string*> searchResults;

	bool searchCheck(const char* searchBuffer, int bufferSize, const std::string& nodeName);
}

int searcher::search(const char* searchBuffer, int bufferSize, int maxResults)
{
	searchResults.clear();
	for (const std::string& nodeName : nodeProvider::getNodeNamesSortedByLength())
	{
		if (searchCheck(searchBuffer, bufferSize, nodeName))
		{
			searchResults.push_back(&nodeName);
			if (searchResults.size() == maxResults)
				return maxResults;
		}
	}
	return searchResults.size();
}

const nodeData* searcher::getDataFor(int searchResultIndex)
{
	return nodeProvider::getNodeDataByName(*searchResults[searchResultIndex]);
}

bool searcher::searchCheck(const char* searchBuffer, int bufferSize, const std::string& nodeName)
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