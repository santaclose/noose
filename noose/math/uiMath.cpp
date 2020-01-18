#include "uiMath.h"
#include <cmath>

const float uiMath::DEG2RAD = 0.0174532925;

bool uiMath::isPointInsideRect(const sf::Vector2f& point, const sf::Rect<float>& rect)
{
	return point.x > rect.left && point.y > rect.top && point.x < rect.left + rect.width && point.y < rect.top + rect.height;
}
bool uiMath::isPointInsideRect(const sf::Vector2f& point, float top, float bottom, float left, float right)
{
	return point.x < right && point.x > left && point.y > top && point.y < bottom;
}

float uiMath::distance(const sf::Vector2f& a, const sf::Vector2f b)
{
	sf::Vector2f disp = a - b;
	return sqrt(disp.x * disp.x + disp.y * disp.y);
}

float uiMath::dot(const sf::Vector2f& a, const sf::Vector2f b)
{
	return a.x * b.x + a.y * b.y;
}
float uiMath::lengthSquared(const sf::Vector2f&& v)
{
	return v.x * v.x + v.y * v.y;
}

sf::Vector2f uiMath::rightVector(sf::Vector2f&& a)
{
	return sf::Vector2f(a.y, -a.x);
}
sf::Vector2f uiMath::normalized(sf::Vector2f&& a)
{
	return a / sqrt(a.x * a.x + a.y * a.y);
}

void uiMath::HSVtoRGB(int H, double S, double V, sf::Color& output)
{
	double C = S * V;
	double X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
	double m = V - C;
	double Rs, Gs, Bs;

	if (H >= 0 && H < 60) {
		Rs = C;
		Gs = X;
		Bs = 0;
	}
	else if (H >= 60 && H < 120) {
		Rs = X;
		Gs = C;
		Bs = 0;
	}
	else if (H >= 120 && H < 180) {
		Rs = 0;
		Gs = C;
		Bs = X;
	}
	else if (H >= 180 && H < 240) {
		Rs = 0;
		Gs = X;
		Bs = C;
	}
	else if (H >= 240 && H < 300) {
		Rs = X;
		Gs = 0;
		Bs = C;
	}
	else {
		Rs = C;
		Gs = 0;
		Bs = X;
	}

	output.r = (Rs + m) * 255;
	output.g = (Gs + m) * 255;
	output.b = (Bs + m) * 255;
}

// Return minimum distance between line segment vw and point p
// used to delete connections
float uiMath::minimumDistance(const sf::Vector2f& v, const sf::Vector2f& w, const sf::Vector2f& p)
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
