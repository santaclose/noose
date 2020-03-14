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

void uiSelectionBox::initialize()
{
	m_box.resize(4, sf::Vertex(sf::Vector2f(0.0f, 0.0f), sf::Color(COLOR)));
	m_box[0].texCoords.x = m_box[0].texCoords.y = m_box[1].texCoords.x = m_box[3].texCoords.y = 0.0;
	m_box[2].texCoords.x = m_box[2].texCoords.y = m_box[1].texCoords.y = m_box[3].texCoords.x = 1.0;

	if (!m_mouseOverShader.loadFromFile("res/shaders/selectionBox.shader", sf::Shader::Fragment))
		std::cout << "[UI] Failed to load selection box shad\n";
}

void uiSelectionBox::display(const sf::Vector2f& position, const std::vector<std::string>& options)
{
	m_onScreen = true;
	int prevSize = m_currentOptionCount;
	m_currentOptionCount = options.size();

	if (m_currentOptionCount > prevSize)
		m_optionTexts.resize(m_currentOptionCount, nullptr);

	sf::Vector2f cursor = position;// -sf::Vector2f(BOX_WIDTH / 2.0, BOX_HEIGHT / 2.0);
	m_box[0].position.x = m_box[1].position.x = cursor.x;
	m_box[0].position.y = m_box[3].position.y = cursor.y;
	m_box[2].position.x = m_box[3].position.x = cursor.x + BOX_WIDTH;
	m_box[1].position.y = m_box[2].position.y = cursor.y + m_currentOptionCount * BOX_HEIGHT;
	for (int i = 0; i < m_currentOptionCount; i++)
	{
		if (m_optionTexts[i] == nullptr)
			m_optionTexts[i] = new sf::Text(options[i], uiData::font, OPTION_FONT_SIZE);
		else
			m_optionTexts[i]->setString(options[i]);

		m_optionTexts[i]->setPosition(cursor + sf::Vector2f(TEXT_MARGIN_X, TEXT_MARGIN_Y));

		cursor.y += BOX_HEIGHT;
	}
	m_mouseOverShader.setUniform("boxCount", (float)m_currentOptionCount);
}

int uiSelectionBox::mouseOver(const sf::Vector2f& position)
{
	sf::FloatRect boxRect(m_box[0].position, sf::Vector2f(BOX_WIDTH, BOX_HEIGHT));
	for (int i = 0; i < m_currentOptionCount; i++)
	{
		if (uiMath::isPointInsideRect(position, boxRect))
			return i;
		boxRect.top += BOX_HEIGHT;
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

	m_mouseOverShader.setUniform("mousePos", sf::Glsl::Vec2((mousePos - m_box[0].position) / (m_box[2].position - m_box[0].position)));
	window.draw(&m_box[0], m_box.size(), sf::Quads, &m_mouseOverShader);
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
