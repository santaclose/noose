#include "uiNode.h"

#include "../math/nooseMath.h"
#include "../interface/uiData.h"
#include "uiConnections.h"

#include <iostream>
#include <math.h>

#include "../nodeData.h"
#include "../types.h"
#include "../type2color.h"

#define TEXT_COLOR 0xf0f0f0ff
#define BAR_COLOR 0x424242bb
#define SELECTED_BAR_COLOR 0x6a6a6abb
#define CONTENT_RECT_COLOR 0x353535bb

#define INPUT_FIELD_HEIGHT 20

#define NODE_TITLE_FONT_SIZE 14
#define PIN_TEXT_FONT_SIZE 11
#define INTERACTIVE_COMPONENT_TEXT_FONT_SIZE 12

#define BAR_HEIGHT 25
#define PROPERTY_HEIGHT 26
#define NODE_WIDTH 180
#define CONTENT_MARGIN_TOP 10

#define TEXT_MARGIN_TOP 4
#define TEXT_MARGIN_LEFT 10

#define PIN_RECT_SIZE 7
#define PIN_RECT_MARGIN 10

#define PIN_TEXT_MARGIN_X 15
#define PIN_TEXT_MARGIN_Y -8
#define CHARACTER_WIDTH 7

#define PIN_COLLISION_DISTANCE 12

inline void setPinColor(sf::Vertex* firstVertex, int type)
{
	firstVertex[0].color = firstVertex[1].color = firstVertex[2].color = firstVertex[3].color = sf::Color(type2color::get(type));
}

uiNode::uiNode(
	const nodeData* data,
	bool nodeCenterInPosition,
	sf::Vector2f& initialPosition,
	const std::vector<void*>& inputFieldPointers,
	void(onValueChangedFunc)(),
	uiSelectionBox* selectionBox)
{
	m_nodeName = data->nodeName;
 	m_inputPinCount = data->inputPinCount;
	m_outputPinCount = data->outputPinCount;

	//std::cout << "creating node " << data->nodeName << std::endl;

	m_contentHeight = (PROPERTY_HEIGHT + INPUT_FIELD_HEIGHT) * m_inputPinCount
				     +(PROPERTY_HEIGHT) * m_outputPinCount;

	int vertexCount = 8         // top bar and content rect
		+ 4 * m_inputPinCount   // pins
		+ 4 * m_outputPinCount;

	m_shapes.resize(vertexCount); // push vertices

	// set bar and content rect colors
	m_shapes[0].color = m_shapes[1].color = m_shapes[2].color = m_shapes[3].color = sf::Color(BAR_COLOR);
	m_shapes[4].color = m_shapes[5].color = m_shapes[6].color = m_shapes[7].color = sf::Color(CONTENT_RECT_COLOR);

	m_title = sf::Text(data->nodeName, uiData::font, NODE_TITLE_FONT_SIZE);
	m_title.setFillColor(sf::Color(TEXT_COLOR));

	m_pinNameTexts = new sf::Text[m_inputPinCount + m_outputPinCount];
	m_inputFields = new uiInputField[m_inputPinCount];

	for (int i = 0; i < m_inputPinCount + m_outputPinCount; i++)
	{
		// assign color to pin vertices
		setPinColor(&m_shapes[8 + i * 4], data->pinTypes[i]);

		// create pin texts
		m_pinNameTexts[i] = sf::Text(data->pinNames[i], uiData::font, PIN_TEXT_FONT_SIZE);

		// create input fields
		if (i < m_inputPinCount)
			m_inputFields[i].create(data->pinTypes[i], inputFieldPointers[i], onValueChangedFunc, &(data->pinEnumOptions[i]), selectionBox);
	}

	sf::Vector2f nodePosOnScreen;
	if (nodeCenterInPosition)
		nodePosOnScreen = sf::Vector2f(
			initialPosition.x - NODE_WIDTH / 2.0,
			initialPosition.y - m_contentHeight / 2.0);
	else
		nodePosOnScreen = initialPosition;
	setPosition(nodePosOnScreen);
}

uiNode::~uiNode()
{
	delete[] m_pinNameTexts;
	delete[] m_inputFields;
	//std::cout << "node deleted\n";
}

