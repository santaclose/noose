#include "uiShowViewport.h"

#include "../math/uiMath.h"
#include "../math/vectorOperators.h"

#include <iostream>

#define BUTTON_RADIUS 0.46f // uv space
#define BUTTON_COLOR 0x5a5a5aff
#define BUTTON_SIZE 50
#define MARGIN 18

sf::RenderWindow* shvRenderWindow;
const sf::Vector2i* shvMouseScreenPosPointer;
sf::VertexArray showViewportButtonVA;
sf::Shader showViewportButtonShader;
sf::RenderWindow* viewportWindow = nullptr;

void uiShowViewport::initialize(sf::RenderWindow& window, const sf::Vector2i* mouseScreenPosPointer, sf::RenderWindow* theViewportWindow)
{
	if (!showViewportButtonShader.loadFromFile("res/shaders/showViewportButton.shader", sf::Shader::Fragment))
		std::cout << "[UI] Failed to load viewport button shader\n";
	showViewportButtonShader.setUniform("radius", BUTTON_RADIUS);

	viewportWindow = theViewportWindow;
	shvRenderWindow = &window;
	shvMouseScreenPosPointer = mouseScreenPosPointer;

	showViewportButtonVA = sf::VertexArray(sf::Quads, 4);
	showViewportButtonVA[0].color = showViewportButtonVA[1].color = showViewportButtonVA[2].color = showViewportButtonVA[3].color = sf::Color(BUTTON_COLOR);

	showViewportButtonVA[0].texCoords.x = showViewportButtonVA[1].texCoords.x = showViewportButtonVA[0].texCoords.y = showViewportButtonVA[3].texCoords.y = 0.0;
	showViewportButtonVA[2].texCoords.x = showViewportButtonVA[3].texCoords.x = showViewportButtonVA[1].texCoords.y = showViewportButtonVA[2].texCoords.y = 1.0;

	showViewportButtonVA[0].position.x = showViewportButtonVA[1].position.x = shvRenderWindow->getSize().x - BUTTON_SIZE - MARGIN;
	showViewportButtonVA[2].position.x = showViewportButtonVA[3].position.x = shvRenderWindow->getSize().x - MARGIN;
	showViewportButtonVA[0].position.y = showViewportButtonVA[3].position.y = MARGIN;
	showViewportButtonVA[1].position.y = showViewportButtonVA[2].position.y = MARGIN + BUTTON_SIZE;
}

void uiShowViewport::terminate()
{
}

void uiShowViewport::onPollEvent(const sf::Event& e)
{
	switch (e.type)
	{
	case sf::Event::Resized:
		showViewportButtonVA[0].position.x = showViewportButtonVA[1].position.x = e.size.width - BUTTON_SIZE - MARGIN;
		showViewportButtonVA[2].position.x = showViewportButtonVA[3].position.x = e.size.width - MARGIN;
		break;
	case sf::Event::MouseButtonPressed:
	{
		if (e.mouseButton.button != sf::Mouse::Left)
			break;

		sf::Vector2f mousePosInUVSpace =
			(sf::Vector2f(shvMouseScreenPosPointer->x, shvMouseScreenPosPointer->y) - showViewportButtonVA[0].position) /
			BUTTON_SIZE;
		if (uiMath::distance(sf::Vector2f(0.5, 0.5), mousePosInUVSpace) < BUTTON_RADIUS)
			viewportWindow->requestFocus();
		break;
	}
	}
}

void uiShowViewport::draw()
{
	sf::FloatRect visibleArea(0, 0, shvRenderWindow->getSize().x, shvRenderWindow->getSize().y);
	shvRenderWindow->setView(sf::View(visibleArea));
	sf::Vector2f mousePos = sf::Vector2f(shvMouseScreenPosPointer->x, shvMouseScreenPosPointer->y);
	shvRenderWindow->draw(showViewportButtonVA, &showViewportButtonShader);
}
