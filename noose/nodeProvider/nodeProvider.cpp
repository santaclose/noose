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
	vector<string> enumOptions;

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
				parsePinLine(line, type, name, defaultData, enumOptions);
				nodeDataList.back().pinNames.push_back(name);
				nodeDataList.back().pinTypes.push_back(typeFromString(type));
				nodeDataList.back().pinEnumOptions.push_back(enumOptions);
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

/*void nodeProvider::print()
{
	std::cout << "[NODE PROVIDER] PRINTING ENUM THING\n";
	for (nodeData& d : nodeDataList)
	{
		std::cout << "node: " << d.nodeName << std::endl;
		for (std::vector<std::string>& v : d.pinEnumOptions)
		{
			for (std::string& s : v)
				std::cout << "\t" << s << std::endl;
		}
	}
}*/

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

void nodeProvider::parsePinLine(const std::string& line, std::string& type, std::string& name, std::string& defaultData, std::vector<std::string>& enumOptions)
{
	int i = 2;
	for (; line[i] != ':'; i++);
	type = line.substr(2, i - 2);
	i += 2;

	int j = i;
	for (; j < line.length() && line[j] != '[' && line[j] != '{'; j++);
	name = line.substr(i, j - i);

	i = j;

	if (j < line.length() && line[j] == '[') // there is a default value
	{
		j++;
		for (i = j; line[i] != ']'; i++);
		defaultData = line.substr(j, i - j);
		i++;
		j = i;
	}
	else
	{
		defaultData = "";
	}

	enumOptions.clear();
	if (j < line.length() && line[j] == '{') // there are enum options
	{
		j++;
		while (true)
		{
			if (line[i] == ',')
			{
				enumOptions.push_back(line.substr(j, i - j));
				j = i = i + 1;
			}
			else if (line[i] == '}')
			{
				enumOptions.push_back(line.substr(j, i - j));
				break;
			}
			else
			{
				i++;
			}
		}
	}
}