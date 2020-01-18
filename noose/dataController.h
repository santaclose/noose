#pragma once
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

#include "nodeData.h"
#include "nodeFunctionality.h"
#include "types.h"

class dataController
{
public:
	static std::vector<std::string*> searchResults;
	static sf::Shader loadImageShader;

private:
	static std::vector<nodeData> nodeDataList;

public:
	static void initialize();
	static int search(const char* searchBuffer, int bufferSize, int maxResults);
	static char* getFileNameFromPath(char* string);
	static void* getDataFor(int searchResultIndex);

private:
	static char tolower(char in);
	static bool searchFunction(const char* searchBuffer, int bufferSize, std::string& nodeName);
	static void parsePinLine(const std::string& line, std::string& a, std::string& b, std::string& d);

	static inline void* getFunctionalityFromIndex(int index)
	{
		if (index == 0)
			return nodeFunctionality::Blend;
		if (index == 1)
			return nodeFunctionality::BrightnessContrast;
		if (index == 2)
			return nodeFunctionality::Checker;
		if (index == 3)
			return nodeFunctionality::Combine;
		if (index == 4)
			return nodeFunctionality::ConstructColor;
		if (index == 5)
			return nodeFunctionality::ConstructVector2i;
		if (index == 6)
			return nodeFunctionality::Crop;
		if (index == 7)
			return nodeFunctionality::Flip;
		if (index == 8)
			return nodeFunctionality::Float;
		if (index == 9)
			return nodeFunctionality::Grayscale;
		if (index == 10)
			return nodeFunctionality::Image;
		if (index == 11)
			return nodeFunctionality::Integer;
		if (index == 12)
			return nodeFunctionality::Invert;
		if (index == 13)
			return nodeFunctionality::LinearGradient;
		if (index == 14)
			return nodeFunctionality::Mask;
		if (index == 15)
			return nodeFunctionality::Repeat;
		if (index == 16)
			return nodeFunctionality::Rotate90;
		if (index == 17)
			return nodeFunctionality::SelectByColor;
		if (index == 18)
			return nodeFunctionality::Separate;
		if (index == 19)
			return nodeFunctionality::Solid;
	}

	static inline int typeFromString(const std::string& s)
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
	}
};