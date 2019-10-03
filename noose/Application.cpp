#include <iostream>
#include <SFML/Graphics.hpp>
#include "uiNodeSystem.h"
#include "uiPushNodes.h"
#include "uiNode.h"
#include "uiMath.h"
#include "uiColorPicker.h"

#include "dataController.h"

#include "uiViewport.h"

static const sf::Color BACKGROUND_COLOR(0x595959ff);

//sf::RenderTexture* outputImage = nullptr;

void onNodeSelected(uiNode& theNode)
{
	uiViewport::outputImage = theNode.getFirstOutputImage();
	std::cout << "viewing image " << uiViewport::outputImage << std::endl;
}

void onNodeDeleted(uiNode& theNode)
{
	if (theNode.getFirstOutputImage() == uiViewport::outputImage)
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
	uiNodeSystem::initialize(windowA);
	uiPushNodes::initialize(windowA);
	uiColorPicker::initialize();

	// node system callbacks
	uiNodeSystem::setOnNodeSelectedCallback(onNodeSelected);
	uiNodeSystem::setOnNodeDeletedCallback(onNodeDeleted);

	uiViewport::initialize(windowB);
	
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
					if (eventWindowA.key.code == sf::Keyboard::S && !uiPushNodes::userIsSearching())
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
			uiViewport::onPollEvent(eventWindowB, mousePos);
		}
		// Clear screen
		windowA.clear(BACKGROUND_COLOR);
		windowB.clear(BACKGROUND_COLOR);

		uiNodeSystem::draw();
		uiPushNodes::draw();

		uiViewport::draw();
		uiColorPicker::tick();

		// Update the window
		windowA.display();
		windowB.display();
	}

	uiNodeSystem::terminate();
	uiColorPicker::terminate();

	dataController::terminate();
	return EXIT_SUCCESS;
}