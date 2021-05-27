#include "uiMessageBox.h"

#include "../math/vectorOperators.h"
#include "../math/nooseMath.h"
#include "../utils.h"
#include "uiData.h"

#include <iostream>

#define FONT_SIZE 14
#define BOX_WIDTH 300
#define BOX_HEIGHT 86
#define COLOR 0x3a3a3aff
#define MARGIN 16
#define SEPARATION 14
#define BUTTON_HEIGHT 26
#define TEXT_OFFSET_Y 5

void uiMessageBox::initialize()
{
	m_box.resize(4, sf::Vertex(sf::Vector2f(0.0f, 0.0f), sf::Color(COLOR)));
	m_box[0].texCoords.x = m_box[0].texCoords.y = m_box[1].texCoords.x = m_box[3].texCoords.y = 0.0;
	m_box[2].texCoords.x = m_box[2].texCoords.y = m_box[1].texCoords.y = m_box[3].texCoords.x = 1.0;

	if (!m_shader.loadFromFile(utils::getProgramDirectory() + "assets/shaders/messageBox.shader", sf::Shader::Fragment))
		std::cout << "[UI] Failed to load message box shader\n";
	m_shader.setUniform("margin", sf::Glsl::Vec2((float)MARGIN / (float)BOX_WIDTH, (float)MARGIN / (float)BOX_HEIGHT));
	m_shader.setUniform("separationX", (float)SEPARATION / (float)BOX_WIDTH);
	m_shader.setUniform("buttonHeight", (float)BUTTON_HEIGHT / (float)BOX_HEIGHT);
}

void uiMessageBox::display(const std::string& message, const std::vector<std::string>& options, const sf::Vector2f& centerPos)
{
	m_onScreen = true;
	int prevSize = m_currentOptionCount;
	m_currentOptionCount = options.size();

	m_box[0].position.x = m_box[1].position.x = centerPos.x - BOX_WIDTH * 0.5f;
	m_box[0].position.y = m_box[3].position.y = centerPos.y - BOX_HEIGHT * 0.5f;
	m_box[2].position.x = m_box[3].position.x = centerPos.x + BOX_WIDTH * 0.5f;
	m_box[1].position.y = m_box[2].position.y = centerPos.y + BOX_HEIGHT * 0.5f;

	m_messageText = sf::Text(message, uiData::font, FONT_SIZE);
	m_messageText.setPosition(
		((m_box[3].position + m_box[0].position) / 2.0f) + sf::Vector2f(0.0, MARGIN) +
		-(sf::Vector2f(m_messageText.getLocalBounds().width, m_messageText.getLocalBounds().height) / 2.0f));

	if (m_currentOptionCount > prevSize)
		m_optionTexts.resize(m_currentOptionCount, nullptr);

	m_buttonWidth = (BOX_WIDTH - MARGIN * 2.0f - (m_currentOptionCount - 1) * SEPARATION) / (float) m_currentOptionCount;

	sf::Vector2f cursor = m_box[1].position; // bottom left corner
	cursor += sf::Vector2f(MARGIN, -MARGIN - BUTTON_HEIGHT);
	for (int i = 0; i < m_currentOptionCount; i++)
	{
		if (m_optionTexts[i] == nullptr)
			m_optionTexts[i] = new sf::Text(options[i], uiData::font, FONT_SIZE);
		else
			m_optionTexts[i]->setString(options[i]);

		m_optionTexts[i]->setPosition(cursor + (sf::Vector2f(m_buttonWidth, BUTTON_HEIGHT) / 2.0f) +
			-(sf::Vector2f(m_optionTexts[i]->getLocalBounds().width, m_optionTexts[i]->getLocalBounds().height) / 2.0f) +
			sf::Vector2f(0.0, -TEXT_OFFSET_Y));

		cursor.x += m_buttonWidth + SEPARATION;
	}
	m_shader.setUniform("optionCount", m_currentOptionCount);
}

int uiMessageBox::mouseOver(const sf::Vector2f& position)
{
	sf::Vector2f cursor = m_box[1].position; // bottom left corner
	cursor += sf::Vector2f(MARGIN, -MARGIN - BUTTON_HEIGHT);

	for (int i = 0; i < m_currentOptionCount; i++)
	{
		if (nooseMath::isPointInsideRect(position, cursor, cursor + sf::Vector2f(m_buttonWidth, BUTTON_HEIGHT)))
			return i;
		cursor.x += m_buttonWidth + SEPARATION;
	}
	return -1;
}

void uiMessageBox::hide()
{
	m_onScreen = false;
}

void uiMessageBox::draw(sf::RenderWindow& window, const sf::Vector2f& mousePos)
{
	if (!m_onScreen)
		return;

	m_shader.setUniform("mousePos", sf::Glsl::Vec2((mousePos - m_box[0].position) / (m_box[2].position - m_box[0].position)));
	window.draw(&m_box[0], m_box.size(), sf::Quads, &m_shader);
	window.draw(m_messageText);
	for (int i = 0; i < m_currentOptionCount; i++)
		window.draw(*m_optionTexts[i]);
}

void uiMessageBox::terminate()
{
	for (sf::Text* p : m_optionTexts)
		delete p;
}
