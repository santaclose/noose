#include "uiNode.h"
#include "uiNodeSystem.h"
#include "uiMath.h"

#include <iostream>
#include <math.h>

#define TEXT_COLOR 0xf0f0f0ff
#define BAR_COLOR 0x161616bb
#define SELECTED_BAR_COLOR 0x6b6b6bbb
#define CONTENT_RECT_COLOR 0x323232bb

#define INTERACTIVE_BOX_COLOR 0x00000030
#define INTERACTIVE_BOX_HEIGHT 20

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

#define VERTICES_PER_PROPERTY 8


inline void setPinColor(sf::Vertex* firstVertex, uiNodeSystem::Types type)
{
	sf::Color theColor;
	switch (type)
	{
	case uiNodeSystem::Types::Integer:
		theColor = sf::Color(0x4ca4fbff);
		break;
	case uiNodeSystem::Types::Float:
		theColor = sf::Color(0xee1133ff);
		break;
	case uiNodeSystem::Types::Vector2i:
		theColor = sf::Color(0x56957aff);
		break;
	/*case uiNodeSystem::Types::Recti:
		rect.setFillColor(sf::Color(0x324afbff));
		break;*/
	case uiNodeSystem::Types::Image:
		theColor = sf::Color(0x00bc44ff);
		break;
	case uiNodeSystem::Types::Color:
		theColor = sf::Color(0xaa7700ff);
		break;
	}
	firstVertex[0].color = firstVertex[1].color = firstVertex[2].color = firstVertex[3].color = theColor;
}

inline void setInteractiveBoxColor(sf::Vertex* firstVertex)
{
	firstVertex[4].color = firstVertex[5].color = firstVertex[6].color = firstVertex[7].color = sf::Color(INTERACTIVE_BOX_COLOR);
}


uiNode::uiNode(const std::string& name, int numberOfInputPins, int numberOfOutputPins, const uiNodeSystem::Types* pinTypes, const std::string* pinNames, sf::Vector2f* initialPosition)
{
	inputPinCount = numberOfInputPins;
	outputPinCount = numberOfOutputPins;
	contentHeight = (PROPERTY_HEIGHT + INTERACTIVE_BOX_HEIGHT) * numberOfInputPins
				   +(PROPERTY_HEIGHT) * numberOfOutputPins;

	int vertexCount = 8 // top bar and content rect
		+ 8 * numberOfInputPins // pins and interactive boxes
		+ 4 * numberOfOutputPins; // just pins

	shapes.resize(vertexCount); // push vertices

	// set bar and content rect colors
	shapes[0].color = shapes[1].color = shapes[2].color = shapes[3].color = sf::Color(BAR_COLOR);
	shapes[4].color = shapes[5].color = shapes[6].color = shapes[7].color = sf::Color(CONTENT_RECT_COLOR);

	if (pinTypes != nullptr)
	{
		for (int i = 0; i < numberOfInputPins; i++)
		{
			// assign color to pin vertices
			setPinColor(&shapes[8 + i * 8], pinTypes[i]);

			// assign color to interactive box
			setInteractiveBoxColor(&shapes[8 + i * 8]);
		}
		for (int i = 0; i < numberOfOutputPins; i++) // for output pins there is no box
		{
			setPinColor(&shapes[8 + numberOfInputPins * 8 + i * 4], pinTypes[inputPinCount + i]);
		}
	}

	//std::cout << "node created: " << this << std::endl;

	title = sf::Text(name, uiNodeSystem::font , NODE_TITLE_FONT_SIZE);
	title.setFillColor(sf::Color(TEXT_COLOR));

	pinNameTexts = new sf::Text[inputPinCount + outputPinCount];
	for (int i = 0; i < inputPinCount + outputPinCount; i++)
	{
		pinNameTexts[i] = sf::Text(pinNames[i], uiNodeSystem::font, PIN_TEXT_FONT_SIZE);
	}

	sf::Vector2f pos = initialPosition == nullptr ? sf::Vector2f(0.0, 0.0) : *initialPosition;
	setPosition(pos);
}

uiNode::~uiNode()
{
	for (int i : lineIndices)
	{
		std::cout << "removing line " << i << std::endl;
		void* nodeAConnected;
		void* nodeBConnected;
		uiNodeConnections::getConnectedNodes(i, nodeAConnected, nodeBConnected);

		// no need to detach from this node cause its gonna be destroyed
		if (nodeAConnected == (void*)this)
			((uiNode*)nodeBConnected)->setLineIndexAsDisconnected(i);
		else
			((uiNode*)nodeAConnected)->setLineIndexAsDisconnected(i);

		std::cout << "uiNodeConnections removing line " << i << std::endl;
		uiNodeConnections::remove(i);
	}
	delete[] pinNameTexts;

	lineIndices.clear();
	shapes.clear();
	std::cout << "node deleted\n";
}

