#pragma once

#include <SFML/Graphics.hpp>

class uiMath
{
public:
	static const float DEG2RAD;

	static bool isPointInsideRect(const sf::Vector2f& point, const sf::Rect<float>& rect);
	static bool isPointInsideRect(const sf::Vector2f& point, float top, float bottom, float left, float right);
	static float distance(const sf::Vector2f& a, const sf::Vector2f b);
	static float dot(const sf::Vector2f& a, const sf::Vector2f b);
	static float lengthSquared(const sf::Vector2f&& v);

	static sf::Vector2f rightVector(sf::Vector2f&& a);
	static sf::Vector2f normalized(sf::Vector2f&& a);

	static float minimumDistance(const sf::Vector2f& v, const sf::Vector2f& w, const sf::Vector2f& p);
	static void HSVtoRGB(int H, double S, double V, sf::Color& output);
};