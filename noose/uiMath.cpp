#include "uiMath.h"
#include <cmath>
//#include <algorithm>

namespace uiMath
{
	const float DEG2RAD = 0.0174532925;
}

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

/* lines intersection */
/*
inline bool onSegment(const sf::Vector2f& p, const sf::Vector2f& q, const sf::Vector2f& r)
{
	if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
		q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
		return true;

	return false;
}

inline int orientation(const sf::Vector2f& p, const sf::Vector2f& q, const sf::Vector2f& r)
{
	int val = (q.y - p.y) * (r.x - q.x) -
		(q.x - p.x) * (r.y - q.y);

	if (val == 0) return 0;  // colinear 

	return (val > 0) ? 1 : 2; // clock or counterclock wise 
}
*/
/*bool uiMath::linesIntersect(const sf::Vector2f& p1, const sf::Vector2f& q1, const sf::Vector2f& p2, const sf::Vector2f& q2)
{/*
	// Find the four orientations needed for general and 
	// special cases 
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	// General case 
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases 
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1 
	if (o1 == 0 && onSegment(p1, p2, q1)) return true;

	// p1, q1 and q2 are colinear and q2 lies on segment p1q1 
	if (o2 == 0 && onSegment(p1, q2, q1)) return true;

	// p2, q2 and p1 are colinear and p1 lies on segment p2q2 
	if (o3 == 0 && onSegment(p2, p1, q2)) return true;

	// p2, q2 and q1 are colinear and q1 lies on segment p2q2 
	if (o4 == 0 && onSegment(p2, q1, q2)) return true;

	return false; // Doesn't fall in any of the above cases */
/*
	float ax = q1.x - p1.x;     // direction of line a
	float ay = q1.y - p1.y;     // ax and ay as above

	float bx = p2.x - q2.x;
	float by = p2.y - q2.y;

	float dx = p2.x - p1.x;
	float dy = p2.y - p1.y;

	float det = ax * by - ay * bx;

	if (det == 0) return false;

	float r = (dx * by - dy * bx) / det;
	float s = (ax * dy - ay * dx) / det;

	return !(r < 0 || r > 1 || s < 0 || s > 1);
}
*/
float uiMath::minimumDistance(const sf::Vector2f& v, const sf::Vector2f& w, const sf::Vector2f& p)
{
	// Return minimum distance between line segment vw and point p
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

sf::Vector2f operator*(const sf::Vector2f& v, const float f)
{
	return sf::Vector2f(v.x * f, v.y * f);
}

sf::Vector2f operator/(const sf::Vector2f& v, const float f)
{
	return sf::Vector2f(v.x / f, v.y / f);
}
