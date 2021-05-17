#include "uiViewport.h"
#include "uiFileSelector.h"
#include "uiData.h"
#include "../math/uiMath.h"
#include "../types.h"
#include "../utils.h"
#include <iostream>
#include <sstream>

#define INITIAL_VIEWPORT_SIZE 500

#define PERCENTAGE_TEXT_MARGIN 20

#define MAX_ZOOM 2
#define MIN_ZOOM 30

#define IMAGE_MARGIN 24
#define BOTTOM_BAR_COLOR 0x222222ff
#define BOTTOM_BAR_HEIGHT 24
#define FONT_SIZE 14
#define BOTTOM_BAR_TEXT_MARGIN 4

namespace uiViewport {

	const std::vector<std::string> SAVE_CONTEXT_MENU_OPTIONS = { "Save as png", "Save as jpg", "Save as bmp", "Save as tga"/* TODO: "Copy to clipboard" */ };
	int rightClickedImageIndex;
	sf::Vector2f mouseWorldPos;
	::uiSelectionBox saveSelectionBox;

	int selectedNode = -1;
	const std::vector<void*>* selectedNodeDataPointers = nullptr;
	const int* selectedNodePinTypes = nullptr;
	int selectedNodeOutputPinCount;

	sf::Text zoomPercentageText;
	int zoomInt = 10;
	float currentZoom = 1.0f;
	sf::RenderWindow* renderWindow;
	const sf::Vector2i* mouseScreenPosPointer;

	bool panning = false;
	sf::Vector2f lastMouseScreenPos;
	sf::View theView;
	sf::Vector2f viewPosition = sf::Vector2f(INITIAL_VIEWPORT_SIZE / 2.0 - IMAGE_MARGIN, INITIAL_VIEWPORT_SIZE / 2.0 - IMAGE_MARGIN);
	sf::Texture imageLimitTexture;
	sf::Sprite imageLimitSprite;
	sf::RectangleShape backgroundRectangle;
	sf::Shader checkerShader;
	sf::Text bottomBarText;
	sf::RectangleShape bottomBarRectangle;
	sf::Shader invertShader;

	void updateView();
	int mouseOver(sf::Vector2f& mousePos);
	void updateBottomBarText();
}

void uiViewport::updateView()
{
	theView = sf::View(viewPosition, (sf::Vector2f)renderWindow->getSize());
	theView.zoom(currentZoom);
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
	uiViewport::mouseScreenPosPointer = mouseScreenPosPointer;
	renderWindow = &theRenderWindow;
	updateView();

	zoomPercentageText = sf::Text("100%", uiData::monospaceFont, FONT_SIZE);
	zoomPercentageText.setPosition(sf::Vector2f(
		-zoomPercentageText.getLocalBounds().width + renderWindow->getSize().x - PERCENTAGE_TEXT_MARGIN,
		PERCENTAGE_TEXT_MARGIN
		));

	bottomBarText = sf::Text("", uiData::monospaceFont, FONT_SIZE);
	backgroundRectangle.setSize((sf::Vector2f) renderWindow->getSize());

	bottomBarRectangle.setSize(sf::Vector2f(
		renderWindow->getSize().x,
		BOTTOM_BAR_HEIGHT));
	bottomBarRectangle.setPosition(sf::Vector2f(
		0.0,
		renderWindow->getSize().y - BOTTOM_BAR_HEIGHT));
	bottomBarText.setPosition(sf::Vector2f(
		BOTTOM_BAR_TEXT_MARGIN,
		renderWindow->getSize().y - BOTTOM_BAR_HEIGHT + BOTTOM_BAR_TEXT_MARGIN));

	bottomBarRectangle.setFillColor(sf::Color(BOTTOM_BAR_COLOR));

	imageLimitTexture.loadFromFile(utils::getProgramDirectory() + "assets/images/imageLimit.png");
	imageLimitSprite = sf::Sprite(imageLimitTexture);

	// checker background
	if (!checkerShader.loadFromFile(utils::getProgramDirectory() + "assets/shaders/checker.shader", sf::Shader::Fragment))
		std::cout << "[UI] Failed to load alpha background shader\n";

	// dark mode
	if (!invertShader.loadFromFile(utils::getProgramDirectory() + "assets/shaders/invert.shader", sf::Shader::Fragment))
		std::cout << "[UI] Failed to load dark mode shader\n";

	saveSelectionBox.initialize();
}

