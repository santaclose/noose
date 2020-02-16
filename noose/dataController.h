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
			return (void*) nodeFunctionality::Blend;
		if (index == 1)
			return (void*) nodeFunctionality::BrightnessContrast;
		if (index == 2)
			return (void*) nodeFunctionality::Checker;
		if (index == 3)
			return (void*) nodeFunctionality::Combine;
		if (index == 4)
			return (void*) nodeFunctionality::ConstructColor;
		if (index == 5)
			return (void*) nodeFunctionality::ConstructVector2i;
		if (index == 6)
			return (void*) nodeFunctionality::Crop;
		if (index == 7)
			return (void*) nodeFunctionality::Flip;
		if (index == 8)
			return (void*) nodeFunctionality::Float;
		if (index == 9)
			return (void*) nodeFunctionality::Grayscale;
		if (index == 10)
			return (void*) nodeFunctionality::Image;
		if (index == 11)
			return (void*) nodeFunctionality::Integer;
		if (index == 12)
			return (void*) nodeFunctionality::Invert;
		if (index == 13)
			return (void*) nodeFunctionality::LinearGradient;
		if (index == 14)
			return (void*) nodeFunctionality::Mask;
		if (index == 15)
			return (void*) nodeFunctionality::Repeat;
		if (index == 16)
			return (void*) nodeFunctionality::Rotate90;
		if (index == 17)
			return (void*) nodeFunctionality::SelectByColor;
		if (index == 18)
			return (void*) nodeFunctionality::Separate;
		if (index == 19)
			return (void*) nodeFunctionality::Solid;
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