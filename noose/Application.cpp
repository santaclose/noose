#include <SFML/Graphics.hpp>
#include "uiNodeSystem.h"
#include "uiPushNodes.h"

#include "dataController.h"

#include <iostream>

static const sf::Color BACKGROUND_COLOR(0x595959ff);

int main()
{
	// Create the main window
	sf::RenderWindow window(sf::VideoMode(1200, 800), "noose");
	sf::View windowView = window.getView();

	uiNodeSystem::initialize(window);
	uiPushNodes::initialize(window);

	// load nodes.dat in memory
	dataController::prepare();

	// Start the game loop
	while (window.isOpen())
	{
		// Process events
		sf::Event event;
		while (window.pollEvent(event))
		{
			sf::Vector2i mousePos = sf::Mouse::getPosition(window);
			switch (event.type)
			{
				case sf::Event::Closed:
				{
					window.close();
					break;
				}
			}
			uiNodeSystem::onPollEvent(event, mousePos);
			uiPushNodes::onPollEvent(event, mousePos);
		}
		// Clear screen
		window.clear(BACKGROUND_COLOR);

		uiNodeSystem::draw(window);
		uiPushNodes::draw(window);

		// Update the window
		window.display();
	}

	uiNodeSystem::terminate(); 
	return EXIT_SUCCESS;
}