void uiViewport::setNodeData(int theSelectedNode, const std::vector<void*>* pointers, const int* pinTypes, int outputPinCount)
{
	selectedNode = theSelectedNode;
	selectedNodeDataPointers = pointers;
	selectedNodePinTypes = pinTypes;
	selectedNodeOutputPinCount = outputPinCount;

	updateBottomBarText();
}

void uiViewport::terminate()
{
	saveSelectionBox.terminate();
}

void uiViewport::hideSelectionBox()
{
	saveSelectionBox.hide();
}


void uiViewport::onPollEvent(const sf::Event& e)
{
	renderWindow->setView(theView);
	mouseWorldPos = renderWindow->mapPixelToCoords(*mouseScreenPosPointer);
	switch (e.type)
	{
		case sf::Event::Resized:
		{
			// update the view to the new size of the window
			updateView();
			backgroundRectangle.setSize((sf::Vector2f) renderWindow->getSize());
			bottomBarRectangle.setSize(sf::Vector2f(
				renderWindow->getSize().x,
				BOTTOM_BAR_HEIGHT));
			bottomBarRectangle.setPosition(sf::Vector2f(
				0.0,
				renderWindow->getSize().y - BOTTOM_BAR_HEIGHT));
			bottomBarText.setPosition(sf::Vector2f(
				BOTTOM_BAR_TEXT_MARGIN,
				renderWindow->getSize().y - BOTTOM_BAR_HEIGHT + BOTTOM_BAR_TEXT_MARGIN
				));
			zoomPercentageText.setPosition(sf::Vector2f(
				-zoomPercentageText.getLocalBounds().width + renderWindow->getSize().x - PERCENTAGE_TEXT_MARGIN,
				PERCENTAGE_TEXT_MARGIN
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
					saveSelectionBox.display((sf::Vector2f)(*mouseScreenPosPointer), SAVE_CONTEXT_MENU_OPTIONS);
			}
			else if (e.mouseButton.button == sf::Mouse::Left)
			{
				int index = saveSelectionBox.mouseOver((sf::Vector2f)(*mouseScreenPosPointer));
				if (index > -1 && saveSelectionBox.isVisible())
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
				saveSelectionBox.hide();
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
				viewPosition -= displacement * currentZoom;
				updateView();
			}

			lastMouseScreenPos = currentMouseScreenPos;
			break;
		}
		case sf::Event::MouseWheelScrolled:
		{
			zoomInt -= e.mouseWheelScroll.delta;

			//clamp from min to max zoom
			if (zoomInt < MAX_ZOOM) zoomInt = MAX_ZOOM; else if (zoomInt > MIN_ZOOM) zoomInt = MIN_ZOOM;
			currentZoom = zoomInt / 10.0f;
			std::stringstream percentageStream;
			percentageStream << (1.0f / currentZoom) * 100.0f << '%';
			zoomPercentageText.setString(percentageStream.str());
			zoomPercentageText.setPosition(sf::Vector2f(
				-zoomPercentageText.getLocalBounds().width + renderWindow->getSize().x - PERCENTAGE_TEXT_MARGIN,
				PERCENTAGE_TEXT_MARGIN
			));
			updateView();
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
	const sf::Vector2u& windowSize = renderWindow->getSize();
	sf::View staticView(
		sf::Vector2f(windowSize.x / 2.0, windowSize.y / 2.0),
		sf::Vector2f(windowSize.x, windowSize.y));

	renderWindow->setView(staticView);
	renderWindow->draw(backgroundRectangle, &checkerShader);
	
	renderWindow->setView(theView);
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

		renderWindow->setView(staticView);
		renderWindow->draw(bottomBarRectangle);
		renderWindow->draw(bottomBarText);
		renderWindow->draw(zoomPercentageText);
	}
	renderWindow->setView(staticView);
	saveSelectionBox.draw(*renderWindow, (sf::Vector2f)(*mouseScreenPosPointer));
}