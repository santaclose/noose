#pragma once

#include <SFML/Graphics.hpp>

namespace uiMenu
{
	void initialize(sf::RenderWindow& window, const sf::Vector2i* mouseScreenPosPointer);
	void terminate();

	void onPollEvent(const std::optional<sf::Event>& e);

	//void updateButtonCenterCoordinates(const sf::Vector2f newPos); no need bc this button is on the top left corner

	void onClickFloatingButton(const sf::Vector2f& buttonPos);
	void draw();
	bool isActive();
}