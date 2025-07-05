#pragma once
#include <SFML/Graphics.hpp>

namespace uiColorPicker
{
	enum SelectionState {
		None, Color, Intensity, Alpha
	};

	void initialize(const sf::Image& iconImage, void (*onWindowCreated)(sf::RenderWindow*), void (*onWindowDestroyed)());
	void tick();
	void terminate();
	void show(sf::Color* newPointer);
	void updatePositionsFromColor();
	void hide();

	void setonColorSelectedCallback(void (*onColorSelected)(sf::Color*));
}