#pragma once
#include <SFML/Graphics.hpp>

namespace uiSelectionBox
{
	void initialize();
	void display(const sf::Vector2f& position, const std::vector<std::string>& options);
	int mouseOver(const sf::Vector2f& position);
	void hide();
	void draw(sf::RenderWindow& window, const sf::Vector2f& mousePos);
	void terminate();
}