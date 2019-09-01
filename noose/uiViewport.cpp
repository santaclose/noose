#include "uiViewport.h"


namespace uiViewport {

	// public
	sf::RenderTexture* outputImage = nullptr;
	//float currentZoom = 1.0f;
	sf::RenderWindow* renderWindow;

	// private

	bool panning = false;

	sf::Vector2f lastMouseScreenPos;
	sf::Vector2f currentMouseScreenPos;

	sf::View theView; // view with pan and zoom transformations
	sf::Vector2f viewPosition = sf::Vector2f(0.0, 0.0);
	//int zoomInt = 10;
}

inline void updateView(float width, float height)
{
	//uiViewport::theView = sf::View(uiViewport::viewPosition, (sf::Vector2f)uiViewport::renderWindow->getSize());
	//uiViewport::theView.zoom(uiViewport::currentZoom);

	sf::FloatRect visibleArea(uiViewport::viewPosition.x, uiViewport::viewPosition.y, width, height);
	uiViewport::theView = sf::View(visibleArea);
}

void uiViewport::initialize(sf::RenderWindow& theRenderWindow)
{
	renderWindow = &theRenderWindow;
	updateView(renderWindow->getSize().x, renderWindow->getSize().y);
}


void uiViewport::onPollEvent(const sf::Event& e, sf::Vector2i& mousePos)
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

void uiViewport::draw()
{
	renderWindow->setView(theView);
	if (outputImage != nullptr)
	{
		sf::Sprite spr(outputImage->getTexture());
		renderWindow->draw(spr);
	}
}