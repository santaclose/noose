#include "nodeProvider.h"
#include <fstream>
#include "../utils.h"

namespace nodeProvider {

	std::vector<nodeData> nodeDataList;
	std::vector<int> sortedByLength;
	std::vector<std::string> categoryNames;
	std::vector<int> categoryStartIndex;
	std::vector<std::vector<std::string>> nodeNamesByCategory;

	void insertSorted();
	void parsePinLine(const std::string& line, std::string& type, std::string& name, std::string& defaultData, std::vector<std::string>& enumOptions);
	inline void* getFunctionalityFromIndex(int index)
	{
		if (index == 0)
			return (void*)nodeFunctionality::ImageFromFile;
		if (index == 1)
			return (void*)nodeFunctionality::Solid;
		if (index == 2)
			return (void*)nodeFunctionality::Checker;
		if (index == 3)
			return (void*)nodeFunctionality::LinearGradient;
		if (index == 4)
			return (void*)nodeFunctionality::SeparateChannels;
		if (index == 5)
			return (void*)nodeFunctionality::CombineChannels;
		if (index == 6)
			return (void*)nodeFunctionality::Blend;
		if (index == 7)
			return (void*)nodeFunctionality::BrightnessContrast;
		if (index == 8)
			return (void*)nodeFunctionality::Crop;
		if (index == 9)
			return (void*)nodeFunctionality::Extend;
		if (index == 10)
			return (void*)nodeFunctionality::Patch;
		if (index == 11)
			return (void*)nodeFunctionality::Flip;
		if (index == 12)
			return (void*)nodeFunctionality::Frame;
		if (index == 13)
			return (void*)nodeFunctionality::Grayscale;
		if (index == 14)
			return (void*)nodeFunctionality::GammaCorrection;
		if (index == 15)
			return (void*)nodeFunctionality::Invert;
		if (index == 16)
			return (void*)nodeFunctionality::Mask;
		if (index == 17)
			return (void*)nodeFunctionality::Repeat;
		if (index == 18)
			return (void*)nodeFunctionality::Rotate90;
		if (index == 19)
			return (void*)nodeFunctionality::Scale;
		if (index == 20)
			return (void*)nodeFunctionality::SelectByColor;
		if (index == 21)
			return (void*)nodeFunctionality::Color;
		if (index == 22)
			return (void*)nodeFunctionality::ColorFromRGBAInts;
		if (index == 23)
			return (void*)nodeFunctionality::RGBAIntsFromColor;
		if (index == 24)
			return (void*)nodeFunctionality::ColorFromImage;
		if (index == 25)
			return (void*)nodeFunctionality::Vector2i;
		if (index == 26)
			return (void*)nodeFunctionality::Vector2iFromInts;
		if (index == 27)
			return (void*)nodeFunctionality::SeparateVector2i;
		if (index == 28)
			return (void*)nodeFunctionality::Vector2iAddition;
		if (index == 29)
			return (void*)nodeFunctionality::Vector2iSubtraction;
		if (index == 30)
			return (void*)nodeFunctionality::Vector2iTimesInt;
		if (index == 31)
			return (void*)nodeFunctionality::Integer;
		if (index == 32)
			return (void*)nodeFunctionality::IntegerAddition;
		if (index == 33)
			return (void*)nodeFunctionality::IntegerSubtraction;
		if (index == 34)
			return (void*)nodeFunctionality::IntegerProduct;
		if (index == 35)
			return (void*)nodeFunctionality::IntegerDivision;
		if (index == 36)
			return (void*)nodeFunctionality::Float;
		if (index == 37)
			return (void*)nodeFunctionality::FloatAddition;
		if (index == 38)
			return (void*)nodeFunctionality::FloatSubtraction;
		if (index == 39)
			return (void*)nodeFunctionality::FloatProduct;
		if (index == 40)
			return (void*)nodeFunctionality::FloatDivision;
		std::cout << "[Node provider] COULD NOT GET FUNCTIONALITY FOR INDEX " << index << std::endl;
		return nullptr;
	}
	inline int typeFromString(const std::string& s)
	{
		if (s == "Integer")
			return NS_TYPE_INT;
		else if (s == "Float")
			return NS_TYPE_FLOAT;
		else if (s == "Vector2i")
			return NS_TYPE_VECTOR2I;
		else if (s == "Image")
			return NS_TYPE_IMAGE;
		else if (s == "Color")
			return NS_TYPE_COLOR;
		std::cout << "[Node provider] ERROR WHEN PARSING PIN TYPE \"" << s << "\"\n";
		return -1;
	}
}

void nodeProvider::initialize()
{
	using namespace std;
	bool insideDataSection = false;
	bool inSection = true;

	unsigned int currentCategory = 0;

	ifstream inputStream(utils::getProgramDirectory() + "assets/nodes.dat");
	string line;

	string type, name, defaultData;
	vector<string> enumOptions;

	while (getline(inputStream, line))
	{
		if (line[0] == '-' && line[1] == '-' && line[2] == ' ')
		{
			categoryNames.emplace_back(line.substr(3, line.length() - 3));
			categoryStartIndex.push_back(nodeDataList.size());
			nodeNamesByCategory.emplace_back();
			currentCategory++;
			continue;
		}

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
			nodeNamesByCategory[currentCategory - 1u].push_back(line);
			nodeDataList.emplace_back();
			nodeDataList.back().nodeId = nodeDataList.size() - 1;
			nodeDataList.back().nodeName = line;
			nodeDataList.back().outputPinCount = nodeDataList.back().inputPinCount = 0;
			nodeDataList.back().nodeFunctionality = getFunctionalityFromIndex(nodeDataList.size() - 1);

			insertSorted();
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
				nodeDataList.back().pinDefaultData.push_back(nullptr);

				if (defaultData.length() > 0)
				{
					switch (nodeDataList.back().pinTypes.back())
					{
					case NS_TYPE_FLOAT:
						nodeDataList.back().pinDefaultData.back() = new float(std::stof(defaultData));
						break;
					case NS_TYPE_INT:
						nodeDataList.back().pinDefaultData.back() = new int(std::stoi(defaultData));
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
		for (int i = 0 ; i < nd.pinDefaultData.size(); i++)
		{
			void* p = nd.pinDefaultData[i];
			if (p != nullptr)
			{
				switch(nd.pinTypes[i])
				{
					case NS_TYPE_FLOAT:
						delete (float*) p;
						break;
					case NS_TYPE_INT:
						delete (int*) p;
						break;
				}
			}
		}
	}
}

void nodeProvider::insertSorted()
{
	int i = 0;
	for (int item : sortedByLength)
	{
		if (nodeDataList[item].nodeName.length() > nodeDataList.back().nodeName.length())
			break;
		i++;
	}
	sortedByLength.insert(sortedByLength.begin() + i, nodeDataList.size() - 1);
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