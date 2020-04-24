#include "uiCategoryPusher.h"
#include "uiNodeSystem.h"
#include "uiSelectionBox.h"

#include "../math/uiMath.h"
#include "../math/vectorOperators.h"

#include "../nodeProvider/nodeProvider.h"

#include <iostream>

#define BUTTON_RADIUS 0.46f // uv space
#define BUTTON_COLOR 0x5a5a5aff
#define BUTTON_SIZE 50
#define MARGIN 18

sf::RenderWindow* cpRenderWindow;
const sf::Vector2i* cpMouseScreenPosPointer;
uiSelectionBox categorySelectionBox;
uiSelectionBox nodeSelectionBox;
sf::VertexArray addButtonVA;
sf::Shader addButtonShader;
bool cpActive = false;
int selectedCategory;

inline void showNodeSelectionBox(int index)
{
	nodeSelectionBox.display(
		(addButtonVA[2].position + addButtonVA[0].position) / 2.0 -
		sf::Vector2f(categorySelectionBox.getBoxWidth(), categorySelectionBox.getBoxHeight() * (nodeProvider::categoryNames.size() - index - 1)),
		nodeProvider::nodeNamesByCategory[index],
		uiSelectionBox::DisplayMode::BottomRightCorner
	);
}

inline void showCategorySelectionBox()
{
	categorySelectionBox.display(
		(addButtonVA[2].position + addButtonVA[0].position) / 2.0,
		nodeProvider::categoryNames,
		uiSelectionBox::DisplayMode::BottomRightCorner
	);
}

void uiCategoryPusher::initialize(sf::RenderWindow& window, const sf::Vector2i* mouseScreenPosPointer)
{
	if (!addButtonShader.loadFromFile("res/shaders/addFloatingButton.shader", sf::Shader::Fragment))
		std::cout << "[UI] Failed to load add floating button shader\n";
	addButtonShader.setUniform("radius", BUTTON_RADIUS);

	cpRenderWindow = &window;
	cpMouseScreenPosPointer = mouseScreenPosPointer;
	categorySelectionBox.initialize();
	nodeSelectionBox.initialize();

	addButtonVA = sf::VertexArray(sf::Quads, 4);
	addButtonVA[0].color = addButtonVA[1].color = addButtonVA[2].color = addButtonVA[3].color = sf::Color(BUTTON_COLOR);

	addButtonVA[0].texCoords.x = addButtonVA[1].texCoords.x = addButtonVA[0].texCoords.y = addButtonVA[3].texCoords.y = 0.0;
	addButtonVA[2].texCoords.x = addButtonVA[3].texCoords.x = addButtonVA[1].texCoords.y = addButtonVA[2].texCoords.y = 1.0;

	addButtonVA[0].position.x = addButtonVA[1].position.x = cpRenderWindow->getSize().x - BUTTON_SIZE - MARGIN;
	addButtonVA[2].position.x = addButtonVA[3].position.x = cpRenderWindow->getSize().x - MARGIN;
	addButtonVA[0].position.y = addButtonVA[3].position.y = cpRenderWindow->getSize().y - BUTTON_SIZE - MARGIN;
	addButtonVA[1].position.y = addButtonVA[2].position.y = cpRenderWindow->getSize().y - MARGIN;
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
		addButtonVA[0].position.x = addButtonVA[1].position.x = e.size.width - BUTTON_SIZE - MARGIN;
		addButtonVA[2].position.x = addButtonVA[3].position.x = e.size.width - MARGIN;

		addButtonVA[0].position.y = addButtonVA[3].position.y = e.size.height - BUTTON_SIZE - MARGIN;
		addButtonVA[1].position.y = addButtonVA[2].position.y = e.size.height - MARGIN;

		if (categorySelectionBox.isVisible())
			showCategorySelectionBox();

		break;
	case sf::Event::MouseButtonPressed:
	{
		if (e.mouseButton.button != sf::Mouse::Left)
			break;

		sf::Vector2f mousePosInUVSpace =
			(sf::Vector2f(cpMouseScreenPosPointer->x, cpMouseScreenPosPointer->y) - addButtonVA[0].position) /
			BUTTON_SIZE;
		if (cpActive)
		{
			int mouseOverIndex = categorySelectionBox.mouseOver((sf::Vector2f) * cpMouseScreenPosPointer);
			if (mouseOverIndex < 0)
			{
				if (nodeSelectionBox.isVisible())
				{
					int mouseOverIndexSub = nodeSelectionBox.mouseOver((sf::Vector2f) * cpMouseScreenPosPointer);
					if (mouseOverIndexSub > -1)
					{
						uiNodeSystem::pushNewNode(
							&(nodeProvider::nodeDataList[nodeProvider::categoryStartIndex[selectedCategory] + mouseOverIndexSub]),
							uiNodeSystem::PushMode::Centered);
					}
				}
				nodeSelectionBox.hide();
				categorySelectionBox.hide();
				cpActive = false;
			}
			else
			{
				selectedCategory = mouseOverIndex;
				showNodeSelectionBox(mouseOverIndex);
			}
		}
		else
		{
			if (uiMath::distance(sf::Vector2f(0.5, 0.5), mousePosInUVSpace) < BUTTON_RADIUS)
			{
				cpActive = true;
				showCategorySelectionBox();
			}
		}
		break;
	}
	}
}

void uiCategoryPusher::draw()
{
	sf::FloatRect visibleArea(0, 0, cpRenderWindow->getSize().x, cpRenderWindow->getSize().y);
	cpRenderWindow->setView(sf::View(visibleArea));
	sf::Vector2f mousePos = sf::Vector2f(cpMouseScreenPosPointer->x, cpMouseScreenPosPointer->y);
	cpRenderWindow->draw(addButtonVA, &addButtonShader);
	categorySelectionBox.draw(*cpRenderWindow, mousePos);
	nodeSelectionBox.draw(*cpRenderWindow, mousePos);
}

bool uiCategoryPusher::isActive()
{
	return cpActive;
}
