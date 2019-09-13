#pragma once

#include <SFML/Graphics.hpp>

namespace uiMath
{
	extern const float DEG2RAD;

	bool isPointInsideRect(const sf::Vector2f& point, const sf::Rect<float>& rect);
	bool isPointInsideRect(const sf::Vector2f& point, float top, float bottom, float left, float right);
	float distance(const sf::Vector2f& a, const sf::Vector2f b);
	float dot(const sf::Vector2f& a, const sf::Vector2f b);
	float lengthSquared(const sf::Vector2f&& v);

	sf::Vector2f rightVector(sf::Vector2f&& a);
	sf::Vector2f normalized(sf::Vector2f&& a);

	//bool linesIntersect(const sf::Vector2f& p1, const sf::Vector2f& q1, const sf::Vector2f& p2, const sf::Vector2f& q2);
	float minimumDistance(const sf::Vector2f& v, const sf::Vector2f& w, const sf::Vector2f& p);
	void HSVtoRGB(int H, double S, double V, sf::Color& output);
};

sf::Vector2f operator*(const sf::Vector2f& v, const float f);
sf::Vector2f operator/(const sf::Vector2f& v, const float f);