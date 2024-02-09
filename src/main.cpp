#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>

#include "interface/uiSelectionBox.h"
#include "interface/uiData.h"

#include "interface/uiNodeSystem.h"
#include "interface/uiColorPicker.h"

#include "interface/uiMenu.h"
#include "interface/uiCategoryPusher.h"
#include "interface/uiHelp.h"
#include "interface/uiFloatingButtonLayer.h"

#include "interface/uiSearchBar.h"
#include "interface/uiViewport.h"

#include "nodeProvider/nodeProvider.h"
#include "logic/nodeSystem.h"
#include "pathUtils.h"
#include "utils.h"

#define REDRAW_COUNT 5

static const sf::Color BACKGROUND_COLOR(0x222222ff);

#ifdef NOOSE_PLATFORM_WINDOWS
#include <windows.h>
#include <atlstr.h>
#endif

void onNodeSelected(int uiNodeId)
{
	std::cout << "[Main] Node " << uiNodeId << " selected\n";
	
	int nodeId = uiNodeSystem::getLogicalNodeId(uiNodeId);
	uiViewport::hideSelectionBox();
	uiViewport::setNodeData(
		uiNodeId,
		&nodeSystem::getDataPointersForNode(nodeId),
		nodeSystem::getPinTypesForNode(nodeId),
		nodeSystem::getOutputPinCountForNode(nodeId)
		);
}

// just before deleting the node
void onNodeDeleted(int uiNodeId)
{
	std::cout << "[Main] Node " << uiNodeId << " deleted\n";

	uiViewport::hideSelectionBox();
	uiViewport::onNodeDeleted(uiNodeId);
}

void onNodeChanged(int uiNodeId)
{
	uiViewport::onNodeChanged(uiNodeId);
}

#ifdef NOOSE_PLATFORM_WINDOWS
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	LPWSTR* szArglist;
	int argc;
	szArglist = CommandLineToArgvW(GetCommandLine(), &argc);
	std::string firstArgument = CW2A(szArglist[0]);
	std::string secondArgument;
	std::string thirdArgument;
	if (argc > 1) secondArgument = CW2A(szArglist[1]);
	if (argc > 2) thirdArgument = CW2A(szArglist[2]);
