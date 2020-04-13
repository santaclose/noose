#pragma once

#include <SFML/Graphics.hpp>

namespace uiCategoryPusher
{
	void initialize(sf::RenderWindow& window, const sf::Vector2i* mouseScreenPosPointer);
	void terminate();

	void onPollEvent(const sf::Event& e);
	void draw();
	bool isActive();
}