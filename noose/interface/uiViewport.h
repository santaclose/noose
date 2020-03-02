#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class uiViewport
{
public:
	static const std::vector<void*>* selectedNodeDataPointers;
	static const int* selectedNodePinTypes;
	static int selectedNodeOutputPinCount;

	static float currentZoom;
	static sf::RenderWindow* renderWindow;

	static void initialize(sf::RenderWindow& theRenderWindow);
	static void onPollEvent(const sf::Event& e, sf::Vector2i& mousePos);
	static void draw();
private:
	static void updateView(float width, float height);

private:
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

	// dark mode
	static sf::Shader invertShader;
};

