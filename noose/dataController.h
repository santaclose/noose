#pragma once
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

namespace dataController
{
	extern std::vector<std::string*> searchResults;
	void initialize();
	void terminate();
	int search(const char* searchBuffer, int bufferSize, int maxResults);
	void* getDataFor(int searchResultIndex);

	extern sf::Shader loadImageShader;
};