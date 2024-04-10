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
		sf::Vector2f(categorySelectionBox.getBoxWidth(), categorySelectionBox.getBoxHeight() * (nodeProvider::getCategories().size() - index - 1)),
		*nodeProvider::getNodesForCategory(nodeProvider::getCategories()[index]),
		uiSelectionBox::DisplayMode::BottomRightCorner
	);
}

void uiCategoryPusher::updateButtonCenterCoordinates(const sf::Vector2f& newPos)
{
	buttonCenterPos = newPos;
}

void uiCategoryPusher::showCategorySelectionBox()
{
	categorySelectionBox.display(
		buttonCenterPos,
		nodeProvider::getCategories(),
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
			if (e.mouseButton.button != sf::Mouse::Button::Left)
				break;

			int mouseOverIndex = categorySelectionBox.mouseOver((sf::Vector2f) * mouseScreenPosPointer);
			if (mouseOverIndex < 0)
			{
				if (nodeSelectionBox.isVisible())
				{
					int mouseOverIndexSub = nodeSelectionBox.mouseOver((sf::Vector2f) * mouseScreenPosPointer);
					if (mouseOverIndexSub > -1)
					{
						const nodeData* nodeToAdd = nodeProvider::getNodeDataByName((*nodeProvider::getNodesForCategory(nodeProvider::getCategories()[selectedCategory]))[mouseOverIndexSub]);
						uiNodeSystem::pushNewNode(
							nodeToAdd,
							uiNodeSystem::PushMode::Centered);
					}
				}
				nodeSelectionBox.hide();
				categorySelectionBox.hide();
			}
			else
			{
				selectedCategory = mouseOverIndex;
				showNodeSelectionBox(selectedCategory);
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
	sf::FloatRect visibleArea({ 0.0f, 0.0f }, { (float)renderWindow->getSize().x, (float)renderWindow->getSize().y });
	renderWindow->setView(sf::View(visibleArea));
	sf::Vector2f mousePos = sf::Vector2f(mouseScreenPosPointer->x, mouseScreenPosPointer->y);
	categorySelectionBox.draw(*renderWindow, mousePos);
	nodeSelectionBox.draw(*renderWindow, mousePos);
}

bool uiCategoryPusher::isActive()
{
	return nodeSelectionBox.isVisible() || categorySelectionBox.isVisible();
}
