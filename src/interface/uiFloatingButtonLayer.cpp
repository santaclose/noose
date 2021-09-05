#include "uiFloatingButtonLayer.h"
#include <pathUtils.h>
#include <vector>
#include <iostream>
#include "uiData.h"
#include <math/nooseMath.h>
#include <math/vectorOperators.h>

#define BUTTON_RADIUS 0.46f // uv space
#define BUTTON_COLOR 0x5a5a5aff
#define BUTTON_SIZE 50
#define MARGIN 18

namespace uiFloatingButtonLayer {

	bool active = true;

	sf::RenderWindow* renderWindow;
	const sf::Vector2i* mouseScreenPosPointer;
	sf::Shader genericShader;

	std::vector<FloatingButton> buttons;
}

void uiFloatingButtonLayer::updatButtonElementPositions(FloatingButton& fb)
{
	if (fb.pos == ButtonPosition::BottomLeft || fb.pos == ButtonPosition::TopLeft)
	{
		fb.va[0].position.x = fb.va[1].position.x = MARGIN;
		fb.va[2].position.x = fb.va[3].position.x = MARGIN + BUTTON_SIZE;
	}
	else
	{
		fb.va[0].position.x = fb.va[1].position.x = renderWindow->getSize().x - BUTTON_SIZE - MARGIN;
		fb.va[2].position.x = fb.va[3].position.x = renderWindow->getSize().x - MARGIN;
	}

	if (fb.pos == ButtonPosition::TopLeft || fb.pos == ButtonPosition::TopRight)
	{
		fb.va[0].position.y = fb.va[3].position.y = MARGIN;
		fb.va[1].position.y = fb.va[2].position.y = MARGIN + BUTTON_SIZE;
	}
	else
	{
		fb.va[0].position.y = fb.va[3].position.y = renderWindow->getSize().y - BUTTON_SIZE - MARGIN;
		fb.va[1].position.y = fb.va[2].position.y = renderWindow->getSize().y - MARGIN;
	}

	if (fb.text != nullptr)
		fb.text->setPosition(
			MARGIN + BUTTON_SIZE / 2.0 - fb.text->getLocalBounds().width / 2.0,
			renderWindow->getSize().y - MARGIN - BUTTON_SIZE / 2.0 - fb.text->getLocalBounds().height / 1.2);
}

void uiFloatingButtonLayer::initialize(sf::RenderWindow& window, const sf::Vector2i* mouseScreenPosPointer)
{
	renderWindow = &window;
	uiFloatingButtonLayer::mouseScreenPosPointer = mouseScreenPosPointer;
	if (!genericShader.loadFromFile(pathUtils::getAssetsDirectory() + "shaders/floatingButton.shader", sf::Shader::Fragment))
		std::cout << "[UI] Failed to load floating button shader\n";
	genericShader.setUniform("radius", BUTTON_RADIUS);
}

void uiFloatingButtonLayer::addButton(ButtonPosition position, const std::string& customShaderPath)
{
	addButton(position, '\0', &customShaderPath);
}

void uiFloatingButtonLayer::addButton(ButtonPosition position, char symbol, const std::string* customShaderPath)
{
	buttons.emplace_back();
	buttons.back().pos = position;
	if (customShaderPath == nullptr)
	{
		buttons.back().customShader = nullptr;
		buttons.back().text = new sf::Text(sf::String(symbol), uiData::font, 30);
	}
	else
	{
		buttons.back().text = nullptr;
		buttons.back().customShader = new sf::Shader();
		if (!buttons.back().customShader->loadFromFile(pathUtils::getAssetsDirectory() + *customShaderPath, sf::Shader::Fragment))
			std::cout << "[UI] Failed to load floating button shader\n";
		buttons.back().customShader->setUniform("radius", BUTTON_RADIUS);
	}

	buttons.back().va = sf::VertexArray(sf::Quads, 4);
	// set vertex colors
	buttons.back().va[0].color = buttons.back().va[1].color = buttons.back().va[2].color = buttons.back().va[3].color = sf::Color(BUTTON_COLOR);

	// set texture coordinates
	buttons.back().va[0].texCoords.x = buttons.back().va[1].texCoords.x = buttons.back().va[0].texCoords.y = buttons.back().va[3].texCoords.y = 0.0;
	buttons.back().va[2].texCoords.x = buttons.back().va[3].texCoords.x = buttons.back().va[1].texCoords.y = buttons.back().va[2].texCoords.y = 1.0;

	updatButtonElementPositions(buttons.back());
}

void uiFloatingButtonLayer::draw()
{
	if (!active)
		return;
	sf::FloatRect visibleArea(0, 0, renderWindow->getSize().x, renderWindow->getSize().y);
	renderWindow->setView(sf::View(visibleArea));

	for (const FloatingButton& fb : buttons)
	{
		if (fb.customShader != nullptr)
		{
			renderWindow->draw(fb.va, fb.customShader);
		}
		else
		{
			renderWindow->draw(fb.va, &genericShader);
			renderWindow->draw(*fb.text);
		}
	}
}

sf::Vector2f uiFloatingButtonLayer::getButtonCenterCoords(ButtonPosition pos)
{
	sf::Vector2f res = { 0.0f, 0.0f };
	if (pos == ButtonPosition::BottomLeft || pos == ButtonPosition::TopLeft)
		res.x = MARGIN + BUTTON_SIZE * 0.5f;
	else
		res.x = renderWindow->getSize().x - MARGIN - BUTTON_SIZE * 0.5f;
	if (pos == ButtonPosition::TopLeft || pos == ButtonPosition::TopRight)
		res.y = MARGIN + BUTTON_SIZE * 0.5f;
	else
		res.y = renderWindow->getSize().y - MARGIN - BUTTON_SIZE * 0.5f;
	return res;
}

void uiFloatingButtonLayer::terminate()
{
	for (FloatingButton& button : buttons)
	{
		if (button.customShader != nullptr)
			delete button.customShader;
		else
			delete button.text;
	}
}

uiFloatingButtonLayer::ButtonPosition uiFloatingButtonLayer::onPollEvent(const sf::Event& e)
{
	switch (e.type)
	{
		case sf::Event::Resized:
			for (FloatingButton& fb : buttons)
				updatButtonElementPositions(fb);
			break;
		case sf::Event::MouseButtonPressed:
		{
			if (e.mouseButton.button != sf::Mouse::Left || !active)
				break;

			for (const FloatingButton& fb : buttons)
			{
				sf::Vector2f mousePosInUVSpace =
					(sf::Vector2f(mouseScreenPosPointer->x, mouseScreenPosPointer->y) - fb.va[0].position) /
					BUTTON_SIZE;
				if (nooseMath::distance(sf::Vector2f(0.5, 0.5), mousePosInUVSpace) < BUTTON_RADIUS)
					return fb.pos;
			}
			break;
		}
	}
	return ButtonPosition::None;
}
