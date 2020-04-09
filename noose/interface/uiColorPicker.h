#pragma once
#include <SFML/Graphics.hpp>

enum SelectionState {
	None, Color, Intensity, Alpha
};

namespace uiColorPicker
{
	void initialize();
	void setOnColorSelectCallback(void (*onSetColor)(sf::Color*));
	void tick();
	void terminate();
	void show(sf::Color* newPointer);
	void hide();
}