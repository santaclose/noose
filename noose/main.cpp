#include <iostream>
#include <SFML/Graphics.hpp>

#include "interface/uiNodeSystem.h"
#include "interface/uiSearchBar.h"
#include "interface/uiViewport.h"
#include "interface/uiColorPicker.h"
#include "interface/uiData.h"
#include "logic/nodeSystem.h"
#include "dataController.h"

static const sf::Color BACKGROUND_COLOR(0x181818ff);

void onNodeSelected(int theNode)
{
	std::cout << "node " << theNode << " selected\n";
	uiViewport::outputImage = nodeSystem::getFirstOutputImageForNode(theNode); //theNode.getFirstOutputImage();
	std::cout << "viewing image " << uiViewport::outputImage << std::endl;
}

void onNodeDeleted(int theNode)
{
	std::cout << "node " << theNode << " deleted\n";
	if (nodeSystem::getFirstOutputImageForNode(theNode) == uiViewport::outputImage)
		uiViewport::outputImage = nullptr;
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
				case sf::Event::KeyPressed:
				{
					if (eventWindowA.key.code == sf::Keyboard::S && !uiSearchBar::userIsSearching())
					{
						if (uiViewport::outputImage == nullptr)
							std::cout << "no image to save\n";
						else
						{
							if (uiViewport::outputImage->getTexture().copyToImage().saveToFile("output.png"))
								std::cout << "image saved as output.png\n";
						}
					}
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

	uiNodeSystem::terminate();
	uiColorPicker::terminate();

	return EXIT_SUCCESS;
}