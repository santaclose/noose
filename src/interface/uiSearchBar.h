#pragma once

#include <SFML/Graphics.hpp>

namespace uiSearchBar
{
	void initialize(sf::RenderWindow& window, const sf::Vector2i* mouseScreenPosPointer);
	void onPollEvent(const sf::Event& e);
	void draw();
	bool isActive();
}