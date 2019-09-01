#pragma once

#include <SFML/Graphics.hpp>

namespace uiViewport
{
	extern sf::RenderTexture* outputImage;
	extern float currentZoom;
	extern sf::RenderWindow* renderWindow;

	void initialize(sf::RenderWindow& theRenderWindow);
	void onPollEvent(const sf::Event& e, sf::Vector2i& mousePos);
	void draw();
};