void uiNode::setPosition(sf::Vector2f& newPosition)
{
	// bar rect
	m_shapes[0].position = newPosition + sf::Vector2f(0, BAR_HEIGHT);
	m_shapes[1].position = newPosition + sf::Vector2f(NODE_WIDTH, BAR_HEIGHT);
	m_shapes[2].position = newPosition + sf::Vector2f(NODE_WIDTH, 0);
	m_shapes[3].position = newPosition;

	// content rect
	m_shapes[4].position = newPosition + sf::Vector2f(0, BAR_HEIGHT + m_contentHeight);
	m_shapes[5].position = newPosition + sf::Vector2f(NODE_WIDTH, BAR_HEIGHT + m_contentHeight);
	m_shapes[6].position = newPosition + sf::Vector2f(NODE_WIDTH, BAR_HEIGHT);
	m_shapes[7].position = newPosition + sf::Vector2f(0, BAR_HEIGHT);

	sf::Vector2f pinCenter;
	pinCenter.x = newPosition.x + PIN_RECT_MARGIN;
	for (int i = 0; i < m_inputPinCount; i++)
	{
		pinCenter.y = newPosition.y + BAR_HEIGHT + PROPERTY_HEIGHT * (i + 0.5f) + INPUT_FIELD_HEIGHT * i;

		// set pin positions
		m_shapes[8 + i * 4].position = pinCenter + sf::Vector2f(-PIN_RECT_SIZE / 2.0f, 0.0f);
		m_shapes[8 + i * 4 + 1].position = pinCenter + sf::Vector2f(0.0f, PIN_RECT_SIZE / 2.0f);
		m_shapes[8 + i * 4 + 2].position = pinCenter + sf::Vector2f(PIN_RECT_SIZE / 2.0f, 0.0f);
		m_shapes[8 + i * 4 + 3].position = pinCenter + sf::Vector2f(0.0f, -PIN_RECT_SIZE / 2.0f);

		m_pinNameTexts[i].setPosition(pinCenter + sf::Vector2f(PIN_TEXT_MARGIN_X, PIN_TEXT_MARGIN_Y));

		m_inputFields[i].setPosition(newPosition + sf::Vector2f(0, BAR_HEIGHT + PROPERTY_HEIGHT * (i + 1) + INPUT_FIELD_HEIGHT * i), NODE_WIDTH, INPUT_FIELD_HEIGHT);
	}
	pinCenter.x = newPosition.x + NODE_WIDTH - PIN_RECT_MARGIN;
	for (int i = 0; i < m_outputPinCount; i++)
	{
		pinCenter.y = newPosition.y + BAR_HEIGHT + (PROPERTY_HEIGHT + INPUT_FIELD_HEIGHT) * m_inputPinCount + PROPERTY_HEIGHT * (i + 0.5f);

		m_shapes[8 + m_inputPinCount * 4 + i * 4].position = pinCenter + sf::Vector2f(-PIN_RECT_SIZE / 2.0f, 0.0f);
		m_shapes[8 + m_inputPinCount * 4 + i * 4 + 1].position = pinCenter + sf::Vector2f(0.0f, PIN_RECT_SIZE / 2.0f);
		m_shapes[8 + m_inputPinCount * 4 + i * 4 + 2].position = pinCenter + sf::Vector2f(PIN_RECT_SIZE / 2.0f, 0.0f);
		m_shapes[8 + m_inputPinCount * 4 + i * 4 + 3].position = pinCenter + sf::Vector2f(0.0f, -PIN_RECT_SIZE / 2.0f);

		m_pinNameTexts[m_inputPinCount + i].setPosition(pinCenter - sf::Vector2f(PIN_TEXT_MARGIN_X + m_pinNameTexts[m_inputPinCount + i].getLocalBounds().width, -PIN_TEXT_MARGIN_Y));
	}

	m_title.setPosition(newPosition + sf::Vector2f(TEXT_MARGIN_LEFT, TEXT_MARGIN_TOP));
}

void uiNode::setInput(int inputIndex, const void* data)
{
	m_inputFields[inputIndex].setValue(data);
}

const sf::Vector2f& uiNode::getPosition() const
{
	return m_shapes[3].position;
}

void uiNode::draw(sf::RenderWindow& window)
{
	//std::cout << "drawing node " << m_title.getString().toAnsiString() << std::endl;

	window.draw(&m_shapes[0], m_shapes.size(), sf::Quads);
	window.draw(m_title);
	for (int i = 0; i < m_inputPinCount + m_outputPinCount; i++)
	{
		if (i < m_inputPinCount)
		{
			if (m_inputFields[i].isEnabled())
				m_inputFields[i].draw(window);
		}
		window.draw(m_pinNameTexts[i]);
	}
}

