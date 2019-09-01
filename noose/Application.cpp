#include <iostream>
#include <SFML/Graphics.hpp>
#include "uiNodeSystem.h"
#include "uiPushNodes.h"
#include "uiNode.h"

#include "dataController.h"

#include "uiViewport.h"

static const sf::Color BACKGROUND_COLOR(0x595959ff);

//sf::RenderTexture* outputImage = nullptr;

void onNodeSelected(uiNode& theNode)
{
	uiViewport::outputImage = theNode.getFirstOutputImage();
	std::cout << "viewing image " << uiViewport::outputImage << std::endl;
}

int main()
{
	// Create the main window
	sf::RenderWindow windowA(sf::VideoMode(1200, 800), "noose");
	// Create the output window
	sf::RenderWindow windowB(sf::VideoMode(500, 500), "viewport");

	// load nodes.dat in memory and create essential shaders
	dataController::prepare();

	// initialize interface components
	uiNodeSystem::initialize(windowA);
	uiPushNodes::initialize(windowA);

	// on node selected callback
	uiNodeSystem::setOnNodeSelectedCallback(onNodeSelected);

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
					if (eventWindowA.key.code == sf::Keyboard::S)
					{
						if (uiViewport::outputImage == nullptr)
							std::cout << "no image to save\n";
						else
						{
							uiViewport::outputImage->getTexture().copyToImage().saveToFile("output.png");
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
				/*case sf::Event::Resized:
				{
					// update the view to the new size of the window
					sf::FloatRect visibleArea(0, 0, eventWindowB.size.width, eventWindowB.size.height);
					windowB.setView(sf::View(visibleArea));
					break;
				}
				*/
			}
			uiViewport::onPollEvent(eventWindowB, mousePos);
		}
		// Clear screen
		windowA.clear(BACKGROUND_COLOR);
		windowB.clear(BACKGROUND_COLOR);

		uiNodeSystem::draw(windowA);
		uiPushNodes::draw(windowA);

		uiViewport::draw();
		/*if (outputImage != nullptr)
		{
			sf::Sprite spr(outputImage->getTexture());
			windowB.draw(spr);
		}*/

		// Update the window
		windowA.display();
		windowB.display();
	}

	uiNodeSystem::terminate();
	return EXIT_SUCCESS;
}