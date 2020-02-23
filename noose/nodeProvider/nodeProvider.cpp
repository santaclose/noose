#include "nodeProvider.h"
#include <fstream>

std::vector<nodeData> nodeProvider::nodeDataList;

void nodeProvider::initialize()
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

	// load node shaders and render state
	nodeFunctionality::initialize();
}
void nodeProvider::terminate()
{
	for (nodeData& nd : nodeDataList)
	{
		for (void* p : nd.pinDefaultData)
		{
			if (p != nullptr)
				delete p;
		}
	}
}

void nodeProvider::parsePinLine(const std::string& line, std::string& a, std::string& b, std::string& d)
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
