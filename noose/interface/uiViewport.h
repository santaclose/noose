#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "uiSelectionBox.h"

class uiViewport
{
private:
	static void updateView();
	static int mouseOver(sf::Vector2f& mousePos);
	static void updateBottomBarText();

public:
	static void initialize(sf::RenderWindow& theRenderWindow, const sf::Vector2i* mouseScreenPosPointer);
	static void setNodeData(int theSelectedNode, const std::vector<void*>* pointers, const int* pinTypes, int outputPinCount);
	static void terminate();
	static void hideSelectionBox();
	static void onPollEvent(const sf::Event& e);
	static void onNodeChanged(int theNode);
	static void onNodeDeleted(int theNode);
	static void draw();

private:
	static int selectedNode;
	static const std::vector<void*>* selectedNodeDataPointers;
	static const int* selectedNodePinTypes;
	static int selectedNodeOutputPinCount;

	static sf::Text zoomPercentageText;
	static int zoomInt;
	static float currentZoom;
	static sf::RenderWindow* vpRenderWindow;
	static const sf::Vector2i* vpMouseScreenPosPointer;

	static const std::vector<std::string> CONTEXT_MENU_OPTIONS;
	static int rightClickedImageIndex;
	static sf::Vector2f mouseWorldPos;
	static uiSelectionBox viewportSelectionBox;

	static bool panning;
	static sf::Vector2f lastMouseScreenPos;

	static sf::View theView; // view with pan and zoom transformations
	static sf::Vector2f viewPosition;

	static sf::Texture imageLimitTexture;
	static sf::Sprite imageLimitSprite;

	// checker background
	static sf::RectangleShape backgroundRectangle;
	static sf::Shader checkerShader;

	// bottom bar
	static sf::RectangleShape bottomBarRectangle;
	static sf::Text bottomBarText;

	// dark mode
	static sf::Shader invertShader;
};

