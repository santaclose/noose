#include "uiNode.h"
#include "uiNodeSystem.h"
#include "uiMath.h"

#include <iostream>
#include <math.h>

#include "nodeData.h"

#define TEXT_COLOR 0xf0f0f0ff
#define BAR_COLOR 0x161616bb
#define SELECTED_BAR_COLOR 0x6b6b6bbb
#define CONTENT_RECT_COLOR 0x323232bb

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

#define VERTICES_PER_PROPERTY 8

inline void reserveDataForPin(uiNodeSystem::Types type, void*& pointer)
{
	switch (type)
	{
	case uiNodeSystem::Types::Integer:
		pointer = new int(0);
		break;
	case uiNodeSystem::Types::Float:
		pointer = new float(0.0f);
		break;
	case uiNodeSystem::Types::Vector2i:
		pointer = new sf::Vector2i(0, 0);
		break;
	case uiNodeSystem::Types::Color:
		pointer = new sf::Color(255, 0, 255, 255);
		break;
	case uiNodeSystem::Types::Image:
		pointer = new sf::RenderTexture();
		break;
	}
}

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
	case uiNodeSystem::Types::Color:
		theColor = sf::Color(0xaa7700ff);
		break;
	/*case uiNodeSystem::Types::Recti:
		rect.setFillColor(sf::Color(0x324afbff));
		break;*/
	case uiNodeSystem::Types::Image:
		theColor = sf::Color(0x00bc44ff);
		break;
	}
	firstVertex[0].color = firstVertex[1].color = firstVertex[2].color = firstVertex[3].color = theColor;
}