#else
int main(int argc, char** argv)
{
	std::string firstArgument(argv[0]);
	std::string secondArgument;
	std::string thirdArgument;
	if (argc > 1) secondArgument = std::string(argv[1]);
	if (argc > 2) thirdArgument = std::string(argv[2]);
#endif

	pathUtils::setProgramDirectory(firstArgument);
	// load nodes in memory
	nodeProvider::initialize();
	//nodeProvider::print();

	if (argc > 2) // command line mode
	{
		serializer::ParsingCallbacks parsingCallbacks;
		parsingCallbacks.OnParseNode = nodeSystem::onProjectFileLoadingAddNode;
		parsingCallbacks.OnParseNodeInput = nodeSystem::onProjectFileLoadingSetNodeInput;
		parsingCallbacks.OnParseConnection = nodeSystem::onProjectFileLoadingAddConnection;
		parsingCallbacks.OnFinishParsing = nodeSystem::onProjectFileLoadingFinish;
		if (utils::endsWith(secondArgument, ".nsj"))
			serializer::LoadFromFileJson(secondArgument, parsingCallbacks);
		else if (utils::endsWith(secondArgument, ".ns"))
			serializer::LoadFromFile(secondArgument, parsingCallbacks);
		else
		{
			std::cout << "[Main] Unknown file extension for file '" + secondArgument + "'\n";
			return EXIT_FAILURE;
		}
		std::ofstream outFile("out.txt");
		int a = 0, b = 0, c = 0;
		for (int i = 0; i <= thirdArgument.length(); i++)
		{
			if (i == thirdArgument.length() || thirdArgument[i] == ',')
			{
				c = i;
				{
					int nodeIndex = std::stoi(thirdArgument.substr(a, b - a));
					b++;
					int pinIndex = std::stoi(thirdArgument.substr(b, c - b));
					int dataType;
					const void* data = nodeSystem::getData(nodeIndex, pinIndex, dataType);
					switch (dataType)
					{
					case NS_TYPE_COLOR:
						outFile << utils::hexStringFromColor(*((sf::Color*)data)) << std::endl;
						break;
					case NS_TYPE_FLOAT:
						outFile << *((float*)data) << std::endl;
						break;
					case NS_TYPE_FONT:
						break;
					case NS_TYPE_IMAGE:
					{
						sf::RenderTexture* renderTexture = (sf::RenderTexture*)data;
						sf::Image outImage = renderTexture->getTexture().copyToImage();
						std::string outImageFileName = std::to_string(nodeIndex) + "_" + std::to_string(pinIndex) + ".png";
						if (outImage.saveToFile(outImageFileName))
							outFile << outImageFileName << std::endl;
						else
							std::cout << "[Main] Failed to save image file\n";
						break;
					}
					case NS_TYPE_INT:
						outFile << *((int*)data) << std::endl;
						break;
					case NS_TYPE_STRING:
						outFile << *((std::string*)data) << std::endl;
						break;
					case NS_TYPE_VECTOR2I:
					{
						sf::Vector2i* vector2i = (sf::Vector2i*)data;
						outFile << vector2i->x << ',' << vector2i->y << std::endl;
						break;
					}
					}
				}
				a = i + 1;
			}
			else if (thirdArgument[i] == ':')
				b = i;
		}

		return EXIT_SUCCESS;
	}

	int redrawCounter = REDRAW_COUNT;

#ifdef NOOSE_RELEASE
	utils::checkForUpdatesAsync();
#endif

	// Create the main window
	sf::RenderWindow windowA(sf::VideoMode({ 1200, 800 }), "node editor", 7U, sf::ContextSettings(), true);
	// Create the output window
	sf::RenderWindow windowB(sf::VideoMode({ 500, 500 }), "viewport", sf::Style::Resize);
	sf::Image iconImage;
	iconImage.loadFromFile(pathUtils::getAssetsDirectory() + "icon.png");
	windowA.setIcon({ iconImage.getSize().x, iconImage.getSize().y }, iconImage.getPixelsPtr());
	windowB.setIcon({ iconImage.getSize().x, iconImage.getSize().y }, iconImage.getPixelsPtr());

	// initialize interface components
	uiData::load();
	uiColorPicker::initialize(iconImage);

	sf::Vector2i mousePosWindowA;
	sf::Vector2i mousePosWindowB;

	uiNodeSystem::initialize(windowA, &mousePosWindowA);

	uiSearchBar::initialize(windowA, &mousePosWindowA);
	uiMenu::initialize(windowA, &mousePosWindowA);
	uiCategoryPusher::initialize(windowA, &mousePosWindowA);

	uiFloatingButtonLayer::initialize(windowA, &mousePosWindowA);
	uiFloatingButtonLayer::addButton(uiFloatingButtonLayer::ButtonPosition::BottomRight, "shaders/addFloatingButton.shader");
	uiFloatingButtonLayer::addButton(uiFloatingButtonLayer::ButtonPosition::TopLeft, "shaders/menuFloatingButton.shader");
	uiFloatingButtonLayer::addButton(uiFloatingButtonLayer::ButtonPosition::BottomLeft, '?');
	uiFloatingButtonLayer::addButton(uiFloatingButtonLayer::ButtonPosition::TopRight, "shaders/showViewportButton.shader");

	uiViewport::initialize(windowB, &mousePosWindowB, uiInputField::setVectorData);

	// node system callbacks
	uiNodeSystem::setOnNodeSelectedCallback(onNodeSelected);
	uiNodeSystem::setOnNodeDeletedCallback(onNodeDeleted);
	uiNodeSystem::setOnNodeChangedCallback(onNodeChanged);

	// load file if opening file
	if (argc > 1)
	{
		serializer::ParsingCallbacks parsingCallbacks;
		parsingCallbacks.OnParseConnection = uiNodeSystem::onProjectFileLoadingAddConnection;
		parsingCallbacks.OnParseNode = uiNodeSystem::onProjectFileLoadingAddNode;
		parsingCallbacks.OnParseNodeInput = uiNodeSystem::onProjectFileLoadingSetNodeInput;
		parsingCallbacks.OnParseNodeEditorState = uiNodeSystem::onProjectFileLoadingSetEditorState;
		parsingCallbacks.OnStartParsing = uiNodeSystem::onProjectFileLoadingStart;
		parsingCallbacks.OnParseViewportState = uiViewport::onProjectFileLoadingSetViewportState;
		if (utils::endsWith(secondArgument, ".nsj"))
			serializer::LoadFromFileJson(secondArgument, parsingCallbacks);
		else if (utils::endsWith(secondArgument, ".ns"))
			serializer::LoadFromFile(secondArgument, parsingCallbacks);
		else
		{
			int type = utils::typeFromExtension(secondArgument);
			switch (type)
			{
			case NS_TYPE_IMAGE:
				uiNodeSystem::pushImageNodeFromFile(secondArgument, uiNodeSystem::PushMode::Centered);
				break;
			case NS_TYPE_FONT:
				uiNodeSystem::pushFontNodeFromFile(secondArgument, uiNodeSystem::PushMode::Centered);
				break;
			default:
				std::cout << "[UI] Unknown file received as argument: " << secondArgument << std::endl;
				exit(1);
			}
		}
	}

	// Start the game loop
	while (windowA.isOpen() || windowB.isOpen())
	{
		// Process events
		sf::Event eventWindowA, eventWindowB;
		while (windowA.pollEvent(eventWindowA))
		{
			mousePosWindowA = sf::Mouse::getPosition(windowA);
			switch (eventWindowA.type)
			{
				case sf::Event::Closed:
				{
					windowA.close();
					windowB.close();
					break;
				}
			}

			if (eventWindowA.type == sf::Event::Resized)
			{
				uiNodeSystem::onPollEvent(eventWindowA);
				uiSearchBar::onPollEvent(eventWindowA);
				uiFloatingButtonLayer::onPollEvent(eventWindowA);
				uiCategoryPusher::updateButtonCenterCoordinates(uiFloatingButtonLayer::getButtonCenterCoords(uiFloatingButtonLayer::ButtonPosition::BottomRight));
				uiCategoryPusher::onPollEvent(eventWindowA);
				uiMenu::onPollEvent(eventWindowA);
			}
			else
			{
				if (uiSearchBar::isActive())
					uiSearchBar::onPollEvent(eventWindowA);
				else if (uiMenu::isActive())
					uiMenu::onPollEvent(eventWindowA);
				else if (uiCategoryPusher::isActive())
					uiCategoryPusher::onPollEvent(eventWindowA);
				else
				{
					if (eventWindowA.type == sf::Event::KeyPressed && !uiInputField::typingInteractionOngoing())
					{
						switch (eventWindowA.key.code)
						{
						case sf::Keyboard::Space:
							uiSearchBar::onSpacebarPressed();
							break;
						case sf::Keyboard::H:
							uiFloatingButtonLayer::active = !uiFloatingButtonLayer::active;
							break;
						}
					}

					uiFloatingButtonLayer::ButtonPosition bp = uiFloatingButtonLayer::onPollEvent(eventWindowA);
					if (bp == uiFloatingButtonLayer::ButtonPosition::None)
						uiNodeSystem::onPollEvent(eventWindowA);
					else
					{
						switch (bp)
						{
						case uiFloatingButtonLayer::ButtonPosition::BottomLeft:
							uiHelp::displayHelp();
							break;
						case uiFloatingButtonLayer::ButtonPosition::TopRight:
							windowB.requestFocus();
							break;
						case uiFloatingButtonLayer::ButtonPosition::BottomRight:
							uiCategoryPusher::onClickFloatingButton(uiFloatingButtonLayer::getButtonCenterCoords(uiFloatingButtonLayer::ButtonPosition::BottomRight));
							break;
						case uiFloatingButtonLayer::ButtonPosition::TopLeft:
							uiMenu::onClickFloatingButton(uiFloatingButtonLayer::getButtonCenterCoords(uiFloatingButtonLayer::ButtonPosition::TopLeft));
							break;
						}
					}
				}
			}
		}
		while (windowB.pollEvent(eventWindowB))
		{
			mousePosWindowB = sf::Mouse::getPosition(windowB);
			switch (eventWindowB.type)
			{
				case sf::Event::Closed:
				{
					windowA.close();
					windowB.close();
					break;
				}
			}
			uiViewport::onPollEvent(eventWindowB);
		}
		// Clear screen
		windowA.clear(BACKGROUND_COLOR);
		windowB.clear(BACKGROUND_COLOR);

		uiNodeSystem::draw();
		uiFloatingButtonLayer::draw();
		uiMenu::draw();
		uiCategoryPusher::draw();
		uiSearchBar::draw();

		uiViewport::draw();

		// Update the window
		windowA.display();
		windowB.display();

		uiColorPicker::tick();

		if (redrawCounter == 0)
		{
			sf::sleepUntilEvent();
			redrawCounter = REDRAW_COUNT;
		}
		else
			redrawCounter--;
	}
	uiMenu::terminate();
	uiCategoryPusher::terminate();
	uiFloatingButtonLayer::terminate();
	uiViewport::terminate();
	uiNodeSystem::terminate();
	uiColorPicker::terminate();
	nodeProvider::terminate();

	return EXIT_SUCCESS;
}