#include "uiCategoryPusher.h"
#include "uiNodeSystem.h"
#include "uiSelectionBox.h"

#include <nodeProvider/nodeProvider.h>

#include <iostream>

namespace uiCategoryPusher {

	sf::RenderWindow* renderWindow;
	const sf::Vector2i* mouseScreenPosPointer;

	uiSelectionBox categorySelectionBox;
	uiSelectionBox nodeSelectionBox;
	int selectedCategory;

	sf::Vector2f buttonCenterPos;
}

void uiCategoryPusher::showNodeSelectionBox(int index)
{
	nodeSelectionBox.display(
		buttonCenterPos -
		sf::Vector2f(categorySelectionBox.getBoxWidth(), categorySelectionBox.getBoxHeight() * (nodeProvider::categoryNames.size() - index - 1)),
		nodeProvider::nodeNamesByCategory[index],
		uiSelectionBox::DisplayMode::BottomRightCorner
	);
}

void uiCategoryPusher::updateButtonCenterCoordinates(const sf::Vector2f newPos)
{
	buttonCenterPos = newPos;
}

void uiCategoryPusher::showCategorySelectionBox()
{
	categorySelectionBox.display(
		buttonCenterPos,
		nodeProvider::categoryNames,
		uiSelectionBox::DisplayMode::BottomRightCorner
	);
}

void uiCategoryPusher::initialize(sf::RenderWindow& window, const sf::Vector2i* mouseScreenPosPointer)
{
	renderWindow = &window;
	uiCategoryPusher::mouseScreenPosPointer = mouseScreenPosPointer;
	categorySelectionBox.initialize();
	nodeSelectionBox.initialize();
}

void uiCategoryPusher::terminate()
{
	categorySelectionBox.terminate();
	nodeSelectionBox.terminate();
}

void uiCategoryPusher::onPollEvent(const sf::Event& e)
{
	switch (e.type)
	{
		case sf::Event::Resized:
			if (categorySelectionBox.isVisible())
				showCategorySelectionBox();
			if (nodeSelectionBox.isVisible())
				showNodeSelectionBox(selectedCategory);
			break;
		case sf::Event::MouseButtonPressed:
		{
			if (e.mouseButton.button != sf::Mouse::Left)
				break;

			int mouseOverIndex = categorySelectionBox.mouseOver((sf::Vector2f) * mouseScreenPosPointer);
			if (mouseOverIndex < 0)
			{
				if (nodeSelectionBox.isVisible())
				{
					int mouseOverIndexSub = nodeSelectionBox.mouseOver((sf::Vector2f) * mouseScreenPosPointer);
					if (mouseOverIndexSub > -1)
					{
						uiNodeSystem::pushNewNode(
							&(nodeProvider::nodeDataList[nodeProvider::categoryStartIndex[selectedCategory] + mouseOverIndexSub]),
							uiNodeSystem::PushMode::Centered);
					}
				}
				nodeSelectionBox.hide();
				categorySelectionBox.hide();
			}
			else
			{
				selectedCategory = mouseOverIndex;
				showNodeSelectionBox(mouseOverIndex);
			}
		}
	}
}
void uiCategoryPusher::onClickFloatingButton(const sf::Vector2f& buttonPos)
{
	buttonCenterPos = buttonPos;
	showCategorySelectionBox();
}

void uiCategoryPusher::draw()
{
	sf::FloatRect visibleArea(0, 0, renderWindow->getSize().x, renderWindow->getSize().y);
	renderWindow->setView(sf::View(visibleArea));
	sf::Vector2f mousePos = sf::Vector2f(mouseScreenPosPointer->x, mouseScreenPosPointer->y);
	categorySelectionBox.draw(*renderWindow, mousePos);
	nodeSelectionBox.draw(*renderWindow, mousePos);
}

bool uiCategoryPusher::isActive()
{
	return nodeSelectionBox.isVisible() || categorySelectionBox.isVisible();
}
