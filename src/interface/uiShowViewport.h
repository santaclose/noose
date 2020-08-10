#pragma once

#include <SFML/Graphics.hpp>

namespace uiShowViewport
{
	void initialize(sf::RenderWindow& window, const sf::Vector2i* mouseScreenPosPointer, sf::RenderWindow* theViewportWindow);
	void terminate();

	void onPollEvent(const sf::Event& e);
	void draw();
}