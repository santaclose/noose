#include "uiViewport.h"
#include <iostream>

sf::RenderTexture* uiViewport::outputImage = nullptr;
float uiViewport::currentZoom;
sf::RenderWindow* uiViewport::renderWindow;

bool uiViewport::panning = false;
sf::Vector2f uiViewport::lastMouseScreenPos;
sf::Vector2f uiViewport::currentMouseScreenPos;
sf::View uiViewport::theView;
sf::Vector2f uiViewport::viewPosition = sf::Vector2f(0.0, 0.0);
sf::Texture uiViewport::imageLimitTexture;
sf::Sprite uiViewport::imageLimitSprite;
sf::RectangleShape uiViewport::backgroundRectangle;
sf::Shader uiViewport::checkerShader;
sf::Shader uiViewport::invertShader;


void uiViewport::updateView(float width, float height)
{
	sf::FloatRect visibleArea(uiViewport::viewPosition.x, uiViewport::viewPosition.y, width, height);
	uiViewport::theView = sf::View(visibleArea);
}

void uiViewport::initialize(sf::RenderWindow& theRenderWindow)
{
	renderWindow = &theRenderWindow;
	updateView(renderWindow->getSize().x, renderWindow->getSize().y);
	backgroundRectangle.setSize((sf::Vector2f) renderWindow->getSize());

	imageLimitTexture.loadFromFile("res/images/imageLimit.png");
	imageLimitSprite = sf::Sprite(imageLimitTexture);

	// checker background
	if (!checkerShader.loadFromFile("res/shaders/checker.shader", sf::Shader::Fragment))
		std::cout << "[UI] Failed to load alpha background shader\n";

	// dark mode
	if (!invertShader.loadFromFile("res/shaders/invert.shader", sf::Shader::Fragment))
		std::cout << "[UI] Failed to load dark mode shader\n";
}


void uiViewport::onPollEvent(const sf::Event& e, sf::Vector2i& mousePos)
{
	switch (e.type)
	{
		case sf::Event::Resized:
		{
			// update the view to the new size of the window
			updateView(e.size.width, e.size.height);
			backgroundRectangle.setSize((sf::Vector2f) renderWindow->getSize());
			break;
		}
		case sf::Event::MouseButtonPressed:
		{
			if (e.mouseButton.button == sf::Mouse::Middle)
			{
				panning = true;
				lastMouseScreenPos = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
			}
			break;
		}
		case sf::Event::MouseButtonReleased:
		{
			panning = false;
			break;
		}
		case sf::Event::MouseMoved:
		{
			currentMouseScreenPos = sf::Vector2f(e.mouseMove.x, e.mouseMove.y);
			sf::Vector2f displacement = -lastMouseScreenPos + currentMouseScreenPos;
			if (panning)
			{
				viewPosition -= displacement;// * currentZoom;
				updateView(renderWindow->getSize().x, renderWindow->getSize().y);
			}

			lastMouseScreenPos = currentMouseScreenPos;
			break;
		}
	}
}

void uiViewport::draw()
{
	const sf::Vector2u& windowSize = renderWindow->getSize();
	sf::View staticView(sf::Vector2f(windowSize.x / 2.0, windowSize.y / 2.0),
		sf::Vector2f(windowSize.x, windowSize.y));
	renderWindow->setView(staticView);

	renderWindow->draw(backgroundRectangle, &checkerShader);
	
	renderWindow->setView(theView);
	if (outputImage != nullptr)
	{
		sf::Sprite spr(outputImage->getTexture());
		renderWindow->draw(spr);

		int x1 = -9, y1 = -9;
		int x2 = outputImage->getSize().x - 8, y2 = outputImage->getSize().y - 8;
		imageLimitSprite.setPosition(x1, y1);
		renderWindow->draw(imageLimitSprite, &invertShader);
		imageLimitSprite.setPosition(x1, y2);
		renderWindow->draw(imageLimitSprite, &invertShader);
		imageLimitSprite.setPosition(x2, y1);
		renderWindow->draw(imageLimitSprite, &invertShader);
		imageLimitSprite.setPosition(x2, y2);
		renderWindow->draw(imageLimitSprite, &invertShader);
	}
}