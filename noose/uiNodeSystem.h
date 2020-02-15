#pragma once

#include <SFML/Graphics.hpp>

namespace uiNodeSystem
{
	void initialize(sf::RenderWindow& theRenderWindow);
	void terminate();

	void pushNewNode(const void* nodeData, sf::Vector2i& initialScreenPosition);
	void onPollEvent(const sf::Event& e, sf::Vector2i& mousePos);
	void draw();

	void setOnNodeSelectedCallback(void (*functionPointer)(int));
	void setOnNodeDeletedCallback(void (*functionPointer)(int)); // called just before deleting the node

	void deselectNode();
}