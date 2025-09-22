#pragma once

#include <SFML/Graphics.hpp>

namespace nooseMath
{
	static constexpr float PI = 3.14159265358979323846264f;
	static constexpr float DEG2RAD = PI / 180.0f;

	int mod(int a, int b);

	bool isPointInsideRect(const sf::Vector2f& point, const sf::Rect<float>& rect);
	bool isPointInsideRect(const sf::Vector2f& point, float top, float bottom, float left, float right);
	bool isPointInsideRect(const sf::Vector2f& point, const sf::Vector2f topLeftCorner, const sf::Vector2f bottomRightCorner);
	float distance(const sf::Vector2f& a, const sf::Vector2f b);
	float dot(const sf::Vector2f& a, const sf::Vector2f b);
	float length(const sf::Vector2f& v);
	float lengthSquared(const sf::Vector2f& v);

	sf::Vector2f rightVector(const sf::Vector2f& a);
	sf::Vector2f normalized(const sf::Vector2f& a);

	float minimumDistance(const sf::Vector2f& v, const sf::Vector2f& w, const sf::Vector2f& p);
};