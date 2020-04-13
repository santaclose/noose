#include "uiViewport.h"
#include "uiFileSelector.h"
#include "uiData.h"
#include "../math/uiMath.h"
#include "../types.h"
#include "../utils.h"
#include <iostream>
#include <sstream>

#define IMAGE_MARGIN 24
#define BOTTOM_BAR_COLOR 0x222222ff
#define BOTTOM_BAR_HEIGHT 24
#define BOTTOM_BAR_FONT_SIZE 14
#define BOTTOM_BAR_TEXT_MARGIN 4

const std::vector<std::string> uiViewport::CONTEXT_MENU_OPTIONS = { "Save as png", "Save as jpg", "Save as bmp", "Save as tga"/* TODO: "Copy to clipboard" */};
int uiViewport::rightClickedImageIndex;
sf::Vector2f uiViewport::mouseWorldPos;
uiSelectionBox uiViewport::viewportSelectionBox;

int uiViewport::selectedNode = -1;
const std::vector<void*>* uiViewport::selectedNodeDataPointers = nullptr;
const int* uiViewport::selectedNodePinTypes = nullptr;
int uiViewport::selectedNodeOutputPinCount;

//float uiViewport::currentZoom;
sf::RenderWindow* uiViewport::vpRenderWindow;
const sf::Vector2i* uiViewport::vpMouseScreenPosPointer;

bool uiViewport::panning = false;
sf::Vector2f uiViewport::lastMouseScreenPos;
sf::View uiViewport::theView;
sf::Vector2f uiViewport::viewPosition = sf::Vector2f(0.0, 0.0);
sf::Texture uiViewport::imageLimitTexture;
sf::Sprite uiViewport::imageLimitSprite;
sf::RectangleShape uiViewport::backgroundRectangle;
sf::Shader uiViewport::checkerShader;
sf::Text uiViewport::bottomBarText;
sf::RectangleShape uiViewport::bottomBarRectangle;
sf::Shader uiViewport::invertShader;

void uiViewport::updateView(float width, float height)
{
	sf::FloatRect visibleArea(uiViewport::viewPosition.x, uiViewport::viewPosition.y, width, height);
	uiViewport::theView = sf::View(visibleArea);
}

