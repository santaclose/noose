#pragma once
#include <SFML/Graphics.hpp>

namespace uiColorPicker
{
	extern void (*onSetColor)(sf::Color*);
	void initialize();
	void show(sf::Color* newPointer);
	void tick();
	void terminate();
}