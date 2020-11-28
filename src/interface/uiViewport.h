#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "uiSelectionBox.h"

namespace uiViewport
{
	void initialize(sf::RenderWindow& theRenderWindow, const sf::Vector2i* mouseScreenPosPointer);
	void setNodeData(int theSelectedNode, const std::vector<void*>* pointers, const int* pinTypes, int outputPinCount);
	void terminate();
	void hideSelectionBox();
	void onPollEvent(const sf::Event& e);
	void onNodeChanged(int theNode);
	void onNodeDeleted(int theNode);
	void draw();
};

