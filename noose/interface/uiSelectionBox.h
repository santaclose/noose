#pragma once
#include <SFML/Graphics.hpp>

class uiSelectionBox
{
private:
	bool m_onScreen = false;
	std::vector<sf::Vertex> m_box;
	std::vector<sf::Text*> m_optionTexts;
	int m_currentOptionCount = 0;
	// need different shader per object to set different boxcounts
	sf::Shader m_mouseOverShader;

public:
	void initialize();
	void display(const sf::Vector2f& position, const std::vector<std::string>& options);
	int mouseOver(const sf::Vector2f& position);
	void hide();
	void draw(sf::RenderWindow& window, const sf::Vector2f& mousePos);
	void terminate();

	bool isVisible();
};