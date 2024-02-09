#include "nodeProvider.h"
#include <fstream>
#include <unordered_map>
#include <vector>
#include <utility>
#include "../pathUtils.h"
#include "../utils.h"

namespace nodeProvider {

	std::vector<nodeData> nodeDataList;
	std::unordered_map<std::string, int> nodeName2data;
	std::vector<std::string> categories;
	std::unordered_map<std::string, std::vector<std::string>> nodesByCategory;
	std::vector<std::string> nodeNamesSortedByLength;
	std::unordered_map<std::string, void*> name2func =
	{
		{"Int", (void*)nodeFunctionality::Integer},
		{"Float", (void*)nodeFunctionality::Float},
		{"Int vector", (void*)nodeFunctionality::Vector2i},
		{"Color", (void*)nodeFunctionality::Color},
		{"Image", (void*)nodeFunctionality::ImageFromFile},
		{"String", (void*)nodeFunctionality::String},
		{"Font", (void*)nodeFunctionality::Font},
		{"Solid", (void*)nodeFunctionality::Solid},
		{"Checker", (void*)nodeFunctionality::Checker},
		{"Linear Gradient", (void*)nodeFunctionality::LinearGradient},
		{"Uniform Noise", (void*)nodeFunctionality::UniformNoise},
		{"Image from text", (void*)nodeFunctionality::ImageFromText},
		{"Separate channels", (void*)nodeFunctionality::SeparateChannels},
		{"Combine channels", (void*)nodeFunctionality::CombineChannels},
		{"Blend", (void*)nodeFunctionality::Blend},
		{"Brightness and contrast", (void*)nodeFunctionality::BrightnessContrast},
		{"Crop", (void*)nodeFunctionality::Crop},
		{"Extend", (void*)nodeFunctionality::Extend},
		{"Patch", (void*)nodeFunctionality::Patch},
		{"Flip", (void*)nodeFunctionality::Flip},
		{"Frame", (void*)nodeFunctionality::Frame},
		{"Grayscale", (void*)nodeFunctionality::Grayscale},
		{"Gamma Correction", (void*)nodeFunctionality::GammaCorrection},
		{"Invert", (void*)nodeFunctionality::Invert},
		{"Mask", (void*)nodeFunctionality::Mask},
		{"Repeat", (void*)nodeFunctionality::Repeat},
		{"Rotate", (void*)nodeFunctionality::Rotate},
		{"Rotate 90", (void*)nodeFunctionality::Rotate90},
		{"Scale", (void*)nodeFunctionality::Scale},
		{"Select by color", (void*)nodeFunctionality::SelectByColor},
		{"Flat Blur", (void*)nodeFunctionality::FlatBlur},
		{"Make color", (void*)nodeFunctionality::ColorFromRGBAInts},
		{"Break color", (void*)nodeFunctionality::RGBAIntsFromColor},
		{"Color from image", (void*)nodeFunctionality::ColorFromImage},
		{"Color from string", (void*)nodeFunctionality::ColorFromString},
		{"Make int vector", (void*)nodeFunctionality::Vector2iFromInts},
		{"Break int vector", (void*)nodeFunctionality::SeparateVector2i},
		{"Add int vectors", (void*)nodeFunctionality::Vector2iAddition},
		{"Subtract int vectors", (void*)nodeFunctionality::Vector2iSubtraction},
		{"Int vector times int", (void*)nodeFunctionality::Vector2iTimesInt},
		{"Add ints", (void*)nodeFunctionality::IntegerAddition},
		{"Subtract ints", (void*)nodeFunctionality::IntegerSubtraction},
		{"Multiply ints", (void*)nodeFunctionality::IntegerProduct},
		{"Divide ints", (void*)nodeFunctionality::IntegerDivision},
		{"Max int", (void*)nodeFunctionality::IntegerMax},
		{"Min int", (void*)nodeFunctionality::IntegerMin},
		{"Abs int", (void*)nodeFunctionality::IntegerAbs},
		{"Add floats", (void*)nodeFunctionality::FloatAddition},
		{"Subtract floats", (void*)nodeFunctionality::FloatSubtraction},
		{"Multiply floats", (void*)nodeFunctionality::FloatProduct},
		{"Divide floats", (void*)nodeFunctionality::FloatDivision},
		{"Max float", (void*)nodeFunctionality::FloatMax},
		{"Min float", (void*)nodeFunctionality::FloatMin},
		{"Abs float", (void*)nodeFunctionality::FloatAbs}
	};
	std::vector<std::string> customNodeFilePaths;

