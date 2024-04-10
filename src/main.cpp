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
#define VIEWPORT_MARGIN_ON_OPEN_FILE 120U

static const sf::Color BACKGROUND_COLOR(0x222222ff);

enum class OpenFileMode {
	None, Project, ProjectJson, Image, Font
};

#ifdef NOOSE_PLATFORM_WINDOWS
#include <windows.h>
#include <atlstr.h>
#endif

static sf::Image iconImage;
static bool nodeEditorIsVisible = false;
static sf::RenderWindow* nodeEditorWindow = nullptr;
static sf::Vector2i mousePosNodeEditor;
static sf::RenderWindow* viewportWindow = nullptr;
static sf::Vector2i mousePosViewport;

OpenFileMode guessOpenFileMode(int argc, const std::string& secondArgument)
{
	if (argc < 2)
		return OpenFileMode::None;
	if (utils::endsWith(secondArgument, ".nsj"))
		return OpenFileMode::ProjectJson;
	if (utils::endsWith(secondArgument, ".ns"))
		return OpenFileMode::Project;
	int type = utils::typeFromExtension(secondArgument);
	switch (type)
	{
	case NS_TYPE_IMAGE: return OpenFileMode::Image;
	case NS_TYPE_FONT: return OpenFileMode::Font;
	default:
		std::cout << "[Main] Unknown file received as argument: " << secondArgument << std::endl;
		return OpenFileMode::None;
	}
}

// callbacks
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

void onNodeDeleted(int uiNodeId) // called just before deleting the node
{
	std::cout << "[Main] Node " << uiNodeId << " deleted\n";

	uiViewport::hideSelectionBox();
	uiViewport::onNodeDeleted(uiNodeId);
}

void onNodeChanged(int uiNodeId)
{
	uiViewport::onNodeChanged(uiNodeId);
}

