#include <iostream>
#include <SFML/Graphics.hpp>

#include "nodeSystem.h"
#include "searchBar.h"
#include "viewport.h"
#include "dataController.h"

#include "node_system/logicalNode.h"

#include "interface/uiColorPicker.h"
#include "interface/uiData.h"

static const sf::Color BACKGROUND_COLOR(0x181818ff);

void onNodeSelected(logicalNode& theNode)
{
	viewport::outputImage = theNode.getFirstOutputImage();
	std::cout << "viewing image " << viewport::outputImage << std::endl;
}

void onNodeDeleted(logicalNode& theNode)
{
	if (theNode.getFirstOutputImage() == viewport::outputImage)
		viewport::outputImage = nullptr;
}

int main()
{
	// Create the output window
	sf::RenderWindow windowB(sf::VideoMode(500, 500), "viewport");
	// Create the main window
	sf::RenderWindow windowA(sf::VideoMode(1200, 800), "noose");

	// load nodes.dat in memory and create essential shaders
	dataController::initialize();

	// initialize interface components
	uiData::load();
	uiColorPicker::initialize();

	nodeSystem::initialize(windowA);
	searchBar::initialize(windowA);
	viewport::initialize(windowB);

	// node system callbacks
	nodeSystem::setOnNodeSelectedCallback(onNodeSelected);
	nodeSystem::setOnNodeDeletedCallback(onNodeDeleted);
	
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
				case sf::Event::KeyPressed:
				{
					if (eventWindowA.key.code == sf::Keyboard::S && !searchBar::userIsSearching())
					{
						if (viewport::outputImage == nullptr)
							std::cout << "no image to save\n";
						else
						{
							if (viewport::outputImage->getTexture().copyToImage().saveToFile("output.png"))
								std::cout << "image saved as output.png\n";
						}
					}
				}
			}
			nodeSystem::onPollEvent(eventWindowA, mousePos);
			searchBar::onPollEvent(eventWindowA, mousePos);
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
			viewport::onPollEvent(eventWindowB, mousePos);
		}
		// Clear screen
		windowA.clear(BACKGROUND_COLOR);
		windowB.clear(BACKGROUND_COLOR);

		nodeSystem::draw();
		searchBar::draw();

		viewport::draw();

		// Update the window
		windowA.display();
		windowB.display();

		uiColorPicker::tick();
	}

	nodeSystem::terminate();
	uiColorPicker::terminate();

	return EXIT_SUCCESS;
}