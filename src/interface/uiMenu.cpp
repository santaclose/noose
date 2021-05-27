#include "uiMenu.h"

#include "uiSelectionBox.h"
#include "uiFileSelector.h"
#include "uiNodeSystem.h"
#include "uiMessageBox.h"

#include "../serializer.h"

#include <iostream>

namespace uiMenu {

	sf::RenderWindow* renderWindow;
	const sf::Vector2i* mouseScreenPosPointer;

	uiSelectionBox selectionBox;
	std::vector<std::string> selectionBoxOptions = { "Open project", "Save project", "Clear project" };

	uiMessageBox confirmMessageBox;

	sf::Vector2f buttonCenterPos;

	void displayClearConfirmMessageBox()
	{
		confirmMessageBox.display("Do you really want to delete all the nodes?", { "yes", "no" }, ((sf::Vector2f)(renderWindow->getSize()) / 2.0f));
	}
}


//void uiMenu::updateButtonCenterCoordinates(const sf::Vector2f newPos)
//{
//	buttonCenterPos = newPos;
//}

void uiMenu::initialize(sf::RenderWindow& window, const sf::Vector2i* mouseScreenPosPointer)
{
	renderWindow = &window;
	uiMenu::mouseScreenPosPointer = mouseScreenPosPointer;
	selectionBox.initialize();
	confirmMessageBox.initialize();
}

void uiMenu::terminate()
{
	selectionBox.terminate();
	confirmMessageBox.terminate();
}

void uiMenu::onPollEvent(const sf::Event& e)
{
	switch (e.type)
	{
	case sf::Event::Resized:
		if (confirmMessageBox.isVisible())
			displayClearConfirmMessageBox();
		break;
	case sf::Event::MouseButtonPressed:
		if (e.mouseButton.button != sf::Mouse::Left)
			break;

		if (confirmMessageBox.isVisible())
		{
			int mouseOverIndex = confirmMessageBox.mouseOver((sf::Vector2f)*mouseScreenPosPointer);
			if (mouseOverIndex == 0) // yes, clear
			{
				uiNodeSystem::clearNodeSelection();
				uiNodeSystem::clearEverything();
				confirmMessageBox.hide();
			}
			else if (mouseOverIndex == 1)
			{
				confirmMessageBox.hide();
			}
		}
		else
		{
			int mouseOverIndex = selectionBox.mouseOver((sf::Vector2f)*mouseScreenPosPointer);
			if (mouseOverIndex > -1)
			{
				switch (mouseOverIndex)
				{
				case 0: // load
				{
					char* filePath = uiFileSelector::openFileDialog("ns");
					if (filePath == nullptr)
						break;
					uiNodeSystem::clearNodeSelection(); // unselect if there is a node selected
					serializer::LoadFromFile(filePath);
					free(filePath);
					break;
				}
				case 1: // save
				{
					char* filePath = uiFileSelector::saveFileDialog("ns");
					if (filePath == nullptr)
						break;
					serializer::SaveIntoFile(filePath);
					free(filePath);
					break;
				}
				case 2: // clear
				{
					displayClearConfirmMessageBox();
					break;
				}
				}
			}
			selectionBox.hide();
		}
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
	confirmMessageBox.draw(*renderWindow, mousePos);
}

bool uiMenu::isActive()
{
	return selectionBox.isVisible() || confirmMessageBox.isVisible();
}