void onToggleNodeEditorVisibility()
{
	nodeEditorWindow->setVisible(nodeEditorIsVisible = !nodeEditorIsVisible);
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

	OpenFileMode openFileMode = guessOpenFileMode(argc, secondArgument);
	pathUtils::setProgramDirectory(firstArgument);
	// load nodes in memory
	nodeProvider::initialize();
	//nodeProvider::print();

	if (argc > 2) // command line mode
	{
		serializer::ParsingCallbacks parsingCallbacks;
		parsingCallbacks.OnParseNode = nodeSystem::onProjectFileParseNode;
		parsingCallbacks.OnParseNodeInput = nodeSystem::onProjectFileParseNodeInput;
		parsingCallbacks.OnParseConnection = nodeSystem::onProjectFileParseConnection;
		parsingCallbacks.OnFinishParsing = nodeSystem::onProjectFileFinishParsing;
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

	// initialize interface components
	uiData::load();
	iconImage.loadFromFile(pathUtils::getAssetsDirectory() + "icon.png");

	nodeEditorWindow = new sf::RenderWindow(sf::VideoMode({ 720, 720 }), "node editor", sf::Style::Default, sf::State::Windowed, sf::ContextSettings(), true);
	viewportWindow = new sf::RenderWindow(sf::VideoMode({ 720, 720 }), "viewport", sf::Style::Resize | sf::Style::Close, sf::State::Windowed);
	nodeEditorWindow->setIcon({ iconImage.getSize().x, iconImage.getSize().y }, iconImage.getPixelsPtr());
	viewportWindow->setIcon({ iconImage.getSize().x, iconImage.getSize().y }, iconImage.getPixelsPtr());
	if (openFileMode != OpenFileMode::Image)
	{
		nodeEditorWindow->setPosition(nodeEditorWindow->getPosition() - sf::Vector2i(720 / 2, 0));
		viewportWindow->setPosition(viewportWindow->getPosition() + sf::Vector2i(720 / 2, 0));
	}

	uiViewport::initialize(*viewportWindow, &mousePosViewport);
	uiColorPicker::initialize(iconImage);
	uiNodeSystem::initialize(*nodeEditorWindow, &mousePosNodeEditor);
	uiSearchBar::initialize(*nodeEditorWindow, &mousePosNodeEditor);
	uiMenu::initialize(*nodeEditorWindow, &mousePosNodeEditor);
	uiCategoryPusher::initialize(*nodeEditorWindow, &mousePosNodeEditor);
	uiFloatingButtonLayer::initialize(*nodeEditorWindow, &mousePosNodeEditor);
	uiFloatingButtonLayer::addButton(uiFloatingButtonLayer::ButtonPosition::BottomRight, "shaders/addFloatingButton.shader");
	uiFloatingButtonLayer::addButton(uiFloatingButtonLayer::ButtonPosition::TopLeft, "shaders/menuFloatingButton.shader");
	uiFloatingButtonLayer::addButton(uiFloatingButtonLayer::ButtonPosition::BottomLeft, '?');
	uiFloatingButtonLayer::addButton(uiFloatingButtonLayer::ButtonPosition::TopRight, "shaders/showViewportButton.shader");
	uiNodeSystem::setOnNodeSelectedCallback(onNodeSelected);
	uiNodeSystem::setOnNodeDeletedCallback(onNodeDeleted);
	uiNodeSystem::setOnNodeChangedCallback(onNodeChanged);
	uiViewport::setOnSelectedPositionChangeCallback(uiInputField::setVectorData);
	uiViewport::setOnToggleNodeEditorVisibilityCallback(onToggleNodeEditorVisibility);

	// load file if opening file
	sf::Vector2u imageToOpenSize = { ~0U, ~0U };
	switch (openFileMode)
	{
	case OpenFileMode::Project:
	case OpenFileMode::ProjectJson:
	{
		serializer::ParsingCallbacks parsingCallbacks;
		parsingCallbacks.OnStartParsing = uiNodeSystem::onProjectFileStartParsing;
		parsingCallbacks.OnParseNode = uiNodeSystem::onProjectFileParseNode;
		parsingCallbacks.OnParseNodeInput = uiNodeSystem::onProjectFileParseNodeInput;
		parsingCallbacks.OnParseConnection = uiNodeSystem::onProjectFileParseConnection;
		parsingCallbacks.OnParseNodeEditorState = uiNodeSystem::onProjectFileParseNodeEditorState;
		parsingCallbacks.OnParseViewportState = uiViewport::onProjectFileParseViewportState;
		if (openFileMode == OpenFileMode::ProjectJson)
			serializer::LoadFromFileJson(secondArgument, parsingCallbacks);
		else
			serializer::LoadFromFile(secondArgument, parsingCallbacks);
		break;
	}
	case OpenFileMode::Image:
		uiNodeSystem::pushImageNodeFromFile(secondArgument, uiNodeSystem::PushMode::Centered, true, { 0.0f, 0.0f }, &imageToOpenSize);
		break;
	case OpenFileMode::Font:
		uiNodeSystem::pushFontNodeFromFile(secondArgument, uiNodeSystem::PushMode::Centered);
		break;
	}

	if (imageToOpenSize.x != ~0U)
	{
		viewportWindow->setSize(imageToOpenSize + sf::Vector2u(VIEWPORT_MARGIN_ON_OPEN_FILE, VIEWPORT_MARGIN_ON_OPEN_FILE));
		auto desktop = sf::VideoMode::getDesktopMode();
		viewportWindow->setPosition({ (int)(desktop.size.x / 2 - viewportWindow->getSize().x / 2), (int)(desktop.size.y / 2 - viewportWindow->getSize().y / 2) });
		nodeEditorWindow->setVisible(nodeEditorIsVisible = false);
		uiViewport::centerView();
	}

	// Start the game loop
	while (nodeEditorWindow->isOpen() || viewportWindow->isOpen())
	{
		// Process events
		sf::Event nodeEditorEvent, viewportEvent;
		while (nodeEditorWindow->pollEvent(nodeEditorEvent))
		{
			mousePosNodeEditor = sf::Mouse::getPosition(*nodeEditorWindow);
			switch (nodeEditorEvent.type)
			{
				case sf::Event::Closed:
				{
					nodeEditorWindow->close();
					viewportWindow->close();
					break;
				}
			}

			if (nodeEditorEvent.type == sf::Event::Resized)
			{
				uiNodeSystem::onPollEvent(nodeEditorEvent);
				uiSearchBar::onPollEvent(nodeEditorEvent);
				uiFloatingButtonLayer::onPollEvent(nodeEditorEvent);
				uiCategoryPusher::updateButtonCenterCoordinates(uiFloatingButtonLayer::getButtonCenterCoords(uiFloatingButtonLayer::ButtonPosition::BottomRight));
				uiCategoryPusher::onPollEvent(nodeEditorEvent);
				uiMenu::onPollEvent(nodeEditorEvent);
			}
			else
			{
				if (uiSearchBar::isActive())
					uiSearchBar::onPollEvent(nodeEditorEvent);
				else if (uiMenu::isActive())
					uiMenu::onPollEvent(nodeEditorEvent);
				else if (uiCategoryPusher::isActive())
					uiCategoryPusher::onPollEvent(nodeEditorEvent);
				else
				{
					if (nodeEditorEvent.type == sf::Event::KeyPressed && !uiInputField::typingInteractionOngoing())
					{
						switch (nodeEditorEvent.key.code)
						{
						case sf::Keyboard::Key::Space:
							uiSearchBar::onSpacebarPressed();
							break;
						case sf::Keyboard::Key::H:
							uiFloatingButtonLayer::active = !uiFloatingButtonLayer::active;
							break;
						}
					}

					uiFloatingButtonLayer::ButtonPosition bp = uiFloatingButtonLayer::onPollEvent(nodeEditorEvent);
					if (bp == uiFloatingButtonLayer::ButtonPosition::None)
						uiNodeSystem::onPollEvent(nodeEditorEvent);
					else
					{
						switch (bp)
						{
						case uiFloatingButtonLayer::ButtonPosition::BottomLeft:
							uiHelp::displayHelp();
							break;
						case uiFloatingButtonLayer::ButtonPosition::TopRight:
							viewportWindow->requestFocus();
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
		while (viewportWindow->pollEvent(viewportEvent))
		{
			mousePosViewport = sf::Mouse::getPosition(*viewportWindow);
			switch (viewportEvent.type)
			{
				case sf::Event::Closed:
				{
					nodeEditorWindow->close();
					viewportWindow->close();
					break;
				}
			}
			uiViewport::onPollEvent(viewportEvent);
		}
		// Clear screen
		nodeEditorWindow->clear(BACKGROUND_COLOR);
		viewportWindow->clear(BACKGROUND_COLOR);

		uiNodeSystem::draw();
		uiFloatingButtonLayer::draw();
		uiMenu::draw();
		uiCategoryPusher::draw();
		uiSearchBar::draw();

		uiViewport::draw();

		// Update the window
		nodeEditorWindow->display();
		viewportWindow->display();

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

	if (viewportWindow != nullptr)
		delete viewportWindow;
	if (nodeEditorWindow != nullptr)
		delete nodeEditorWindow;

	return EXIT_SUCCESS;
}