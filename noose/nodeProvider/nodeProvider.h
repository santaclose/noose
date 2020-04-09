#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "nodeFunctionality.h"
#include "../nodeData.h"
#include "../types.h"

class nodeProvider
{
public:
	static std::vector<nodeData> nodeDataList;
	static void initialize();
	//static void print();
	static void terminate();

private:
	static void parsePinLine(const std::string& line, std::string& type, std::string& name, std::string& defaultData, std::vector<std::string>& enumOptions);

	static inline void* getFunctionalityFromIndex(int index)
	{
		if (index == 0)
			return (void*)nodeFunctionality::Integer;
		if (index == 1)
			return (void*)nodeFunctionality::IntegerAddition;
		if (index == 2)
			return (void*)nodeFunctionality::IntegerSubtraction;
		if (index == 3)
			return (void*)nodeFunctionality::IntegerProduct;
		if (index == 4)
			return (void*)nodeFunctionality::IntegerDivision;
		if (index == 5)
			return (void*)nodeFunctionality::Vector2i;
		if (index == 6)
			return (void*)nodeFunctionality::Vector2iFromInts;
		if (index == 7)
			return (void*)nodeFunctionality::SeparateVector2i;
		if (index == 8)
			return (void*)nodeFunctionality::Vector2iAddition;
		if (index == 9)
			return (void*)nodeFunctionality::Vector2iSubtraction;
		if (index == 10)
			return (void*)nodeFunctionality::Vector2iTimesInt;
		if (index == 11)
			return (void*)nodeFunctionality::Float;
		if (index == 12)
			return (void*)nodeFunctionality::FloatAddition;
		if (index == 13)
			return (void*)nodeFunctionality::FloatSubtraction;
		if (index == 14)
			return (void*)nodeFunctionality::FloatProduct;
		if (index == 15)
			return (void*)nodeFunctionality::FloatDivision;
		if (index == 16)
			return (void*)nodeFunctionality::Color;
		if (index == 17)
			return (void*)nodeFunctionality::ColorFromInts;
		if (index == 18)
			return (void*)nodeFunctionality::Image;
		if (index == 19)
			return (void*)nodeFunctionality::SeparateChannels;
		if (index == 20)
			return (void*)nodeFunctionality::CombineChannels;
		if (index == 21)
			return (void*)nodeFunctionality::Solid;
		if (index == 22)
			return (void*)nodeFunctionality::Checker;
		if (index == 23)
			return (void*)nodeFunctionality::LinearGradient;
		if (index == 24)
			return (void*)nodeFunctionality::Blend;
		if (index == 25)
			return (void*)nodeFunctionality::BrightnessContrast;
		if (index == 26)
			return (void*)nodeFunctionality::Crop;
		if (index == 27)
			return (void*)nodeFunctionality::Flip;
		if (index == 28)
			return (void*)nodeFunctionality::Frame;
		if (index == 29)
			return (void*)nodeFunctionality::Grayscale;
		if (index == 30)
			return (void*)nodeFunctionality::GammaCorrection;
		if (index == 31)
			return (void*)nodeFunctionality::Invert;
		if (index == 32)
			return (void*)nodeFunctionality::Mask;
		if (index == 33)
			return (void*)nodeFunctionality::Repeat;
		if (index == 34)
			return (void*)nodeFunctionality::Rotate90;
		if (index == 35)
			return (void*)nodeFunctionality::SelectByColor;
		std::cout << "[Node provider] COULD NOT GET FUNCTIONALITY FOR INDEX " << index << std::endl;
		return nullptr;
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
		std::cout << "[Node provider] ERROR WHEN PARSING PIN TYPE \"" << s << "\"\n";
		return -1;
	}
};

