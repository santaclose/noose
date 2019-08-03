#pragma once

#include <SFML/Graphics.hpp>

namespace uiPushNodes
{
	void initialize(sf::RenderWindow& window);
	void onPollEvent(const sf::Event& e, sf::Vector2i& mousePos);
	void draw(sf::RenderWindow& window);
}