#include "uiViewport.h"
#include "uiFileSelector.h"
#include "../math/uiMath.h"
#include "../types.h"
#include <iostream>

#define IMAGE_MARGIN 24

const std::vector<std::string> uiViewport::CONTEXT_MENU_OPTIONS = { "Save as output.png", "Save as"/*, "Copy to clipboard" */};
int uiViewport::rightClickedImageIndex;
sf::Vector2f uiViewport::mouseWorldPos;
uiSelectionBox uiViewport::viewportSelectionBox;

const std::vector<void*>* uiViewport::selectedNodeDataPointers = nullptr;
const int* uiViewport::selectedNodePinTypes = nullptr;
int uiViewport::selectedNodeOutputPinCount;

//float uiViewport::currentZoom;
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

int uiViewport::mouseOver(sf::Vector2f& mousePos)
{
	if (selectedNodeDataPointers == nullptr)
		return -1;

	sf::Vector2f cursor(0.0f, 0.0f);
	//unsigned int currentXOffset = 0;
	int c = selectedNodeDataPointers->size(); // count
	for (int i = c - selectedNodeOutputPinCount; i < c; i++)
	{
		switch (selectedNodePinTypes[i])
		{
		case NS_TYPE_IMAGE:
		{
			const sf::Vector2u& imageSize = ((sf::RenderTexture*)((*selectedNodeDataPointers)[i]))->getSize();
			if (uiMath::isPointInsideRect(mousePos, cursor, cursor + (sf::Vector2f) imageSize))
				return i;

			cursor.x += imageSize.x;
			cursor.x += IMAGE_MARGIN;
			break;
		}
		default:
			break;
		}
	}
	return -1;
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

	viewportSelectionBox.initialize();
}

void uiViewport::terminate()
{
	viewportSelectionBox.terminate();
}

void uiViewport::hideSelectionBox()
{
	viewportSelectionBox.hide();
}


void uiViewport::onPollEvent(const sf::Event& e, sf::Vector2i& mousePos)
{
	renderWindow->setView(theView);
	mouseWorldPos = renderWindow->mapPixelToCoords(mousePos);
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
			else if (e.mouseButton.button == sf::Mouse::Right)
			{
				rightClickedImageIndex = mouseOver(mouseWorldPos);
				if (rightClickedImageIndex > -1)
					viewportSelectionBox.display(mouseWorldPos, CONTEXT_MENU_OPTIONS);
			}
			else if (e.mouseButton.button == sf::Mouse::Left)
			{
				int index = viewportSelectionBox.mouseOver(mouseWorldPos);
				if (index > -1)
				{
					switch (index)
					{
					case 0: // save as output.png
						if (((sf::RenderTexture*)(*uiViewport::selectedNodeDataPointers)[rightClickedImageIndex])->getTexture().copyToImage().saveToFile("output.png"))
							std::cout << "[UI] Image saved as output.png\n";
						else
							std::cout << "[UI] Could not save image\n";
						break;
					case 1: // save as
					{
						char* filePath = uiFileSelector::selectFile(true);
						if (filePath != nullptr)
						{
							if (((sf::RenderTexture*)(*uiViewport::selectedNodeDataPointers)[rightClickedImageIndex])->getTexture().copyToImage().saveToFile(filePath))
								std::cout << "[UI] Image saved\n";
							else
								std::cout << "[UI] Could not save image\n";
							free(filePath);
						}
					}
					default:
						break;
					}
				}
				viewportSelectionBox.hide();
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
	if (selectedNodeDataPointers != nullptr)
	{
		unsigned int currentXOffset = 0;
		int x1 = -9, y1 = -9;
		int x2, y2;
		int c = selectedNodeDataPointers->size(); // count
		for (int i = c - selectedNodeOutputPinCount; i < c; i++)
		{
			switch (selectedNodePinTypes[i])
			{
			case NS_TYPE_IMAGE:
			{
				const sf::Vector2u& imageSize = ((sf::RenderTexture*)((*selectedNodeDataPointers)[i]))->getSize();
				sf::Sprite spr(((sf::RenderTexture*)((*selectedNodeDataPointers)[i]))->getTexture());
				spr.setPosition((float)currentXOffset, spr.getPosition().y);
				renderWindow->draw(spr);

				x1 = currentXOffset - 9;
				x2 = currentXOffset + imageSize.x - 8;
				y2 = imageSize.y - 8;
				imageLimitSprite.setPosition(x1, y1);
				renderWindow->draw(imageLimitSprite, &invertShader);
				imageLimitSprite.setPosition(x1, y2);
				renderWindow->draw(imageLimitSprite, &invertShader);
				imageLimitSprite.setPosition(x2, y1);
				renderWindow->draw(imageLimitSprite, &invertShader);
				imageLimitSprite.setPosition(x2, y2);
				renderWindow->draw(imageLimitSprite, &invertShader);

				currentXOffset += imageSize.x;
				currentXOffset += IMAGE_MARGIN;
				break;
			}
			default:
				break;
			}
		}
	}
	viewportSelectionBox.draw(*renderWindow, mouseWorldPos);
}