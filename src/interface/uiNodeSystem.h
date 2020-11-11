#pragma once

#include <SFML/Graphics.hpp>
#include "../nodeData.h"
#include "uiNode.h"

namespace uiNodeSystem
{
	enum class PushMode { AtCursorPosition = 0, Centered = 1, AtPosition = 2 };

	void initialize(sf::RenderWindow& theRenderWindow, const sf::Vector2i* mouseScreenPosPointer);
	void terminate();

	void pushNewNode(
		const nodeData* nData,
		PushMode mode = PushMode::AtCursorPosition,
		bool nodeCenterInPosition = true,
		sf::Vector2f worldPos = {0.0, 0.0});
	void onPollEvent(const sf::Event& e);
	void draw();

	void setOnNodeSelectedCallback(void (*functionPointer)(int));
	void setOnNodeDeletedCallback(void (*functionPointer)(int)); // called just before deleting the node
	void setOnNodeChangedCallback(void (*functionPointer)(int));

	std::vector<uiNode*>& getUiNodeList();
	void setBoundInputFieldNode(int node);

	void clearEverything();
	void createConnection(int leftNode, int rightNode, int leftPin, int rightPin);
}