int uiViewport::mouseOver(sf::Vector2f& mousePos)
{
	if (selectedNode == -1)
		return -1;

	sf::Vector2f cursor(0.0f, 0.0f);
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

void uiViewport::updateBottomBarText()
{
	std::stringstream newBottomBarText;
	bool firstPrinted = false;
	int c = selectedNodeDataPointers->size(); // count
	for (int i = c - selectedNodeOutputPinCount; i < c; i++)
	{
		if (selectedNodePinTypes[i] != NS_TYPE_IMAGE)
		{
			if (firstPrinted)
				newBottomBarText << " | ";
			else
				firstPrinted = true;
		}
		switch (selectedNodePinTypes[i])
		{
		case NS_TYPE_INT:
		{
			newBottomBarText << *((int*)(*selectedNodeDataPointers)[i]);
			break;
		}
		case NS_TYPE_FLOAT:
		{
			newBottomBarText << *((float*)(*selectedNodeDataPointers)[i]);
			break;
		}
		case NS_TYPE_VECTOR2I:
		{
			sf::Vector2i* data = (sf::Vector2i*) (*selectedNodeDataPointers)[i];
			newBottomBarText << data->x << "," << data->y;
			break;
		}
		case NS_TYPE_IMAGE:
			break;
		case NS_TYPE_COLOR:
		{
			newBottomBarText << "#" << utils::intToHex(((sf::Color*) (*selectedNodeDataPointers)[i])->toInteger());
			break;
		}
		}
	}
	bottomBarText.setString(newBottomBarText.str());
}

void uiViewport::initialize(sf::RenderWindow& theRenderWindow, const sf::Vector2i* mouseScreenPosPointer)
{
	vpMouseScreenPosPointer = mouseScreenPosPointer;
	vpRenderWindow = &theRenderWindow;
	updateView(vpRenderWindow->getSize().x, vpRenderWindow->getSize().y);

	bottomBarText = sf::Text("", uiData::monospaceFont, BOTTOM_BAR_FONT_SIZE);
	backgroundRectangle.setSize((sf::Vector2f) vpRenderWindow->getSize());

	bottomBarRectangle.setSize(sf::Vector2f(
		vpRenderWindow->getSize().x,
		BOTTOM_BAR_HEIGHT));
	bottomBarRectangle.setPosition(sf::Vector2f(
		0.0,
		vpRenderWindow->getSize().y - BOTTOM_BAR_HEIGHT));
	bottomBarText.setPosition(sf::Vector2f(
		BOTTOM_BAR_TEXT_MARGIN,
		vpRenderWindow->getSize().y - BOTTOM_BAR_HEIGHT + BOTTOM_BAR_TEXT_MARGIN));

	bottomBarRectangle.setFillColor(sf::Color(BOTTOM_BAR_COLOR));

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

void uiViewport::setNodeData(int theSelectedNode, const std::vector<void*>* pointers, const int* pinTypes, int outputPinCount)
{
	selectedNode = theSelectedNode;
	selectedNodeDataPointers = pointers;
	selectedNodePinTypes = pinTypes;
	selectedNodeOutputPinCount = outputPinCount;

	updateBottomBarText();
}

/*const std::vector<void*>* uiViewport::getNodeDataPointers()
{
	return selectedNodeDataPointers;
}*/
/*
void uiViewport::hideNodeData()
{
	selectedNodeDataPointers = nullptr;
}*/

void uiViewport::terminate()
{
	viewportSelectionBox.terminate();
}

void uiViewport::hideSelectionBox()
{
	viewportSelectionBox.hide();
}


void uiViewport::onPollEvent(const sf::Event& e)
{
	vpRenderWindow->setView(theView);
	mouseWorldPos = vpRenderWindow->mapPixelToCoords(*vpMouseScreenPosPointer);
	switch (e.type)
	{
		case sf::Event::Resized:
		{
			// update the view to the new size of the window
			updateView(e.size.width, e.size.height);
			backgroundRectangle.setSize((sf::Vector2f) vpRenderWindow->getSize());
			bottomBarRectangle.setSize(sf::Vector2f(
				vpRenderWindow->getSize().x,
				BOTTOM_BAR_HEIGHT));
			bottomBarRectangle.setPosition(sf::Vector2f(
				0.0,
				vpRenderWindow->getSize().y - BOTTOM_BAR_HEIGHT));
			bottomBarText.setPosition(sf::Vector2f(
				BOTTOM_BAR_TEXT_MARGIN,
				vpRenderWindow->getSize().y - BOTTOM_BAR_HEIGHT + BOTTOM_BAR_TEXT_MARGIN
				));
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
				if (index > -1 && viewportSelectionBox.isVisible())
				{
					std::string fileExtension;
					switch (index)
					{
					default:
					case 0:
						fileExtension = "png";
						break;
					case 1:
						fileExtension = "jpg";
						break;
					case 2:
						fileExtension = "bmp";
						break;
					case 3:
						fileExtension = "tga";
						break;
					}
					char* filePath = uiFileSelector::saveFileDialog(fileExtension);
					if (filePath != nullptr)
					{
						if (((sf::RenderTexture*)(*uiViewport::selectedNodeDataPointers)[rightClickedImageIndex])->getTexture().copyToImage().saveToFile(filePath))
							std::cout << "[UI] Image saved\n";
						else
							std::cout << "[UI] Could not save image\n";
						free(filePath);
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
			sf::Vector2f currentMouseScreenPos = sf::Vector2f(e.mouseMove.x, e.mouseMove.y);
			sf::Vector2f displacement = -lastMouseScreenPos + currentMouseScreenPos;
			if (panning)
			{
				viewPosition -= displacement;// * currentZoom;
				updateView(vpRenderWindow->getSize().x, vpRenderWindow->getSize().y);
			}

			lastMouseScreenPos = currentMouseScreenPos;
			break;
		}
	}
}

void uiViewport::onNodeChanged(int theNode)
{
	if (theNode == selectedNode)
	{
		updateBottomBarText();
	}
}

void uiViewport::onNodeDeleted(int theNode)
{
	if (theNode == selectedNode)
		selectedNode = -1;
}

void uiViewport::draw()
{
	const sf::Vector2u& windowSize = vpRenderWindow->getSize();
	sf::View staticView(
		sf::Vector2f(windowSize.x / 2.0, windowSize.y / 2.0),
		sf::Vector2f(windowSize.x, windowSize.y));

	vpRenderWindow->setView(staticView);
	vpRenderWindow->draw(backgroundRectangle, &checkerShader);
	
	vpRenderWindow->setView(theView);
	if (selectedNode > -1)
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
				vpRenderWindow->draw(spr);

				x1 = currentXOffset - 9;
				x2 = currentXOffset + imageSize.x - 8;
				y2 = imageSize.y - 8;
				imageLimitSprite.setPosition(x1, y1);
				vpRenderWindow->draw(imageLimitSprite, &invertShader);
				imageLimitSprite.setPosition(x1, y2);
				vpRenderWindow->draw(imageLimitSprite, &invertShader);
				imageLimitSprite.setPosition(x2, y1);
				vpRenderWindow->draw(imageLimitSprite, &invertShader);
				imageLimitSprite.setPosition(x2, y2);
				vpRenderWindow->draw(imageLimitSprite, &invertShader);

				currentXOffset += imageSize.x;
				currentXOffset += IMAGE_MARGIN;
				break;
			}
			default:
				break;
			}
		}

		vpRenderWindow->setView(staticView);
		vpRenderWindow->draw(bottomBarRectangle);
		vpRenderWindow->draw(bottomBarText);
	}
	vpRenderWindow->setView(theView);
	viewportSelectionBox.draw(*vpRenderWindow, mouseWorldPos);
}