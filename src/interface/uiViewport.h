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
	void onNodeChanged(int uiNodeId);
	void onNodeDeleted(int uiNodeId);
	void centerView();
	void draw();

	void setOnSelectedPositionChangeCallback(void (*onSelectedPositionChange)(const sf::Vector2i& vec));
	void setOnToggleNodeEditorVisibilityCallback(void (*onToggleNodeEditorVisibility)());

	void getView(int& zoom, sf::Vector2f& position);
	void setView(int zoom, const sf::Vector2f& position);

	// project file loading
	void onProjectFileParseViewportState(int viewportZoom,
		float viewportViewPositionX, float viewportViewPositionY);
};

