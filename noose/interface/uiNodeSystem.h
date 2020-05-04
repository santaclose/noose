#pragma once

#include <SFML/Graphics.hpp>
#include "../nodeData.h"

namespace uiNodeSystem
{
	enum PushMode { OnCursorPosition = 0, Centered = 1 };

	void initialize(sf::RenderWindow& theRenderWindow, const sf::Vector2i* mouseScreenPosPointer);
	void terminate();

	void pushNewNode(const nodeData* nData, PushMode mode = PushMode::OnCursorPosition);
	void onPollEvent(const sf::Event& e);
	void draw();

	void setOnNodeSelectedCallback(void (*functionPointer)(int));
	void setOnNodeDeletedCallback(void (*functionPointer)(int)); // called just before deleting the node
	void setOnNodeChangedCallback(void (*functionPointer)(int));
}