uiNode::uiNode(const void* theNodeData, sf::Vector2f& initialPosition)
{
	nodeData* data = (nodeData*)theNodeData;
	inputPinCount = data->inputPinCount;
	outputPinCount = data->outputPinCount;
	contentHeight = (PROPERTY_HEIGHT + INPUT_FIELD_HEIGHT) * inputPinCount
				   +(PROPERTY_HEIGHT) * outputPinCount;

	int vertexCount = 8 // top bar and content rect
		+ 4 * inputPinCount // pins and interactive boxes
		+ 4 * outputPinCount; // just pins

	shapes.resize(vertexCount); // push vertices

	// set bar and content rect colors
	shapes[0].color = shapes[1].color = shapes[2].color = shapes[3].color = sf::Color(BAR_COLOR);
	shapes[4].color = shapes[5].color = shapes[6].color = shapes[7].color = sf::Color(CONTENT_RECT_COLOR);

	for (int i = 0; i < inputPinCount; i++)
	{
		// assign color to pin vertices
		setPinColor(&shapes[8 + i * 4], data->pinTypes[i]);
	}
	for (int i = 0; i < outputPinCount; i++) // there is no box for output pins
	{
		setPinColor(&shapes[8 + inputPinCount * 4 + i * 4], data->pinTypes[inputPinCount + i]);
	}

	title = sf::Text(data->nodeName, uiNodeSystem::font , NODE_TITLE_FONT_SIZE);
	title.setFillColor(sf::Color(TEXT_COLOR));

	pinNameTexts = new sf::Text[inputPinCount + outputPinCount];

	for (int i = 0; i < inputPinCount + outputPinCount; i++)
	{
		pinNameTexts[i] = sf::Text(data->pinNames[i], uiNodeSystem::font, PIN_TEXT_FONT_SIZE);
	}

	// get a copy of the types
	pinTypes = new uiNodeSystem::Types[inputPinCount + outputPinCount];
	memcpy(pinTypes, &(data->pinTypes[0]), sizeof(uiNodeSystem::Types) * (inputPinCount + outputPinCount));

	pinDataPointers = (void**) malloc(sizeof(void*) * (inputPinCount + outputPinCount));
	for (int i = 0; i < (inputPinCount + outputPinCount); i++)
	{
		reserveDataForPin(pinTypes[i], pinDataPointers[i]);
	}

	inputFields = new uiInputField[inputPinCount];
	for (int i = 0; i < inputPinCount; i++)
	{
		inputFields[i].initialize(data->pinTypes[i], pinDataPointers[i]);
	}

	setPosition(initialPosition);

	/*for (int i = 0; i < inputPinCount + outputPinCount; i++)
	{
		switch (pinTypes[i])
		{
		case uiNodeSystem::Types::Integer:
			std::cout << *((int*)pinData[i]) << std::endl;
			break;
		case uiNodeSystem::Types::Color:
			std::cout << "color\n";
			std::cout << ((sf::Color*)pinData[i]) << std::endl;
		}
	}*/
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
	//delete[] interactiveBoxTexts;
	delete[] inputFields;
	delete[] pinTypes;
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
		pinCenter.y = newPosition.y + BAR_HEIGHT + PROPERTY_HEIGHT * (i + 0.5f) + INPUT_FIELD_HEIGHT * i;

		// set pin positions
		shapes[8 + i * 4].position = pinCenter + sf::Vector2f(-PIN_RECT_SIZE / 2.0f, 0.0f);
		shapes[8 + i * 4 + 1].position = pinCenter + sf::Vector2f(0.0f, PIN_RECT_SIZE / 2.0f);
		shapes[8 + i * 4 + 2].position = pinCenter + sf::Vector2f(PIN_RECT_SIZE / 2.0f, 0.0f);
		shapes[8 + i * 4 + 3].position = pinCenter + sf::Vector2f(0.0f, -PIN_RECT_SIZE / 2.0f);

		pinNameTexts[i].setPosition(pinCenter + sf::Vector2f(PIN_TEXT_MARGIN_X, PIN_TEXT_MARGIN_Y));

		inputFields[i].setPosition(newPosition + sf::Vector2f(0, BAR_HEIGHT + PROPERTY_HEIGHT * (i+1) + INPUT_FIELD_HEIGHT * i), NODE_WIDTH, INPUT_FIELD_HEIGHT);
	}
	pinCenter.x = newPosition.x + NODE_WIDTH - PIN_RECT_MARGIN;
	for (int i = 0; i < outputPinCount; i++)
	{
		pinCenter.y = newPosition.y + BAR_HEIGHT + (PROPERTY_HEIGHT + INPUT_FIELD_HEIGHT) * inputPinCount + PROPERTY_HEIGHT * (i + 0.5f);

		shapes[8 + inputPinCount * 4 + i * 4].position = pinCenter + sf::Vector2f(-PIN_RECT_SIZE / 2.0f, 0.0f);
		shapes[8 + inputPinCount * 4 + i * 4 + 1].position = pinCenter + sf::Vector2f(0.0f, PIN_RECT_SIZE / 2.0f);
		shapes[8 + inputPinCount * 4 + i * 4 + 2].position = pinCenter + sf::Vector2f(PIN_RECT_SIZE / 2.0f, 0.0f);
		shapes[8 + inputPinCount * 4 + i * 4 + 3].position = pinCenter + sf::Vector2f(0.0f, -PIN_RECT_SIZE / 2.0f);

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
		if (i < inputPinCount)
		{
			inputFields[i].draw(window);
			//	window.draw(interactiveBoxTexts[i]);
		}
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
		pinCenter.y = p.y + PROPERTY_HEIGHT * (i + 0.5f) + INPUT_FIELD_HEIGHT * i;
		if (uiMath::distance(pinCenter, mousePosInWorld) < PIN_COLLISION_DISTANCE)
		{
			pinPosition = pinCenter;
			returnPinColor = &(shapes[8 + i * 4].color);
			return i;
		}
	}

	pinCenter.x = p.x + NODE_WIDTH - PIN_RECT_MARGIN;
	direction = 0;
	for (int i = 0; i < outputPinCount; i++)
	{
		pinCenter.y = p.y + (PROPERTY_HEIGHT + INPUT_FIELD_HEIGHT) * inputPinCount + PROPERTY_HEIGHT * (i + 0.5f);
		if (uiMath::distance(pinCenter, mousePosInWorld) < PIN_COLLISION_DISTANCE)
		{
			pinPosition = pinCenter;
			returnPinColor = &(shapes[8 + inputPinCount * 4 + i * 4].color);
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

bool uiNode::onClickOverInputField(const sf::Vector2f& mousePosInWorld) // we return whether the mouse was or not over an input field
{
	for (int i = 0; i < inputPinCount; i++)
	{
		if (inputFields[i].onClick(mousePosInWorld)) // mouse was over input field
		{
			std::cout << "input field number " << i << std::endl;
			return true;
		}
	}
	return false;
}