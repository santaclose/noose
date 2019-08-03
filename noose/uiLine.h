#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

struct uiLine
{
	bool available = false;
	sf::Vector2f posA;
	sf::Vector2f posB;
	void* nodeA;
	void* nodeB;
	int pinA;
	int pinB;
	int direction; // if 0 pinA is an output pin

	uiLine(const sf::Vector2f& pinPosition, void* theNodeA, int thePinA, int theDirection);
};

std::ostream& operator<<(std::ostream& os, const uiLine& l);