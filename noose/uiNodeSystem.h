#pragma once

#include <SFML/Graphics.hpp>

namespace uiNodeSystem
{
	enum Types
	{	
		Integer,
		Float,
		Vector2i,
		Image,
		Color
	};

	extern float currentZoom;
	extern sf::Font font;
	extern sf::RenderWindow* renderWindow;

	void initialize(sf::RenderWindow& theRenderWindow);
	void terminate();

	void pushNewNode(const std::string& name, int numberOfInputPins, int numberOfOutputPins, const uiNodeSystem::Types* pinTypes, const std::string* pinNames, sf::Vector2i* initialScreenPosition = nullptr);
	void onPollEvent(const sf::Event& e, sf::Vector2i& mousePos);
	void draw(sf::RenderWindow& window);
}