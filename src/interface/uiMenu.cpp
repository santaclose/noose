#include "uiMenu.h"

#include "uiSelectionBox.h"
#include "uiNodeSystem.h"

#include "../serializer.h"
#include "../utils.h"

#include <iostream>
#include <portable-file-dialogs.h>

namespace uiMenu {

	sf::RenderWindow* renderWindow;
	const sf::Vector2i* mouseScreenPosPointer;

	uiSelectionBox selectionBox;
	std::vector<std::string> selectionBoxOptions = { "Open project", "Save project", "Clear project" };

	sf::Vector2f buttonCenterPos;

	std::string continueMessageBoxTitle = "Alert";
	std::string continueMessageBoxMessage = "The current state of the node editor will be lost, do you want to continue?";
}

void uiMenu::initialize(sf::RenderWindow& window, const sf::Vector2i* mouseScreenPosPointer)
{
	renderWindow = &window;
	uiMenu::mouseScreenPosPointer = mouseScreenPosPointer;
	selectionBox.initialize();
}

void uiMenu::terminate()
{
	selectionBox.terminate();
}

void uiMenu::onPollEvent(const sf::Event& e)
{
	switch (e.type)
	{
	case sf::Event::MouseButtonPressed:
		if (e.mouseButton.button != sf::Mouse::Left)
			break;
		int mouseOverIndex = selectionBox.mouseOver((sf::Vector2f)*mouseScreenPosPointer);
		if (mouseOverIndex > -1)
		{
			switch (mouseOverIndex)
			{
			case 0: // load
			{
				if (!uiNodeSystem::isEmpty())
				{
					pfd::button choice = pfd::message(continueMessageBoxTitle, continueMessageBoxMessage, pfd::choice::yes_no, pfd::icon::warning).result();
					if (choice == pfd::button::no)
						break;
				}
				std::vector<std::string> selection = pfd::open_file("Open file", "", { "Noose file (.ns)", "*.ns" }).result();
				if (selection.size() == 0)
				{
					std::cout << "[UI] File not loaded\n";
					break;
				}

				uiNodeSystem::clearNodeSelection(); // unselect if there is a node selected
				serializer::LoadFromFile(selection[0]);
				break;
			}
			case 1: // save
			{
				std::string destination = pfd::save_file("Save file", "", { "Noose file (.ns)", "*.ns" }).result();
				if (destination.length() == 0)
				{
					std::cout << "[UI] File not saved\n";
					break;
				}
				destination = destination + (utils::fileHasExtension(destination.c_str(), "ns") ? "" : ".ns");
				serializer::SaveIntoFile(destination);
				break;
			}
			case 2: // clear
			{
				if (!uiNodeSystem::isEmpty())
				{
					pfd::button choice = pfd::message(continueMessageBoxTitle, continueMessageBoxMessage, pfd::choice::yes_no, pfd::icon::warning).result();
					if (choice == pfd::button::no)
						break;
				}

				uiNodeSystem::clearNodeSelection();
				uiNodeSystem::clearEverything();
				break;
			}
			}
		}
		selectionBox.hide();
	}
}
void uiMenu::onClickFloatingButton(const sf::Vector2f& buttonPos)
{
	buttonCenterPos = buttonPos;

	selectionBox.display(
		buttonCenterPos,
		selectionBoxOptions,
		uiSelectionBox::DisplayMode::TopLeftCorner
	);
}

void uiMenu::draw()
{
	sf::FloatRect visibleArea(0, 0, renderWindow->getSize().x, renderWindow->getSize().y);
	renderWindow->setView(sf::View(visibleArea));
	sf::Vector2f mousePos = (sf::Vector2f)*mouseScreenPosPointer;
	selectionBox.draw(*renderWindow, mousePos);
}

bool uiMenu::isActive()
{
	return selectionBox.isVisible();
}
