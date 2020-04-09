#include <iostream>
#include <SFML/Graphics.hpp>

#include "interface/uiNodeSystem.h"
#include "interface/uiSearchBar.h"
#include "interface/uiViewport.h"
#include "interface/uiColorPicker.h"
#include "interface/uiData.h"
#include "interface/uiSelectionBox.h"
#include "logic/nodeSystem.h"
#include "nodeProvider/nodeProvider.h"

static const sf::Color BACKGROUND_COLOR(0x222222ff);

void onNodeSelected(int theNode)
{
	std::cout << "[Main] Node " << theNode << " selected\n";
	
	uiViewport::hideSelectionBox();
	uiViewport::setNodeData(
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

	if (&nodeSystem::getDataPointersForNode(theNode) == uiViewport::getNodeDataPointers())
		uiViewport::hideNodeData();
}

int main()
{
	// Create the output window
	sf::RenderWindow windowB(sf::VideoMode(500, 500), "viewport");
	// Create the main window
	sf::RenderWindow windowA(sf::VideoMode(1200, 800), "noose");

	// load nodes in memory
	nodeProvider::initialize();
	//nodeProvider::print();

	// initialize interface components
	uiData::load();
	uiColorPicker::initialize();

	uiNodeSystem::initialize(windowA);
	uiSearchBar::initialize(windowA);
	uiViewport::initialize(windowB);

	// node system callbacks
	uiNodeSystem::setOnNodeSelectedCallback(onNodeSelected);
	uiNodeSystem::setOnNodeDeletedCallback(onNodeDeleted);
	
	// Start the game loop
	while (windowA.isOpen() || windowB.isOpen())
	{
		// Process events
		sf::Event eventWindowA, eventWindowB;
		while (windowA.pollEvent(eventWindowA))
		{
			sf::Vector2i mousePos = sf::Mouse::getPosition(windowA);
			switch (eventWindowA.type)
			{
				case sf::Event::Closed:
				{
					windowA.close();
					windowB.close();
					break;
				}
			}
			uiNodeSystem::onPollEvent(eventWindowA, mousePos);
			uiSearchBar::onPollEvent(eventWindowA, mousePos);
		}
		while (windowB.pollEvent(eventWindowB))
		{
			sf::Vector2i mousePos = sf::Mouse::getPosition(windowB);
			switch (eventWindowB.type)
			{
				case sf::Event::Closed:
				{
					windowA.close();
					windowB.close();
					break;
				}
			}
			uiViewport::onPollEvent(eventWindowB, mousePos);
		}
		// Clear screen
		windowA.clear(BACKGROUND_COLOR);
		windowB.clear(BACKGROUND_COLOR);

		uiNodeSystem::draw();
		uiSearchBar::draw();

		uiViewport::draw();

		// Update the window
		windowA.display();
		windowB.display();

		uiColorPicker::tick();
	}

	uiViewport::terminate();
	uiNodeSystem::terminate();
	uiColorPicker::terminate();
	nodeProvider::terminate();

	return EXIT_SUCCESS;
}