void uiNode::setPosition(sf::Vector2f& newPosition)
{
	// bar rect
	shapes[0].position = newPosition + sf::Vector2f(0, BAR_HEIGHT);
	shapes[1].position = newPosition + sf::Vector2f(NODE_WIDTH, BAR_HEIGHT);
	shapes[2].position = newPosition + sf::Vector2f(NODE_WIDTH, 0);
	shapes[3].position = newPosition;

	// content rect
	shapes[4].position = newPosition + sf::Vector2f(0, BAR_HEIGHT + contentHeight);
	shapes[5].position = newPosition + sf::Vector2f(NODE_WIDTH, BAR_HEIGHT + contentHeight);
	shapes[6].position = newPosition + sf::Vector2f(NODE_WIDTH, BAR_HEIGHT);
	shapes[7].position = newPosition + sf::Vector2f(0, BAR_HEIGHT);

	sf::Vector2f pinCenter;
	pinCenter.x = newPosition.x + PIN_RECT_MARGIN;
	for (int i = 0; i < inputPinCount; i++)
	{
		pinCenter.y = newPosition.y + BAR_HEIGHT + PROPERTY_HEIGHT * (i + 0.5f) + INTERACTIVE_BOX_HEIGHT * i;

		// set pin positions
		shapes[8 + i * 8].position = pinCenter + sf::Vector2f(-PIN_RECT_SIZE / 2.0f, 0.0f);
		shapes[8 + i * 8 + 1].position = pinCenter + sf::Vector2f(0.0f, PIN_RECT_SIZE / 2.0f);
		shapes[8 + i * 8 + 2].position = pinCenter + sf::Vector2f(PIN_RECT_SIZE / 2.0f, 0.0f);
		shapes[8 + i * 8 + 3].position = pinCenter + sf::Vector2f(0.0f, -PIN_RECT_SIZE / 2.0f);

		pinNameTexts[i].setPosition(pinCenter + sf::Vector2f(PIN_TEXT_MARGIN_X, PIN_TEXT_MARGIN_Y));

		// interactive box
		shapes[8 + i * 8 + 4].position = newPosition + sf::Vector2f(NODE_WIDTH/15.0, BAR_HEIGHT + (PROPERTY_HEIGHT + INTERACTIVE_BOX_HEIGHT) * (i + 1) - (INTERACTIVE_BOX_HEIGHT / 6.0));// /8.0 for margin
		shapes[8 + i * 8 + 5].position = newPosition + sf::Vector2f(NODE_WIDTH-NODE_WIDTH/15.0, BAR_HEIGHT + (PROPERTY_HEIGHT + INTERACTIVE_BOX_HEIGHT) * (i + 1) - (INTERACTIVE_BOX_HEIGHT / 6.0));// /8.0 for margin
		shapes[8 + i * 8 + 6].position = newPosition + sf::Vector2f(NODE_WIDTH-NODE_WIDTH/15.0, BAR_HEIGHT + (PROPERTY_HEIGHT + INTERACTIVE_BOX_HEIGHT) * i + PROPERTY_HEIGHT);
		shapes[8 + i * 8 + 7].position = newPosition + sf::Vector2f(NODE_WIDTH/15.0, BAR_HEIGHT + (PROPERTY_HEIGHT + INTERACTIVE_BOX_HEIGHT) * i + PROPERTY_HEIGHT);

	}
	pinCenter.x = newPosition.x + NODE_WIDTH - PIN_RECT_MARGIN;
	for (int i = 0; i < outputPinCount; i++)
	{
		pinCenter.y = newPosition.y + BAR_HEIGHT + (PROPERTY_HEIGHT + INTERACTIVE_BOX_HEIGHT) * inputPinCount + PROPERTY_HEIGHT * (i + 0.5f);

		shapes[8 + inputPinCount * 8 + i * 4].position = pinCenter + sf::Vector2f(-PIN_RECT_SIZE / 2.0f, 0.0f);
		shapes[8 + inputPinCount * 8 + i * 4 + 1].position = pinCenter + sf::Vector2f(0.0f, PIN_RECT_SIZE / 2.0f);
		shapes[8 + inputPinCount * 8 + i * 4 + 2].position = pinCenter + sf::Vector2f(PIN_RECT_SIZE / 2.0f, 0.0f);
		shapes[8 + inputPinCount * 8 + i * 4 + 3].position = pinCenter + sf::Vector2f(0.0f, -PIN_RECT_SIZE / 2.0f);

		pinNameTexts[inputPinCount + i].setPosition(pinCenter - sf::Vector2f(PIN_TEXT_MARGIN_X + pinNameTexts[inputPinCount + i].getLocalBounds().width, -PIN_TEXT_MARGIN_Y));
	}

	title.setPosition(newPosition + sf::Vector2f(TEXT_MARGIN_LEFT, TEXT_MARGIN_TOP));
}

