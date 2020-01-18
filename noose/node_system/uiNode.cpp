#include "uiNode.h"
//#include "uiNodeSystem.h"
#include "../math/uiMath.h"
#include "../interface/uiData.h"
#include "uiConnections.h"

#include <iostream>
#include <math.h>

#include "../nodeData.h"
#include "../types.h"

#define TEXT_COLOR 0xf0f0f0ff
#define BAR_COLOR 0x363636bb
#define SELECTED_BAR_COLOR 0x464646bb
#define CONTENT_RECT_COLOR 0x2c2c2cbb

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

#define PIN_COLLISION_DISTANCE 8

inline void setPinColor(sf::Vertex* firstVertex, int type)
{
	sf::Color theColor;
	switch (type)
	{
	case NS_TYPE_INT:
		theColor = sf::Color(0x4ca4fbff);
		break;
	case NS_TYPE_FLOAT:
		theColor = sf::Color(0xee1133ff);
		break;
	case NS_TYPE_VECTOR2I:
		theColor = sf::Color(0x56957aff);
		break;
	case NS_TYPE_COLOR:
		theColor = sf::Color(0xaa7700ff);
		break;
	case NS_TYPE_IMAGE:
		theColor = sf::Color(0x00bc44ff);
		break;
	}
	firstVertex[0].color = firstVertex[1].color = firstVertex[2].color = firstVertex[3].color = theColor;
}

uiNode::uiNode(const void* theNodeData, sf::Vector2f& initialPosition, void** inputFieldPointers, void(onValueChangedFunc)())
{
	nodeData* data = (nodeData*)theNodeData;
 	m_inputPinCount = data->inputPinCount;
	m_outputPinCount = data->outputPinCount;

	std::cout << "creating node " << data->nodeName << std::endl;

	m_contentHeight = (PROPERTY_HEIGHT + INPUT_FIELD_HEIGHT) * m_inputPinCount
				     +(PROPERTY_HEIGHT) * m_outputPinCount;

	int vertexCount = 8         // top bar and content rect
		+ 4 * m_inputPinCount   // pins
		+ 4 * m_outputPinCount; // just pins

	m_shapes.resize(vertexCount); // push vertices

	// set bar and content rect colors
	m_shapes[0].color = m_shapes[1].color = m_shapes[2].color = m_shapes[3].color = sf::Color(BAR_COLOR);
	m_shapes[4].color = m_shapes[5].color = m_shapes[6].color = m_shapes[7].color = sf::Color(CONTENT_RECT_COLOR);

	m_title = sf::Text(data->nodeName, uiData::font , NODE_TITLE_FONT_SIZE);
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
			m_inputFields[i].create(data->pinTypes[i], inputFieldPointers[i], onValueChangedFunc);
	}

	setPosition(initialPosition);
}

uiNode::~uiNode()
{
	delete[] m_pinNameTexts;
	delete[] m_inputFields;
	
	m_connectedLines.clear();
	m_shapes.clear();
	std::cout << "node deleted\n";
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

uiNode::MousePos uiNode::mouseOver(const sf::Vector2f& mousePos, int& index, int& subIndex)
{
	sf::Vector2f p = m_shapes[3].position; // top left corner position
	sf::FloatRect r(p, sf::Vector2f(NODE_WIDTH, BAR_HEIGHT + m_contentHeight));

	// check if its out
	if (!uiMath::isPointInsideRect(mousePos, r))
		return MousePos::Outside;

	// top bar check
	r.height = BAR_HEIGHT;
	if (uiMath::isPointInsideRect(mousePos, r))
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
		if (uiMath::distance(pinCenter, mousePos) < PIN_COLLISION_DISTANCE)
		{
			index = i;
			return MousePos::Pin;
		}
	}

	pinCenter.x = p.x + NODE_WIDTH - PIN_RECT_MARGIN;

	for (int i = 0; i < m_outputPinCount; i++)
	{
		pinCenter.y = p.y + (PROPERTY_HEIGHT + INPUT_FIELD_HEIGHT) * m_inputPinCount + PROPERTY_HEIGHT * (i + 0.5f);
		if (uiMath::distance(pinCenter, mousePos) < PIN_COLLISION_DISTANCE)
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
	return uiMath::isPointInsideRect(mousePosInWorld, topBar);
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

	for (lineInfo& i : m_connectedLines)
		uiConnections::displacePoint(displacement, i.lineIndex, i.pin >= m_inputPinCount);
}

void uiNode::attachConnectionPoint(int lineIndex, int pin)
{
	// attach line
	m_connectedLines.emplace_back();
	m_connectedLines.back().lineIndex = lineIndex;
	m_connectedLines.back().pin = pin;

	if (pin < m_inputPinCount) // is input pin
		m_inputFields[pin].disable(); // disable input field
}

void uiNode::setLineIndexAsDisconnected(int lineIndex)
{
	for (int i = 0; i < m_connectedLines.size(); i++)
	{
		if (m_connectedLines[i].lineIndex == lineIndex)
		{
			int pin = m_connectedLines[i].pin;
			if (pin < m_inputPinCount) // line is connected to an input pin
				m_inputFields[pin].enable(); // enable input field

			//std::cout << "line " << m_connectedLines[i] << " being removed from node " << this << std::endl;
			m_connectedLines.erase(m_connectedLines.begin() + i);
			break;
		}
	}
}

void uiNode::bindInputField(int index, int subIndex)
{
	m_inputFields[index].bind(subIndex);
}

sf::Vector2f uiNode::getPinPosition(int index)
{
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

int* uiNode::getConnectedLinesInfo(int& count)
{
	count = m_connectedLines.size();
	int* res = new int[m_connectedLines.size()];
	for (int i = 0; i < m_connectedLines.size(); i++)
		res[i] = m_connectedLines[i].lineIndex;
	return res;

	/*std::cout << "sizeeeee: " << m_connectedLines.size();
	std::cout << std::endl;
	count = m_connectedLines.size();
	return &(m_connectedLines[0].lineIndex);*/
}