// check if mouse is over some part of the node
uiNode::MousePos uiNode::mouseOver(const sf::Vector2f& mousePos, int& index, int& subIndex)
{
	sf::Vector2f p = m_shapes[3].position; // top left corner position
	sf::FloatRect r(p, sf::Vector2f(NODE_WIDTH, BAR_HEIGHT + m_contentHeight));

	// check if its out
	if (!nooseMath::isPointInsideRect(mousePos, r))
		return MousePos::Outside;

	// top bar check
	r.height = BAR_HEIGHT;
	if (nooseMath::isPointInsideRect(mousePos, r))
		return MousePos::TopBar;

	// input fields check
	for (int i = 0; i < m_inputPinCount; i++)
	{
		if (!m_inputFields[i].isEnabled())
			continue; // no collision if not enabled

		if (m_inputFields[i].mouseOver(mousePos, subIndex)) // mouse was over input field
		{
			index = i;
			return MousePos::InputField;
		}
	}

	// pin check
	p = m_shapes[0].position;
	sf::Vector2f pinCenter;

	pinCenter.x = p.x + PIN_RECT_MARGIN;

	for (int i = 0; i < m_inputPinCount; i++)
	{
		pinCenter.y = p.y + PROPERTY_HEIGHT * (i + 0.5f) + INPUT_FIELD_HEIGHT * i;
		if (nooseMath::distance(pinCenter, mousePos) < PIN_COLLISION_DISTANCE)
		{
			index = i;
			return MousePos::Pin;
		}
	}

	pinCenter.x = p.x + NODE_WIDTH - PIN_RECT_MARGIN;

	for (int i = 0; i < m_outputPinCount; i++)
	{
		pinCenter.y = p.y + (PROPERTY_HEIGHT + INPUT_FIELD_HEIGHT) * m_inputPinCount + PROPERTY_HEIGHT * (i + 0.5f);
		if (nooseMath::distance(pinCenter, mousePos) < PIN_COLLISION_DISTANCE)
		{
			index = m_inputPinCount + i;
			return MousePos::Pin;
		}
	}

	return MousePos::OtherInside;
}

bool uiNode::mouseOverTopBar(const sf::Vector2f& mousePosInWorld)
{
	sf::Vector2f p = m_shapes[3].position; // top left corener position
	sf::FloatRect topBar(p, sf::Vector2f(NODE_WIDTH, BAR_HEIGHT));
	return nooseMath::isPointInsideRect(mousePosInWorld, topBar);
}

void uiNode::paintAsSelected()
{
	//	std::cout << "painting as selected\n";
	m_shapes[0].color = m_shapes[1].color = m_shapes[2].color = m_shapes[3].color = sf::Color(SELECTED_BAR_COLOR);
}

void uiNode::paintAsUnselected()
{
	//	std::cout << "painting as UNselected\n";
	m_shapes[0].color = m_shapes[1].color = m_shapes[2].color = m_shapes[3].color = sf::Color(BAR_COLOR);
}

void uiNode::displace(const sf::Vector2f& displacement)
{
	sf::Vector2f tempv = m_shapes[3].position /* top left corener position*/ + displacement;
	setPosition(tempv);

	for (int i = 0; i < m_connectedLineIndices.size(); i++)
		uiConnections::displacePoint(displacement, m_connectedLineIndices[i], m_connectedLinePins[i] >= m_inputPinCount);
}

void uiNode::attachConnectionPoint(int lineIndex, int pin)
{
	// attach line
	m_connectedLineIndices.push_back(lineIndex);
	m_connectedLinePins.push_back(pin);
	//std::cout << "attaching line index " << lineIndex << std::endl;

	if (pin < m_inputPinCount) // is input pin
		m_inputFields[pin].disable(); // disable input field
}

void uiNode::setLineIndexAsDisconnected(int lineIndex)
{
	for (int i = 0; i < m_connectedLineIndices.size(); i++)
	{
		if (m_connectedLineIndices[i] == lineIndex)
		{
			int pin = m_connectedLinePins[i];
			if (pin < m_inputPinCount) // line is connected to an input pin
				m_inputFields[pin].enable(); // enable input field

			//std::cout << "line " << m_connectedLines[i] << " being removed from node " << this << std::endl;
			m_connectedLinePins.erase(m_connectedLinePins.begin() + i);
			m_connectedLineIndices.erase(m_connectedLineIndices.begin() + i);
			break;
		}
	}
}

bool uiNode::canConnectToPin(int pin)
{
	bool isConnected = false;
	for (int linePin : m_connectedLinePins)
	{
		if (linePin == pin)
		{
			isConnected = true;
			break;
		}
	}

	// cannot connect if its an input pin and is already connected
	return (pin >= m_inputPinCount || !isConnected);
}

void uiNode::bindInputField(int index, int subIndex, uiInputField::InteractionMode interactionMode)
{
	m_inputFields[index].bind(subIndex, interactionMode);
}

sf::Vector2f uiNode::getPinPosition(int index)
{
	//std::cout << "getting position of pin " << index << std::endl;

	sf::Vector2f p = m_shapes[0].position; // bottom left corner position of topbar

	if (index < m_inputPinCount)
	{
		p.x += PIN_RECT_MARGIN;
		p.y += PROPERTY_HEIGHT * (index + 0.5f) + INPUT_FIELD_HEIGHT * index;
	}
	else
	{
		p.x += NODE_WIDTH - PIN_RECT_MARGIN;
		p.y += (PROPERTY_HEIGHT + INPUT_FIELD_HEIGHT) * m_inputPinCount + PROPERTY_HEIGHT * (index-m_inputPinCount + 0.5f);
	}
	return p;
}

const sf::Color& uiNode::getPinColor(int index)
{
	return m_shapes[8 + index * 4].color;
}

int uiNode::getInputPinCount()
{
	return m_inputPinCount;
}

std::vector<int> uiNode::getConnectedLines()
{
	return m_connectedLineIndices;
}