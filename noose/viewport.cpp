#include "viewport.h"
#include <iostream>

sf::RenderTexture* viewport::outputImage;
float viewport::currentZoom;
sf::RenderWindow* viewport::renderWindow;

bool viewport::panning = false;
sf::Vector2f viewport::lastMouseScreenPos;
sf::Vector2f viewport::currentMouseScreenPos;
sf::View viewport::theView;
sf::Vector2f viewport::viewPosition = sf::Vector2f(0.0, 0.0);
sf::Texture viewport::imageLimitTexture;
sf::Sprite viewport::imageLimitSprite;
sf::Shader viewport::checkerShader;
sf::Shader viewport::invertShader;


void viewport::updateView(float width, float height)
{
	//viewport::theView = sf::View(viewport::viewPosition, (sf::Vector2f)viewport::renderWindow->getSize());
	//viewport::theView.zoom(viewport::currentZoom);

	sf::FloatRect visibleArea(viewport::viewPosition.x, viewport::viewPosition.y, width, height);
	viewport::theView = sf::View(visibleArea);
}

void viewport::initialize(sf::RenderWindow& theRenderWindow)
{
	renderWindow = &theRenderWindow;
	updateView(renderWindow->getSize().x, renderWindow->getSize().y);

	imageLimitTexture.loadFromFile("res/images/imageLimit.png");
	imageLimitSprite = sf::Sprite(imageLimitTexture);

	// checker background
	std::cout << "loading alpha background shader\n";
	checkerShader.loadFromFile("res/shaders/checker.shader", sf::Shader::Fragment);

	// dark mode
	std::cout << "loading dark mode shader\n";
	invertShader.loadFromFile("res/shaders/invert.shader", sf::Shader::Fragment);
}


void viewport::onPollEvent(const sf::Event& e, sf::Vector2i& mousePos)
{
	switch (e.type)
	{
		case sf::Event::Resized:
		{
			// update the view to the new size of the window
			updateView(e.size.width, e.size.height);
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

void viewport::draw()
{
	const sf::Vector2u& windowSize = renderWindow->getSize();
	sf::View staticView(sf::Vector2f(windowSize.x / 2.0, windowSize.y / 2.0),
		sf::Vector2f(windowSize.x, windowSize.y));
	renderWindow->setView(staticView);

	sf::Texture background;
	background.create(renderWindow->getSize().x, renderWindow->getSize().y);
	sf::Sprite backgroundSprite(background);
	renderWindow->draw(backgroundSprite, &checkerShader);
	
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