#include "nooseMath.h"
#include <cmath>
#include "vectorOperators.h"

int nooseMath::mod(int a, int b)
{
	int ret = a % b;
	if (ret < 0)
		ret += b;
	return ret;
}

bool nooseMath::isPointInsideRect(const sf::Vector2f& point, const sf::Rect<float>& rect)
{
	return point.x > rect.position.x && point.y > rect.position.y && point.x < rect.position.x + rect.size.x && point.y < rect.position.y + rect.size.y;
}
bool nooseMath::isPointInsideRect(const sf::Vector2f& point, float top, float bottom, float left, float right)
{
	return point.x < right && point.x > left && point.y > top && point.y < bottom;
}
bool nooseMath::isPointInsideRect(const sf::Vector2f& point, const sf::Vector2f topLeftCorner, const sf::Vector2f bottomRightCorner)
{
	return point.x < bottomRightCorner.x && point.x > topLeftCorner.x && point.y < bottomRightCorner.y && point.y > topLeftCorner.y;
}

float nooseMath::distance(const sf::Vector2f& a, const sf::Vector2f b)
{
	sf::Vector2f disp = a - b;
	return sqrt(disp.x * disp.x + disp.y * disp.y);
}

float nooseMath::dot(const sf::Vector2f& a, const sf::Vector2f b)
{
	return a.x * b.x + a.y * b.y;
}
float nooseMath::length(const sf::Vector2f& v)
{
	return std::sqrtf(v.x * v.x + v.y * v.y);
}
float nooseMath::lengthSquared(const sf::Vector2f& v)
{
	return v.x * v.x + v.y * v.y;
}

sf::Vector2f nooseMath::rightVector(const sf::Vector2f& a)
{
	return sf::Vector2f(a.y, -a.x);
}
sf::Vector2f nooseMath::normalized(const sf::Vector2f& a)
{
	return a / sqrt(a.x * a.x + a.y * a.y);
}

// Return minimum distance between line segment vw and point p
// used to delete connections
float nooseMath::minimumDistance(const sf::Vector2f& v, const sf::Vector2f& w, const sf::Vector2f& p)
{
	float l2 = lengthSquared(w - v);  // i.e. |w-v|^2 -  avoid a sqrt
	if (l2 == 0.0) return distance(p, v);   // v == w case
	// Consider the line extending the segment, parameterized as v + t (w - v).
	// We find projection of point p onto the line. 
	// It falls where t = [(p-v) . (w-v)] / |w-v|^2
	// We clamp t from [0,1] to handle points outside the segment vw.
	
	float t = std::max(0.0f, std::min(1.0f, dot(p - v, w - v) / l2));
	sf::Vector2f projection = v + (w - v) * t;  // Projection falls on the segment
	return distance(p, projection);
}
