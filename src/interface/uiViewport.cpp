#include "uiViewport.h"
#include "uiData.h"
#include "../math/nooseMath.h"
#include "../types.h"
#include "../utils.h"
#include "../pathUtils.h"
#include <iostream>
#include <sstream>

#define INITIAL_VIEWPORT_SIZE 500

#define PERCENTAGE_TEXT_MARGIN 20

#define MAX_ZOOM 2
#define MIN_ZOOM 30

#define IMAGE_MARGIN 24
#define BOTTOM_BAR_COLOR 0x222222a0
#define BOTTOM_BAR_HEIGHT 24
#define FONT_SIZE 14
#define BOTTOM_BAR_TEXT_MARGIN 4

namespace uiViewport {

	const std::vector<std::string> SAVE_CONTEXT_MENU_OPTIONS = { "Save as png", "Save as jpg", "Save as bmp", "Save as tga", "Copy to clipboard" };
	int rightClickedImageIndex;
	sf::Vector2f mouseWorldPos;
	::uiSelectionBox saveSelectionBox;

	int selectedUiNode = -1;
	const std::vector<void*>* selectedNodeDataPointers = nullptr;
	const int* selectedNodePinTypes = nullptr;
	int selectedNodeOutputPinCount;

	sf::Text zoomPercentageText(uiData::monospaceFont, "");
	int zoomInt = 10;
	float currentZoom = 1.0f;
	sf::RenderWindow* renderWindow;
	const sf::Vector2i* mouseScreenPosPointer;

	void (*onSelectedPositionChange)(const sf::Vector2i& vec) = nullptr;
	void (*onToggleNodeEditorVisibility)() = nullptr;

	bool panning = false;
	sf::Vector2f lastMouseScreenPos;
	sf::View theView;
	sf::Vector2f viewPosition = sf::Vector2f(INITIAL_VIEWPORT_SIZE / 2.0 - IMAGE_MARGIN, INITIAL_VIEWPORT_SIZE / 2.0 - IMAGE_MARGIN);
	sf::Texture imageLimitTexture;
	sf::Sprite* imageLimitSprite;
	sf::RectangleShape backgroundRectangle;
	sf::Shader checkerShader;
	sf::Text bottomBarText(uiData::monospaceFont);
	sf::RectangleShape bottomBarRectangle;
	sf::Shader invertShader;

	bool pickingPosition = false;

	void updateView();
	int mouseOver(sf::Vector2f& mousePos);
	void updateBottomBarText();
}

void uiViewport::updateView()
{
	currentZoom = zoomInt / 10.0f;
	std::stringstream percentageStream;
	percentageStream << (1.0f / currentZoom) * 100.0f << '%';
	zoomPercentageText.setString(percentageStream.str());
	zoomPercentageText.setPosition(sf::Vector2f(
		-zoomPercentageText.getLocalBounds().size.x + renderWindow->getSize().x - PERCENTAGE_TEXT_MARGIN,
		PERCENTAGE_TEXT_MARGIN
	));

	theView = sf::View(viewPosition, (sf::Vector2f)renderWindow->getSize());
	theView.zoom(currentZoom);
}

