#pragma once

#include <SFML/Graphics.hpp>

namespace uiCategoryPusher
{
	void initialize(sf::RenderWindow& window, const sf::Vector2i* mouseScreenPosPointer);
	void terminate();

	void onPollEvent(const sf::Event& e);

	void showCategorySelectionBox();
	void showNodeSelectionBox(int index);

	void updateButtonCenterCoordinates(const sf::Vector2f& newPos);

	void onClickFloatingButton(const sf::Vector2f& buttonPos);
	void draw();
	bool isActive();
}