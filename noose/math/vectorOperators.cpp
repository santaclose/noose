#include "vectorOperators.h"

sf::Vector2f operator*(const sf::Vector2f& v, const float f)
{
	return sf::Vector2f(v.x * f, v.y * f);
}

sf::Vector2f operator/(const sf::Vector2f& v, const float f)
{
	return sf::Vector2f(v.x / f, v.y / f);
}
