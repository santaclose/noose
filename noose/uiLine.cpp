#include "uiLine.h"

uiLine::uiLine(const sf::Vector2f& pinPosition, void* theNodeA, int thePinA, int theDirection)
{
	posA = posB = pinPosition;
	nodeA = theNodeA;
	pinA = thePinA;
	direction = theDirection;
	available = false;
}

std::ostream& operator<<(std::ostream& os, const uiLine& l)
{
	os << "uiLine:\n\tavailable: " << l.available <<
		"\n\tnodeA: " << l.nodeA <<
		"\n\tnodeB: " << l.nodeB;
	return os;
}