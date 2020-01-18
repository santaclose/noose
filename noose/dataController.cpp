#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>

#include "dataController.h"

std::vector<std::string*> dataController::searchResults;

std::vector<nodeData> dataController::nodeDataList;
sf::Shader dataController::loadImageShader;

// loads the dat file in memory
void dataController::initialize()
{
	using namespace std;
	bool insideDataSection = false;
	bool inSection = true;

	int currentNode = -1;

	ifstream inputStream("res/nodes.dat");
	string line;

	string type, name, defaultData;

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
				parsePinLine(line, type, name, defaultData);
				nodeDataList.back().pinNames.push_back(name);
				nodeDataList.back().pinTypes.push_back(typeFromString(type));
				if (defaultData.length() == 0)
					nodeDataList.back().pinDefaultData.push_back(nullptr);
				else
				{
					switch (nodeDataList.back().pinTypes.back())
					{
					case NS_TYPE_FLOAT:
						nodeDataList.back().pinDefaultData.push_back(new float(std::stof(defaultData)));
						break;
					case NS_TYPE_INT:
						nodeDataList.back().pinDefaultData.push_back(new int(std::stoi(defaultData)));
						break;
					}
				}
				//nodeDataList.back().pinDefaultData.push_back(defaultDataFromString(defaultData));
				//std::cout << "default pin data: " << defaultData << std::endl;
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

char* dataController::getFileNameFromPath(char* string)
{
	int i = 0;
	for (; string[i] != '\0'; i++);
	for (; string[i - 1] != '\\' && string[i - 1] != '/'; i--);
	return &(string[i]);
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

char dataController::tolower(char in) {
	if (in <= 'Z' && in >= 'A')
		return in - ('Z' - 'z');
	return in;
}

bool dataController::searchFunction(const char* searchBuffer, int bufferSize, std::string& nodeName)
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

void dataController::parsePinLine(const std::string& line, std::string& a, std::string& b, std::string& d)
{
	int i = 2;
	for (; line[i] != ':'; i++);
	a = line.substr(2, i - 2);
	i += 2;
	int j = i;
	for (; j < line.length() && line[j] != '['; j++);
	b = line.substr(i, j - i);
	if (j < line.length()) // there is a default value
	{
		j++;
		for (i = j; line[i] != ']'; i++);
		d = line.substr(j, i - j);
		//std::cout << "found default" << std::endl;
	}
	else
	{
		d = "";
	}
}