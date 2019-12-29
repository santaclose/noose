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

	void pushNewNode(const void* nodeData, sf::Vector2i& initialScreenPosition);
	void onPollEvent(const sf::Event& e, sf::Vector2i& mousePos);
	void draw();

	void setOnNodeSelectedCallback(void* functionPointer);
	void setOnNodeDeletedCallback(void* functionPointer); // called just before deleting the node

	void deselectNode();

	void recalculatePropagationMatrices();
}