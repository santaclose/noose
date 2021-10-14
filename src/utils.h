#pragma once
#include <iomanip>
#include <SFML/Graphics.hpp>

namespace utils
{
	void rgb2hsv(const sf::Color& rgbColor, float& h, float& s, float& v);
	sf::Color colorFromHexString(const std::string& hexString);

	template <typename T>
	std::string intToHex(T i)
	{
		std::stringstream stream;
		stream
			<< std::setfill ('0') << std::setw(sizeof(T)*2) 
			<< std::hex << i;
		return stream.str();
	}
}