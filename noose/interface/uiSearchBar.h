#pragma once

#include <SFML/Graphics.hpp>

class uiSearchBar
{
private:
	static bool searching;
	static sf::RectangleShape searchRectangle;
	static sf::Text searchText;
	static char searchBuffer[];
	static int searchBufferCurrentChar;
	static sf::RectangleShape resultsBar;
	static sf::Text resultsTexts[];
	static int currentResultCount;
	static sf::RenderWindow* renderWindow;

private:
	static void performSearch();
	static void clearSearch();

public:
	static void initialize(sf::RenderWindow& window);
	static void onPollEvent(const sf::Event& e, sf::Vector2i& mousePos);
	static void draw();

	//static bool userIsSearching();
};