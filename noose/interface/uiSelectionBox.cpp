#include "uiSelectionBox.h"
#include "uiData.h"
#include "../math/vectorOperators.h"
#include "../math/uiMath.h"
#include <vector>
#include <iostream>

#define OPTION_FONT_SIZE 14
#define BOX_WIDTH 160
#define BOX_HEIGHT 30
#define COLOR 0x222222cc
#define TEXT_MARGIN_X 7
#define TEXT_MARGIN_Y 7


//sf::Shader m_mouseOverShader;

bool m_onScreen = false;
std::vector<sf::Vertex> m_boxes;
std::vector<sf::Text*> m_optionTexts;
int m_currentOptionCount = 0;

void uiSelectionBox::initialize()
{
	//m_mouseOverShader.loadFromFile()
}

void uiSelectionBox::display(const sf::Vector2f& position, const std::vector<std::string>& options)
{
	m_onScreen = true;
	int prevSize = m_currentOptionCount;
	m_currentOptionCount = options.size();

	if (m_currentOptionCount > prevSize)
	{
		m_optionTexts.resize(m_currentOptionCount, nullptr);
		m_boxes.resize(m_currentOptionCount * 4);
		sf::Color theColor(COLOR);
		for (int i = prevSize * 4; i < m_currentOptionCount * 4; i++)
			m_boxes[i].color = theColor;
	}
	else if (m_currentOptionCount < prevSize)
	{
		for (int i = m_currentOptionCount * 4; i < m_boxes.size(); i++)
			m_boxes[i].color.a = 0;
	}

	sf::Vector2f cursor = position;
	for (int i = 0; i < m_currentOptionCount; i++)
	{
		if (m_optionTexts[i] == nullptr)
			m_optionTexts[i] = new sf::Text(options[i], uiData::font, OPTION_FONT_SIZE);
		else
			m_optionTexts[i]->setString(options[i]);

		m_boxes[  i * 4  ].position.x = m_boxes[i * 4 + 1].position.x = cursor.x;
		m_boxes[i * 4 + 2].position.x = m_boxes[i * 4 + 3].position.x = cursor.x + BOX_WIDTH;
		m_boxes[  i * 4  ].position.y = m_boxes[i * 4 + 3].position.y = cursor.y;
		m_boxes[i * 4 + 1].position.y = m_boxes[i * 4 + 2].position.y = cursor.y + BOX_HEIGHT;

		m_optionTexts[i]->setPosition(m_boxes[i * 4].position + sf::Vector2f(TEXT_MARGIN_X, TEXT_MARGIN_Y));

		cursor.y += BOX_HEIGHT;
	}
}

int uiSelectionBox::mouseOver(const sf::Vector2f& position)
{
	for (int i = 0; i < m_currentOptionCount; i++)
	{
		if (uiMath::isPointInsideRect(position, m_boxes[i * 4].position, m_boxes[i * 4 + 2].position))
			return i;
	}
	return -1;
}

void uiSelectionBox::hide()
{
	m_onScreen = false;
}

void uiSelectionBox::draw(sf::RenderWindow& window, const sf::Vector2f& mousePos)
{
	if (!m_onScreen)
		return;
	window.draw(&m_boxes[0], m_boxes.size(), sf::Quads);
	for (int i = 0; i < m_currentOptionCount; i++)
	{
		window.draw(*m_optionTexts[i]);
	}
}

void uiSelectionBox::terminate()
{
	for (sf::Text* p : m_optionTexts)
		delete p;
}
