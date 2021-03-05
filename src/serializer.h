#pragma once
#include <string>

class serializer {
public:
	static void LoadFromFile(const std::string& filePath);
	static void SaveIntoFile(const std::string& filePath);
	static void LoadImageFile(const std::string& filePath);
};