void uiNode::draw(sf::RenderWindow& window)
{
	window.draw(&shapes[0], shapes.size(), sf::Quads);
	window.draw(title);
	for (int i = 0; i < inputPinCount + outputPinCount; i++)
	{
		window.draw(pinNameTexts[i]);
	}
}

bool uiNode::mouseOverNode(const sf::Vector2f& mousePosInWorld)
{
	sf::Vector2f p = shapes[3].position; // top left corener position
	sf::FloatRect r(p, sf::Vector2f(NODE_WIDTH, BAR_HEIGHT + contentHeight));
	return uiMath::isPointInsideRect(mousePosInWorld, r);
}
bool uiNode::mouseOverTopBar(const sf::Vector2f& mousePosInWorld)
{
	sf::Vector2f p = shapes[3].position; // top left corener position
	sf::FloatRect topBar(p, sf::Vector2f(NODE_WIDTH, BAR_HEIGHT));
	return uiMath::isPointInsideRect(mousePosInWorld, topBar);
}
int uiNode::mouseOverPin(const sf::Vector2f& mousePosInWorld, sf::Color*& returnPinColor, sf::Vector2f& pinPosition, int& direction)
{
	sf::Vector2f p = shapes[0].position; // bottom left corner position of topbar
	sf::FloatRect contentRect(p, sf::Vector2f(NODE_WIDTH, contentHeight));
	if (!uiMath::isPointInsideRect(mousePosInWorld, contentRect)) // if cursor is outside content rect its not over any pin
		return -1;

	sf::Vector2f pinCenter;

	pinCenter.x = p.x + PIN_RECT_MARGIN;
	direction = 1;
	for (int i = 0; i < inputPinCount; i++)
	{
		pinCenter.y = p.y + PROPERTY_HEIGHT * (i + 0.5f) + INTERACTIVE_BOX_HEIGHT * i;
		if (uiMath::distance(pinCenter, mousePosInWorld) < PIN_COLLISION_DISTANCE)
		{
			pinPosition = pinCenter;
			returnPinColor = &(shapes[8 + i * 8].color);
			return i;
		}
	}

	pinCenter.x = p.x + NODE_WIDTH - PIN_RECT_MARGIN;
	direction = 0;
	for (int i = 0; i < outputPinCount; i++)
	{
		pinCenter.y = p.y + (PROPERTY_HEIGHT + INTERACTIVE_BOX_HEIGHT) * inputPinCount + PROPERTY_HEIGHT * (i + 0.5f);
		if (uiMath::distance(pinCenter, mousePosInWorld) < PIN_COLLISION_DISTANCE)
		{
			pinPosition = pinCenter;
			returnPinColor = &(shapes[8 + inputPinCount * 8 + i * 4].color);
			return inputPinCount + i;
		}
	}

	return -1;
}

void uiNode::paintAsSelected()
{
//	std::cout << "painting as selected\n";
	shapes[0].color = shapes[1].color = shapes[2].color = shapes[3].color = sf::Color(SELECTED_BAR_COLOR);
}

void uiNode::paintAsUnselected()
{
//	std::cout << "painting as UNselected\n";
	shapes[0].color = shapes[1].color = shapes[2].color = shapes[3].color = sf::Color(BAR_COLOR);
}

void uiNode::move(const sf::Vector2f& displacement)
{
	sf::Vector2f tempv = shapes[3].position /* top left corener position*/ + displacement;
	setPosition(tempv);

	for (int i : lineIndices)
	{
		uiNodeConnections::movePoint(i, (void*)this, displacement);
	}
}

void uiNode::attachConnectionPoint(int lineIndex)
{
	lineIndices.push_back(lineIndex);
}

// remove all connections to this line
void uiNode::setLineIndexAsDisconnected(int lineIndex)
{
	for (int i = 0; i < lineIndices.size(); i++)
	{
		if (lineIndices[i] == lineIndex)
		{
			//std::cout << "line " << lineIndices[i] << " being removed from node " << this << std::endl;
			lineIndices.erase(lineIndices.begin() + i);
			i--;
		}
	}
}

bool uiNode::isInputAndAlreadyConnected(int pin)
{
	for (int i : lineIndices)
	{
		if (uiNodeConnections::lines[i].pinA == pin && uiNodeConnections::lines[i].nodeA == (void*)this ||
			uiNodeConnections::lines[i].pinB == pin && uiNodeConnections::lines[i].nodeB == (void*)this) // is connected
		{
			return pin < inputPinCount; // is connected and is input
		}
	}
	// is not connected
	return false;
}


void uiNode::print()
{
	std::cout << '\t' << this << ' ' << inputPinCount << ' ' << outputPinCount << '\n';
}