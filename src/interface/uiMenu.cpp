#include "uiMenu.h"

#include "uiSelectionBox.h"
#include "uiNodeSystem.h"
#include "uiViewport.h"

#include "../serializer.h"
#include "../pathUtils.h"
#include "../utils.h"

#include <iostream>

namespace uiMenu {

	sf::RenderWindow* renderWindow;
	const sf::Vector2i* mouseScreenPosPointer;

	uiSelectionBox selectionBox;
	std::vector<std::string> selectionBoxOptions = { "Open project", "Save project", "Save project as json", "Clear project" };

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
		if (e.mouseButton.button != sf::Mouse::Button::Left)
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
					utils::osChoice choice = utils::osYesNoMessageBox(continueMessageBoxTitle, continueMessageBoxMessage);
					if (choice == utils::osChoice::No)
						break;
				}
				std::vector<std::string> selection = utils::osOpenFileDialog("Open file", "Noose file (.ns .nsj)", "*.ns *.nsj");
				if (selection.size() == 0)
				{
					std::cout << "[UI] File not loaded\n";
					break;
				}

				serializer::ParsingCallbacks parsingCallbacks;
				parsingCallbacks.OnStartParsing = uiNodeSystem::onProjectFileStartParsing;
				parsingCallbacks.OnParseNode = uiNodeSystem::onProjectFileParseNode;
				parsingCallbacks.OnParseNodeInput = uiNodeSystem::onProjectFileParseNodeInput;
				parsingCallbacks.OnParseConnection = uiNodeSystem::onProjectFileParseConnection;
				parsingCallbacks.OnParseNodeEditorState = uiNodeSystem::onProjectFileParseNodeEditorState;
				parsingCallbacks.OnParseViewportState = uiViewport::onProjectFileParseViewportState;
				if (utils::endsWith(selection[0], ".nsj"))
					serializer::LoadFromFileJson(selection[0], parsingCallbacks);
				else
					serializer::LoadFromFile(selection[0], parsingCallbacks);
				break;
			}
			case 1: // save
			{
				std::string destination = utils::osSaveFileDialog("Save file", "Noose file (.ns)", "*.ns");
				if (destination.length() == 0)
				{
					std::cout << "[UI] File not saved\n";
					break;
				}
				destination = destination + (pathUtils::fileHasExtension(destination.c_str(), "ns") ? "" : ".ns");
				serializer::SaveIntoFile(destination);
				break;
			}
			case 2: // save json
			{
				std::string destination = utils::osSaveFileDialog("Save file", "Noose json file (.nsj)", "*.nsj");
				if (destination.length() == 0)
				{
					std::cout << "[UI] File not saved\n";
					break;
				}
				destination = destination + (pathUtils::fileHasExtension(destination.c_str(), "nsj") ? "" : ".nsj");
				serializer::SaveIntoFileJson(destination);
				break;
			}
			case 3: // clear
			{
				if (!uiNodeSystem::isEmpty())
				{
					utils::osChoice choice = utils::osYesNoMessageBox(continueMessageBoxTitle, continueMessageBoxMessage);
					if (choice == utils::osChoice::No)
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
	sf::FloatRect visibleArea({ 0.0f, 0.0f }, { (float)renderWindow->getSize().x, (float)renderWindow->getSize().y });
	renderWindow->setView(sf::View(visibleArea));
	sf::Vector2f mousePos = (sf::Vector2f)*mouseScreenPosPointer;
	selectionBox.draw(*renderWindow, mousePos);
}

bool uiMenu::isActive()
{
	return selectionBox.isVisible();
}
