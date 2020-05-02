#pragma once

#include <SFML/Graphics.hpp>

namespace uiHelp
{
	void initialize(sf::RenderWindow& window, const sf::Vector2i* mouseScreenPosPointer);
	void terminate();

	void onPollEvent(const sf::Event& e);
	void draw();
}