#pragma once
#include <iomanip>
#include <SFML/Graphics.hpp>

namespace utils
{
	char lower(char in);
	void rgb2hsv(const sf::Color& rgbColor, float& h, float& s, float& v);
	bool colorFromHexString(const std::string& hexString, sf::Color& outColor);

	template <typename T>
	std::string intToHex(T i)
	{
		std::stringstream stream;
		stream
			<< std::setfill ('0') << std::setw(sizeof(T)*2) 
			<< std::hex << i;
		return stream.str();
	}

	void drawQuads(sf::RenderTarget& renderTarget, const sf::Vertex* vertices, size_t vertexCount, const sf::RenderStates& renderStates = sf::RenderStates::Default);
}