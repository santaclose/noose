#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "uiSelectionBox.h"

class uiViewport
{
private:
	static void updateView(float width, float height);
	static int mouseOver(sf::Vector2f& mousePos);

public:
	static void initialize(sf::RenderWindow& theRenderWindow);
	static void setNodeData(const std::vector<void*>* pointers, const int* pinTypes, int outputPinCount);
	static const std::vector<void*>* getNodeDataPointers();
	static void hideNodeData();
	static void terminate();
	static void hideSelectionBox();
	static void onPollEvent(const sf::Event& e, sf::Vector2i& mousePos);
	static void draw();

private:
	static const std::vector<void*>* selectedNodeDataPointers;
	static const int* selectedNodePinTypes;
	static int selectedNodeOutputPinCount;

	//static float currentZoom;
	static sf::RenderWindow* renderWindow;

	static const std::vector<std::string> CONTEXT_MENU_OPTIONS;
	static int rightClickedImageIndex;
	static sf::Vector2f mouseWorldPos;
	static uiSelectionBox viewportSelectionBox;

	static bool panning;

	static sf::Vector2f lastMouseScreenPos;
	static sf::Vector2f currentMouseScreenPos;

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

