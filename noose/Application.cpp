#include <SFML/Graphics.hpp>
#include "uiNodeSystem.h"
#include "uiPushNodes.h"

#include "dataController.h"

#include <iostream>


static const sf::Color BACKGROUND_COLOR(0x595959ff);

int main()
{
	// Create the main window
	sf::RenderWindow windowA(sf::VideoMode(1200, 800), "noose");

	// Create the output window
	sf::RenderWindow windowB(sf::VideoMode(500, 500), "viewport");

	uiNodeSystem::initialize(windowA);
	uiPushNodes::initialize(windowA);

	// load nodes.dat in memory
	dataController::prepare();

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
			uiPushNodes::onPollEvent(eventWindowA, mousePos);
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
		}
		// Clear screen
		windowA.clear(BACKGROUND_COLOR);
		windowB.clear(BACKGROUND_COLOR);

		uiNodeSystem::draw(windowA);
		uiPushNodes::draw(windowA);

		// Update the window
		windowA.display();
		windowB.display();
	}

	uiNodeSystem::terminate(); 
	return EXIT_SUCCESS;
}