	void insertSorted();
	bool parseLine(const std::string& line, bool& insideDataSection, bool& inSection, int& currentCustomNode);
	void parsePinLine(const std::string& line, std::string& type, std::string& name, std::string& defaultData, std::vector<std::string>& enumOptions);
	inline void* getFunctionalityFromName(const std::string& name)
	{
		if (name2func.find(name) == name2func.end())
		{
			std::cout << "[Node provider] COULD NOT GET FUNCTIONALITY FOR NODE " << name << std::endl;
			return nullptr;
		}
		return name2func[name];
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
		else if (s == "String")
			return NS_TYPE_STRING;
		else if (s == "Font")
			return NS_TYPE_FONT;
		std::cout << "[Node provider] ERROR WHEN PARSING PIN TYPE \"" << s << "\"\n";
		return -1;
	}
}

void nodeProvider::initialize()
{
	std::ifstream builtInNodesInputStream(pathUtils::getAssetsDirectory() + "nodes.dat");

	int currentCustomNode = -1;
	bool insideDataSection = false;
	bool inSection = true;
	std::string line;
	while (getline(builtInNodesInputStream, line))
		parseLine(line, insideDataSection, inSection, currentCustomNode);
	builtInNodesInputStream.close();

	for (nodeData& nd : nodeDataList)
		nd.nodeFunctionality = getFunctionalityFromName(nd.nodeName);

	pathUtils::getCustomNodeFiles(customNodeFilePaths);
	for (const std::string& customNodeFilePath : customNodeFilePaths)
	{
		currentCustomNode++;
		std::ifstream customNodeInputStream(customNodeFilePath);
		while (getline(customNodeInputStream, line) && parseLine(line, insideDataSection, inSection, currentCustomNode));
		customNodeInputStream.close();
	}

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
const nodeData* nodeProvider::getNodeDataByName(const std::string& name)
{
	if (nodeName2data.find(name) == nodeName2data.end())
		return nullptr; // not found
	return &nodeDataList[nodeName2data[name]];
}
const std::vector<std::string>& nodeProvider::getNodeNamesSortedByLength()
{
	return nodeNamesSortedByLength;
}
const std::vector<std::string>& nodeProvider::getCategories()
{
	return categories;
}
const std::vector<std::string>* nodeProvider::getNodesForCategory(const std::string& name)
{
	if (nodesByCategory.find(name) == nodesByCategory.end())
		return nullptr; // not found
	return &nodesByCategory[name];
}

const std::string& nodeProvider::getCustomNodeFilePath(int customNodeId)
{
	return customNodeFilePaths[customNodeId];
}

void nodeProvider::insertSorted()
{
	int i = 0;
	for (const std::string& item : nodeNamesSortedByLength)
	{
		if (item.length() > nodeDataList.back().nodeName.length())
			break;
		i++;
	}
	nodeNamesSortedByLength.insert(nodeNamesSortedByLength.begin() + i, nodeDataList.back().nodeName);
}

bool nodeProvider::parseLine(const std::string& line, bool& insideDataSection, bool& inSection, int& currentCustomNode)
{
	static std::string type;
	static std::string name;
	static std::string defaultData;
	static std::vector<std::string> enumOptions;

	if (line[0] == '|') // non parseable region
		return false;

	if (line[0] == '[')
	{
		insideDataSection = true;
		return true;
	}
	else if (line[0] == ']')
	{
		insideDataSection = false;
		return true;
	}
	if (!insideDataSection)
	{
		nodeDataList.emplace_back();
		nodeDataList.back().nodeName = line;
		nodeDataList.back().outputPinCount = nodeDataList.back().inputPinCount = 0;
		nodeName2data[line] = nodeDataList.size() - 1;

		insertSorted();
	}
	else
	{
		if (line.find("\tcategory: ") != std::string::npos)
		{
			std::string parsedCat = line.substr(11);
			nodesByCategory[parsedCat].push_back(nodeDataList.back().nodeName);
			bool needToAdd = true;
			for (const std::string& cat : categories)
			{
				if (cat == parsedCat)
				{
					needToAdd = false;
					break;
				}
			}
			if (needToAdd)
				categories.push_back(parsedCat);
		}
		else if (line.find("\tin:") != std::string::npos)
			inSection = true;
		else if (line.find("\tout:") != std::string::npos)
			inSection = false;
		else
		{
			parsePinLine(line, type, name, defaultData, enumOptions);
			nodeDataList.back().pinNames.push_back(name);
			nodeDataList.back().pinTypes.push_back(typeFromString(type));
			nodeDataList.back().pinEnumOptions.push_back(enumOptions);
			nodeDataList.back().pinDefaultData.push_back(nullptr);
			nodeDataList.back().customNodeId = currentCustomNode;

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
				case NS_TYPE_STRING:
					nodeDataList.back().pinDefaultData.back() = new std::string(defaultData);
					break;
				case NS_TYPE_COLOR:
					nodeDataList.back().pinDefaultData.back() = new sf::Color();
					utils::colorFromHexString(defaultData, *((sf::Color*)nodeDataList.back().pinDefaultData.back()));
					break;
				}
			}

			if (inSection)
				nodeDataList.back().inputPinCount++;
			else
				nodeDataList.back().outputPinCount++;
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