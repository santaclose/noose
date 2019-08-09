#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>

#include "dataController.h"
#include "nodeData.h"
#include "nodeFunctionality.h"
 
namespace dataController
{
	std::vector<std::string*> searchResults;
	sf::Shader loadImageShader;
}

std::vector<nodeData> nodeDataList;

inline void* getFunctionalityFromIndex(int index)
{
	if (index == 0)
		return nodeFunctionality::Checker;
	if (index == 1)
		return nodeFunctionality::ConstructColor;
	if (index == 2)
		return nodeFunctionality::ConstructVector2i;
	if (index == 3)
		return nodeFunctionality::Flip;
	if (index == 4)
		return nodeFunctionality::Float;
	if (index == 5)
		return nodeFunctionality::Image;
	if (index == 6)
		return nodeFunctionality::Integer;
	if (index == 7)
		return nodeFunctionality::Invert;
	if (index == 8)
		return nodeFunctionality::LinearGradient;
	if (index == 9)
		return nodeFunctionality::Multiply;
	if (index == 10)
		return nodeFunctionality::Output;
	if (index == 11)
		return nodeFunctionality::Repeat;
	if (index == 12)
		return nodeFunctionality::Rotate90;
}

inline void parsePinLine(const std::string& line, std::string& a, std::string& b)
{
	int i = 2;
	for (; line[i] != ':'; i++);
	a = line.substr(2, i - 2);
	i += 2;
	b = line.substr(i, line.length() - i);
}

inline uiNodeSystem::Types typeFromString(const std::string& s)
{
	if (s == "Image")
		return uiNodeSystem::Types::Image;
	else if (s == "Vector2i")
		return uiNodeSystem::Types::Vector2i;
	else if (s == "Float")
		return uiNodeSystem::Types::Float;
	else if (s == "Integer")
		return uiNodeSystem::Types::Integer;
	else if (s == "Color")
		return uiNodeSystem::Types::Color;
}

// loads the dat file in memory
void dataController::prepare()
{
	using namespace std;
	bool insideDataSection = false;
	bool inSection = true;

	int currentNode = -1;

	ifstream inputStream("res/nodes.dat");
	string line;

	string type, name;

	while (getline(inputStream, line))
	{
		if (line[0] == '[')
		{
			insideDataSection = true;
			continue;
		}
		else if (line[0] == ']')
		{
			insideDataSection = false;
			continue;
		}
		if (!insideDataSection)
		{
			nodeDataList.emplace_back();
			nodeDataList.back().nodeName = line;
			nodeDataList.back().outputPinCount = nodeDataList.back().inputPinCount = 0;
			nodeDataList.back().nodeFunctionality = getFunctionalityFromIndex(nodeDataList.size() - 1);
		}
		else
		{
			if (line.find("\tin") != string::npos)
				inSection = true;
			else if (line.find("\tout:") != string::npos)
				inSection = false;
			else
			{
				parsePinLine(line, type, name);
				nodeDataList.back().pinNames.push_back(name);
				nodeDataList.back().pinTypes.push_back(typeFromString(type));
				if (inSection)
					nodeDataList.back().inputPinCount++;
				else
					nodeDataList.back().outputPinCount++;
			}
		}
	}


	inputStream.close();
	/*
	// log
	for (nodeData& n : nodeDataList)
	{
		cout << n.nodeName << ":\n";
		cout << "\tinput pins: " << n.inputPinCount << '\n';
		cout << "\toutput pins: " << n.outputPinCount << "\n\tin:\n";
		for (int i = 0; i < n.inputPinCount + n.outputPinCount; i++)
		{
			if (i == n.inputPinCount)
				cout << "\tout:\n";
			cout << "\t\t" << n.pinNames[i] << ": " << n.pinTypes[i] << endl;
		}
	}*/


	// load essential shaders

	if (!loadImageShader.loadFromFile("res/shaders/loadImage.shader", sf::Shader::Fragment))
	{
		std::cout << "could not load shader for loading image.\n";
		return;
	}

	// load node shaders
	nodeFunctionality::initialize();
}

void* dataController::getDataFor(int searchResultIndex)
{
	for (nodeData& n : nodeDataList)
	{
		if (&(n.nodeName) == searchResults[searchResultIndex])
		{
			return &n;
		}
	}
	return nullptr;
}

inline char tolower(char in) {
	if (in <= 'Z' && in >= 'A')
		return in - ('Z' - 'z');
	return in;
}

inline bool searchFunction(const char* searchBuffer, int bufferSize, std::string& nodeName)
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

int dataController::search(const char* searchBuffer, int bufferSize, int maxResults)
{
	//std::cout << "searching\n";
	searchResults.clear();
	for (nodeData& n : nodeDataList)
	{
		if (searchFunction(searchBuffer, bufferSize, n.nodeName))
		{
			//std::cout << "pushing to search results\n";
			searchResults.push_back(&n.nodeName);
			if (searchResults.size() == maxResults)
				return maxResults;
		}
	}
	return searchResults.size();
}