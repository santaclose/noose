#include <iostream>
#include <SFML/Graphics.hpp>

#include "interface/uiSelectionBox.h"
#include "interface/uiData.h"

#include "interface/uiNodeSystem.h"
#include "interface/uiColorPicker.h"

#include "interface/uiCategoryPusher.h"
#include "interface/uiHelp.h"
#include "interface/uiFloatingButtonLayer.h"

#include "interface/uiSearchBar.h"
#include "interface/uiViewport.h"

#include "nodeProvider/nodeProvider.h"
#include "logic/nodeSystem.h"
#include "utils.h"

static const sf::Color BACKGROUND_COLOR(0x222222ff);

void onNodeSelected(int theNode)
{
	std::cout << "[Main] Node " << theNode << " selected\n";
	
	uiViewport::hideSelectionBox();
	uiViewport::setNodeData(
		theNode,
		&nodeSystem::getDataPointersForNode(theNode),
		nodeSystem::getPinTypesForNode(theNode),
		nodeSystem::getOutputPinCountForNode(theNode)
		);
}

// just before deleting the node
void onNodeDeleted(int theNode)
{
	std::cout << "[Main] Node " << theNode << " deleted\n";

	uiViewport::hideSelectionBox();
	uiViewport::onNodeDeleted(theNode);
}

void onNodeChanged(int theNode)
{
	uiViewport::onNodeChanged(theNode);
}

int main(int argc, char** argv)
{
	utils::setProgramDirectory(argv[0]);

	// Create the main window
	sf::RenderWindow windowA(sf::VideoMode(1200, 800), "node editor");
	// Create the output window
	sf::RenderWindow windowB(sf::VideoMode(500, 500), "viewport", sf::Style::Resize);
	sf::Image iconImage;
	iconImage.loadFromFile(utils::getProgramDirectory() + "assets/icon.png");
	windowA.setIcon(iconImage.getSize().x, iconImage.getSize().y, iconImage.getPixelsPtr());
	windowB.setIcon(iconImage.getSize().x, iconImage.getSize().y, iconImage.getPixelsPtr());

	// load nodes in memory
	nodeProvider::initialize();
	//nodeProvider::print();

	// initialize interface components
	uiData::load();
	uiColorPicker::initialize();
	
	sf::Vector2i mousePosWindowA;
	sf::Vector2i mousePosWindowB;

	uiNodeSystem::initialize(windowA, &mousePosWindowA);

	uiSearchBar::initialize(windowA, &mousePosWindowA);
	uiCategoryPusher::initialize(windowA, &mousePosWindowA);

	uiFloatingButtonLayer::initialize(windowA, &mousePosWindowA);
	uiFloatingButtonLayer::addButton(uiFloatingButtonLayer::ButtonPosition::BottomRight, "assets/shaders/addFloatingButton.shader");
	uiFloatingButtonLayer::addButton(uiFloatingButtonLayer::ButtonPosition::BottomLeft, '?');
	uiFloatingButtonLayer::addButton(uiFloatingButtonLayer::ButtonPosition::TopRight, "assets/shaders/showViewportButton.shader");

	uiViewport::initialize(windowB, &mousePosWindowB);

	// node system callbacks
	uiNodeSystem::setOnNodeSelectedCallback(onNodeSelected);
	uiNodeSystem::setOnNodeDeletedCallback(onNodeDeleted);
	uiNodeSystem::setOnNodeChangedCallback(onNodeChanged);

	// load file if opening file
	if (argc > 1)
	{
		std::string fileToOpenPath(argv[1]);
		if (fileToOpenPath.compare(fileToOpenPath.length() - 3, 3, ".ns") == 0)
			serializer::LoadFromFile(argv[1]);
		else
			serializer::LoadImageFile(argv[1]);
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
			}
			else
			{
				if (uiSearchBar::isActive())
					uiSearchBar::onPollEvent(eventWindowA);
				else if (uiCategoryPusher::isActive())
					uiCategoryPusher::onPollEvent(eventWindowA);
				else
				{
					if (eventWindowA.type == sf::Event::KeyPressed && eventWindowA.key.code == sf::Keyboard::Space)
						uiSearchBar::onSpacebarPressed();

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
		uiCategoryPusher::draw();
		uiSearchBar::draw();

		uiViewport::draw();

		// Update the window
		windowA.display();
		windowB.display();

		uiColorPicker::tick();
	}

	uiCategoryPusher::terminate();
	uiFloatingButtonLayer::terminate();
	uiViewport::terminate();
	uiNodeSystem::terminate();
	uiColorPicker::terminate();
	nodeProvider::terminate();

	return EXIT_SUCCESS;
}