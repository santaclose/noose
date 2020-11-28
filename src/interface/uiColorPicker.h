#pragma once
#include <SFML/Graphics.hpp>

namespace uiColorPicker
{
	enum SelectionState {
		None, Color, Intensity, Alpha
	};

	void initialize();
	void setOnColorSelectCallback(void (*onSetColor)(sf::Color*));
	void tick();
	void terminate();
	void show(sf::Color* newPointer, void (*onCloseWindow)());
	void updatePositionsFromColor();
	void hide();
}