int uiViewport::mouseOver(sf::Vector2f& mousePos)
{
	if (selectedUiNode == -1)
		return -1;

	sf::Vector2f cursor(0.0f, 0.0f);

	// iterate through all output pins
	int c = selectedNodeDataPointers->size();
	for (int i = c - selectedNodeOutputPinCount; i < c; i++)
	{
		switch (selectedNodePinTypes[i])
		{
		case NS_TYPE_IMAGE:
		{
			const sf::Vector2u& imageSize = ((sf::RenderTexture*)((*selectedNodeDataPointers)[i]))->getSize();
			if (nooseMath::isPointInsideRect(mousePos, cursor, cursor + (sf::Vector2f)imageSize))
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
	if (selectedUiNode == -1)
	{
		bottomBarText.setString("");
		return;
	}

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
			sf::Vector2i* data = (sf::Vector2i*)(*selectedNodeDataPointers)[i];
			newBottomBarText << data->x << "," << data->y;
			break;
		}
		case NS_TYPE_IMAGE:
			break;
		case NS_TYPE_COLOR:
		{
			newBottomBarText << "#" << utils::hexStringFromColor(*((sf::Color*)(*selectedNodeDataPointers)[i]));
			break;
		}
		case NS_TYPE_FONT:
		{
			sf::Font* data = (sf::Font*)(*selectedNodeDataPointers)[i];
			newBottomBarText << data->getInfo().family;
			break;
		}
		case NS_TYPE_STRING:
		{
			std::string* data = (std::string*)(*selectedNodeDataPointers)[i];
			newBottomBarText << *data;
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

	zoomPercentageText = sf::Text(uiData::monospaceFont, "100%", FONT_SIZE);
	zoomPercentageText.setPosition(sf::Vector2f(
		-zoomPercentageText.getLocalBounds().size.x + renderWindow->getSize().x - PERCENTAGE_TEXT_MARGIN,
		PERCENTAGE_TEXT_MARGIN
	));

	bottomBarText = sf::Text(uiData::monospaceFont, "", FONT_SIZE);
	backgroundRectangle.setSize((sf::Vector2f)renderWindow->getSize());

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

	if (!imageLimitTexture.loadFromFile(pathUtils::getAssetsDirectory() + "images/imageLimit.png"))
		std::cout << "[UI] Failed to load image limit image\n";
	imageLimitSprite = new sf::Sprite(imageLimitTexture);

	// checker background
	if (!checkerShader.loadFromFile(pathUtils::getAssetsDirectory() + "shaders/checker.shader", sf::Shader::Type::Fragment))
		std::cout << "[UI] Failed to load alpha background shader\n";

	// dark mode
	if (!invertShader.loadFromFile(pathUtils::getAssetsDirectory() + "shaders/invert.shader", sf::Shader::Type::Fragment))
		std::cout << "[UI] Failed to load dark mode shader\n";

	saveSelectionBox.initialize();
}

void uiViewport::setNodeData(int theSelectedNode, const std::vector<void*>* pointers, const int* pinTypes, int outputPinCount)
{
	selectedUiNode = theSelectedNode;
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


void uiViewport::onPollEvent(const std::optional<sf::Event>& e)
{
	renderWindow->setView(theView);
	mouseWorldPos = renderWindow->mapPixelToCoords(*mouseScreenPosPointer);


	if (e->is<sf::Event::Resized>())
	{
		// update the view to the new size of the window
		updateView();
		backgroundRectangle.setSize((sf::Vector2f)renderWindow->getSize());
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
			-zoomPercentageText.getLocalBounds().size.x + renderWindow->getSize().x - PERCENTAGE_TEXT_MARGIN,
			PERCENTAGE_TEXT_MARGIN
		));
	}
	else if (e->is<sf::Event::MouseButtonPressed>())
	{
		sf::Mouse::Button button = e->getIf<sf::Event::MouseButtonPressed>()->button;
		sf::Vector2i eventMousePos = e->getIf<sf::Event::MouseButtonPressed>()->position;
		if (button == sf::Mouse::Button::Middle)
		{
			panning = true;
			lastMouseScreenPos = sf::Vector2f(eventMousePos.x, eventMousePos.y);
		}
		else if (button == sf::Mouse::Button::Right)
		{
			rightClickedImageIndex = mouseOver(mouseWorldPos);
			if (rightClickedImageIndex > -1)
				saveSelectionBox.display((sf::Vector2f)(*mouseScreenPosPointer), SAVE_CONTEXT_MENU_OPTIONS);
		}
		else if (button == sf::Mouse::Button::Left)
		{
			int index = saveSelectionBox.mouseOver((sf::Vector2f)(*mouseScreenPosPointer));
			if (saveSelectionBox.isVisible())
			{
				if (index == 4)
				{
					sf::Image imageToCopy = ((sf::RenderTexture*)(*uiViewport::selectedNodeDataPointers)[rightClickedImageIndex])->getTexture().copyToImage();
					if (!utils::imageToClipboard(imageToCopy))
						std::cout << "[UI] Failed to copy image to clipboard\n";
					else
						utils::osShowNotification("Image copied to clipboard");
				}
				else if (index > -1)
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

					std::string destination = utils::osSaveFileDialog("Save file", fileExtension, "*." + fileExtension);
					if (destination.length() == 0)
					{
						std::cout << "[UI] Image not saved\n";
					}
					else
					{
						destination = destination + (pathUtils::fileHasExtension(destination.c_str(), fileExtension.c_str()) ? "" : '.' + fileExtension);
						if (((sf::RenderTexture*)(*uiViewport::selectedNodeDataPointers)[rightClickedImageIndex])->getTexture().copyToImage().saveToFile(destination))
							std::cout << "[UI] Image saved\n";
						else
							std::cout << "[UI] Could not save image\n";
					}
				}
				saveSelectionBox.hide();
			}
			else
			{
				pickingPosition = true;
				if (onSelectedPositionChange != nullptr)
					onSelectedPositionChange((sf::Vector2i)mouseWorldPos);
			}
		}
	}
	else if (e->is<sf::Event::MouseButtonReleased>())
	{
		sf::Mouse::Button button = e->getIf<sf::Event::MouseButtonReleased>()->button;
		if (button == sf::Mouse::Button::Middle)
		{
			panning = false;
		}
		else if (button == sf::Mouse::Button::Left)
		{
			pickingPosition = false;
		}
	}
	else if (e->is<sf::Event::MouseMoved>())
	{
		sf::Vector2i eventMousePos = e->getIf<sf::Event::MouseMoved>()->position;
		sf::Vector2f currentMouseScreenPos = sf::Vector2f(eventMousePos.x, eventMousePos.y);
		sf::Vector2f displacement = -lastMouseScreenPos + currentMouseScreenPos;
		if (panning)
		{
			viewPosition -= displacement * currentZoom;
			updateView();
		}

		if (pickingPosition)
			if (onSelectedPositionChange != nullptr)
				onSelectedPositionChange((sf::Vector2i)mouseWorldPos);

		lastMouseScreenPos = currentMouseScreenPos;
	}
	else if (e->is<sf::Event::MouseWheelScrolled>())
	{
		zoomInt -= e->getIf<sf::Event::MouseWheelScrolled>()->delta;

		// clamp from min to max zoom
		if (zoomInt < MAX_ZOOM)
			zoomInt = MAX_ZOOM;
		else if (zoomInt > MIN_ZOOM)
			zoomInt = MIN_ZOOM;
		updateView();
	}
	else if (e->is<sf::Event::KeyPressed>())
	{
		sf::Keyboard::Key keyCode = e->getIf<sf::Event::KeyPressed>()->code;
		if (keyCode == sf::Keyboard::Key::F)
			centerView();
		else if (keyCode == sf::Keyboard::Key::H)
			if (onToggleNodeEditorVisibility != nullptr)
				onToggleNodeEditorVisibility();
	}
}

// executed after all node computations are done in the entire graph
void uiViewport::onNodeChanged(int uiNodeId)
{
	updateBottomBarText();
}

void uiViewport::onNodeDeleted(int uiNodeId)
{
	if (uiNodeId == selectedUiNode)
		selectedUiNode = -1;
}

void uiViewport::centerView()
{
	sf::Vector2f boundingBoxMax = { 0.0f , 0.0f };
	int imageCount = 0;

	// iterate through all output pins
	int c = selectedNodeDataPointers->size();
	for (int i = c - selectedNodeOutputPinCount; i < c; i++)
	{
		switch (selectedNodePinTypes[i])
		{
		case NS_TYPE_IMAGE:
		{
			const sf::Vector2u& imageSize = ((sf::RenderTexture*)((*selectedNodeDataPointers)[i]))->getSize();
			boundingBoxMax.x += imageSize.x;
			boundingBoxMax.y = imageSize.y > boundingBoxMax.y ? imageSize.y : boundingBoxMax.y;
			imageCount++;
			break;
		}
		default:
			break;
		}
	}

	if (imageCount > 0)
	{
		boundingBoxMax.x += (imageCount - 1) * IMAGE_MARGIN;
		viewPosition = boundingBoxMax / 2.0f;
		updateView();
	}
}

#define FIXES_RENDERING_ISSUE_WITH_ODD_WINDOW_SIZES 0.01f

void uiViewport::draw()
{
	const sf::Vector2u& windowSize = renderWindow->getSize();
	sf::View staticView(
		sf::Vector2f(windowSize.x / 2.0, windowSize.y / 2.0),
		sf::Vector2f(windowSize.x, windowSize.y));

	renderWindow->setView(staticView);
	renderWindow->draw(backgroundRectangle, &checkerShader);

	renderWindow->setView(theView);
	if (selectedUiNode > -1)
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
				sf::RenderTexture* imageRenderTexture = (sf::RenderTexture*)((*selectedNodeDataPointers)[i]);
				imageRenderTexture->display();
				const sf::Vector2u& imageSize = imageRenderTexture->getSize();
				sf::Sprite spr(imageRenderTexture->getTexture());
				spr.setPosition({ (float)currentXOffset + FIXES_RENDERING_ISSUE_WITH_ODD_WINDOW_SIZES, spr.getPosition().y + FIXES_RENDERING_ISSUE_WITH_ODD_WINDOW_SIZES });
				renderWindow->draw(spr);

				x1 = currentXOffset - 9;
				x2 = currentXOffset + imageSize.x - 8;
				y2 = imageSize.y - 8;
				imageLimitSprite->setPosition({ (float)x1 + FIXES_RENDERING_ISSUE_WITH_ODD_WINDOW_SIZES, (float)y1 + FIXES_RENDERING_ISSUE_WITH_ODD_WINDOW_SIZES });
				renderWindow->draw(*imageLimitSprite, &invertShader);
				imageLimitSprite->setPosition({ (float)x1 + FIXES_RENDERING_ISSUE_WITH_ODD_WINDOW_SIZES, (float)y2 + FIXES_RENDERING_ISSUE_WITH_ODD_WINDOW_SIZES });
				renderWindow->draw(*imageLimitSprite, &invertShader);
				imageLimitSprite->setPosition({ (float)x2 + FIXES_RENDERING_ISSUE_WITH_ODD_WINDOW_SIZES, (float)y1 + FIXES_RENDERING_ISSUE_WITH_ODD_WINDOW_SIZES });
				renderWindow->draw(*imageLimitSprite, &invertShader);
				imageLimitSprite->setPosition({ (float)x2 + FIXES_RENDERING_ISSUE_WITH_ODD_WINDOW_SIZES, (float)y2 + FIXES_RENDERING_ISSUE_WITH_ODD_WINDOW_SIZES });
				renderWindow->draw(*imageLimitSprite, &invertShader);

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

void uiViewport::setOnSelectedPositionChangeCallback(void(*onSelectedPositionChange)(const sf::Vector2i& vec))
{
	uiViewport::onSelectedPositionChange = onSelectedPositionChange;
}

void uiViewport::setOnToggleNodeEditorVisibilityCallback(void(*onToggleNodeEditorVisibility)())
{
	uiViewport::onToggleNodeEditorVisibility = onToggleNodeEditorVisibility;
}

void uiViewport::getView(int& zoom, sf::Vector2f& position)
{
	zoom = zoomInt;
	position = viewPosition;
}

void uiViewport::setView(int zoom, const sf::Vector2f& position)
{
	zoomInt = zoom;
	viewPosition = position;
	updateView();
}

// project file loading

void uiViewport::onProjectFileParseViewportState(int viewportZoom, float viewportViewPositionX, float viewportViewPositionY)
{
	setView(viewportZoom, { viewportViewPositionX, viewportViewPositionY });
}
