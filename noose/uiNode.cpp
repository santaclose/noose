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

inline void* reserveDataForPin(uiNodeSystem::Types type)
{
	switch (type)
	{
	case uiNodeSystem::Types::Integer:
		return new int(0);
	case uiNodeSystem::Types::Float:
		return new float(0.0f);
	case uiNodeSystem::Types::Vector2i:
		return new sf::Vector2i(0, 0);
	case uiNodeSystem::Types::Color:
		return new sf::Color(255, 0, 255, 255);
	case uiNodeSystem::Types::Image:
		return new sf::RenderTexture();
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
	std::cout << "creating node " << data->nodeName << std::endl;
	nodeFunctionalityPointer = (void (*)(uiNode* theNode))(data->nodeFunctionality);
//	nodeFunctionalityPointer = nodeFunctionality;
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

	//pinDataPointers = (void**) malloc(sizeof(void*) * (inputPinCount + outputPinCount));
	pinDataPointers.reserve(inputPinCount + outputPinCount); // so it doesn't have to reallocate at each iteration
	receivedDataPointers.reserve(inputPinCount + outputPinCount);

	for (int i = 0; i < (inputPinCount + outputPinCount); i++)
	{
		pinDataPointers.push_back(reserveDataForPin(pinTypes[i]));
		receivedDataPointers.push_back(nullptr);
		// log
		std::cout << "memory allocated for an ";
		switch (pinTypes[i])
		{
		case uiNodeSystem::Types::Integer:
			std::cout << "int ";
			break;
		case uiNodeSystem::Types::Float:
			std::cout << "float ";
			break;
		case uiNodeSystem::Types::Vector2i:
			std::cout << "vec ";
			break;
		case uiNodeSystem::Types::Color:
			std::cout << "color ";
			break;
		case uiNodeSystem::Types::Image:
			std::cout << "image ";
			break;
		}
		std::cout << "at " << pinDataPointers.back() << std::endl;
		//reserveDataForPin(pinTypes[i], pinDataPointers[i]);
	}
	// log
	std::cout << "pinDataPointers has a size of " << pinDataPointers.size() << " after allocating\n";

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

		if (nodeAConnected == (void*)this)
			((uiNode*)nodeBConnected)->setLineIndexAsDisconnected(i);
		else
			((uiNode*)nodeAConnected)->setLineIndexAsDisconnected(i);

		std::cout << "uiNodeConnections removing line " << i << std::endl;
		uiNodeConnections::remove(i);
	}

	for (int i = 0; i < (inputPinCount + outputPinCount); i++)
	{
		delete pinDataPointers[i];
	}
	//free(pinDataPointers);
	pinDataPointers.clear();
	receivedDataPointers.clear();

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
	std::cout << "attaching line to node " << title.getString().toAnsiString() << std::endl;
	
	// attach line
	lineIndices.push_back(lineIndex);

	if (uiNodeConnections::lines[lineIndex].nodeA == (void*)this) // we are node a
	{
		if (uiNodeConnections::lines[lineIndex].pinA < inputPinCount) // ours is an input pin
		{
			receivedDataPointers[uiNodeConnections::lines[lineIndex].pinA] =
				((uiNode*)(uiNodeConnections::lines[lineIndex].nodeB))->getDataPointerForPin(uiNodeConnections::lines[lineIndex].pinB);
		}
	}
	else // we are node b
	{
		if (uiNodeConnections::lines[lineIndex].pinB < inputPinCount) // ours is an input pin
		{
			receivedDataPointers[uiNodeConnections::lines[lineIndex].pinB] =
				((uiNode*)(uiNodeConnections::lines[lineIndex].nodeA))->getDataPointerForPin(uiNodeConnections::lines[lineIndex].pinA);
		}
	}

	// overwrite pointer if is input pin
	/*std::cout << "line index: " << lineIndex << std::endl;
	std::cout << "line nodeA: " << uiNodeConnections::lines[lineIndex].nodeA <<
		"\nline nodeB: " << uiNodeConnections::lines[lineIndex].nodeB <<
		"\nline pinA: " << uiNodeConnections::lines[lineIndex].pinA <<
		"\nline pinB: " << uiNodeConnections::lines[lineIndex].pinB <<
		"\nthis: " << (void*)this << std::endl;
	if (uiNodeConnections::lines[lineIndex].nodeA == (void*)this) // we are node a
	{
		std::cout << "we are node a\n";
		if (uiNodeConnections::lines[lineIndex].pinA < inputPinCount) // ours is an input pin
		{
			std::cout << "changing pointer from " << pinDataPointers[uiNodeConnections::lines[lineIndex].pinA] <<
				"\n to " << ((uiNode*)uiNodeConnections::lines[lineIndex].nodeB)->getDataPointerForPin(uiNodeConnections::lines[lineIndex].pinB) << std::endl;
			std::cout << "pinDataPointers.size(): " << pinDataPointers.size() << std::endl;
			std::cout << "accessing element: " << uiNodeConnections::lines[lineIndex].pinA << std::endl;
			pinDataPointers[uiNodeConnections::lines[lineIndex].pinA] =
				((uiNode*)(uiNodeConnections::lines[lineIndex].nodeB))->getDataPointerForPin(uiNodeConnections::lines[lineIndex].pinB);
		}
		else
		{
			std::cout << "this line is not connected to an input pin of this node\n";
		}
	}
	else // we are node b
	{
		std::cout << "we are node b\n";
		if (uiNodeConnections::lines[lineIndex].pinB < inputPinCount) // ours is an input pin
		{
			std::cout << "changing pointer from " << pinDataPointers[uiNodeConnections::lines[lineIndex].pinB] <<
				"\n to " << ((uiNode*)uiNodeConnections::lines[lineIndex].nodeA)->getDataPointerForPin(uiNodeConnections::lines[lineIndex].pinA) << std::endl;
			std::cout << "pinDataPointers.size(): " << pinDataPointers.size() << std::endl;
			std::cout << "accessing element: " << uiNodeConnections::lines[lineIndex].pinB << std::endl;
			pinDataPointers[uiNodeConnections::lines[lineIndex].pinB] =
				((uiNode*)(uiNodeConnections::lines[lineIndex].nodeA))->getDataPointerForPin(uiNodeConnections::lines[lineIndex].pinA);
		}
		else
		{
			std::cout << "this line is not connected to an input pin of this node\n";
		}
	}*/
}

// remove all connections to this line ( it must be only one connection? ) *
void uiNode::setLineIndexAsDisconnected(int lineIndex)
{
	for (int i = 0; i < lineIndices.size(); i++)
	{
		if (lineIndices[i] == lineIndex)
		{
			/////+
			// get input field pointer back if is input field

			/*if (((uiNode*)(uiNodeConnections::lines[lineIndex].nodeA)) == this) // we are node a
			{
				if (uiNodeConnections::lines[lineIndex].pinA < inputPinCount) // ours is an input pin
				{
					std::cout << "changing pointer from " << pinDataPointers[uiNodeConnections::lines[lineIndex].pinA] <<
						"\n to " << inputFields[uiNodeConnections::lines[lineIndex].pinA].getDataPointer() << std::endl;

					pinDataPointers[uiNodeConnections::lines[lineIndex].pinA] =
						inputFields[uiNodeConnections::lines[lineIndex].pinA].getDataPointer();
				}
			}
			else // we are node b
			{
				if (uiNodeConnections::lines[lineIndex].pinB < inputPinCount) // ours is an input pin
				{
					std::cout << "changing pointer from " << pinDataPointers[uiNodeConnections::lines[lineIndex].pinB] <<
						"\n to " << inputFields[uiNodeConnections::lines[lineIndex].pinB].getDataPointer() << std::endl;

					pinDataPointers[uiNodeConnections::lines[lineIndex].pinB] =
						inputFields[uiNodeConnections::lines[lineIndex].pinB].getDataPointer();
				}
			}
			// detach line
			lineIndices.erase(lineIndices.begin() + i);
			break;*/
			/////+
			std::cout << "line " << lineIndices[i] << " being removed from node " << this << std::endl;
			lineIndices.erase(lineIndices.begin() + i);

			if (uiNodeConnections::lines[lineIndex].nodeA == (void*)this) // we are node a
			{
				if (uiNodeConnections::lines[lineIndex].pinA < inputPinCount) // ours is an input pin
				{
					receivedDataPointers[uiNodeConnections::lines[lineIndex].pinA] = nullptr;
				}
			}
			else // we are node b
			{
				if (uiNodeConnections::lines[lineIndex].pinB < inputPinCount) // ours is an input pin
				{
					receivedDataPointers[uiNodeConnections::lines[lineIndex].pinB] = nullptr;
				}
			}
			break;// i--; ---------
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


sf::RenderTexture* uiNode::getFirstInputImage()
{
	for (int i = 0; i < inputPinCount; i++)
	{
		if (pinTypes[i] == uiNodeSystem::Types::Image)
			return (sf::RenderTexture*)getDataPointerForPin(i);
	}
	return nullptr;
}

void* uiNode::getDataPointerForPin(int pinIndex)
{
	std::cout << "getDataPointers called for pin " << pinIndex << " in node " << title.getString().toAnsiString() << std::endl;
	//return pinDataPointers[pinIndex];

	if (receivedDataPointers[pinIndex] != nullptr)
		return receivedDataPointers[pinIndex];

	return pinDataPointers[pinIndex];
	// output pointers are always the same
	/*if (pinIndex >= inputPinCount)
	{
		return pinDataPointers[pinIndex];
	}
	else
	{
		// check if its connected and get the proper pointer
		for (int i : lineIndices)
		{
			if (uiNodeConnections::lines[i].pinA == pinIndex)
			{
				return ((uiNode*)(uiNodeConnections::lines[i].nodeB))->getDataPointerForPin(uiNodeConnections::lines[i].pinB);
			}
			else if (uiNodeConnections::lines[i].pinB == pinIndex)
			{
				return ((uiNode*)(uiNodeConnections::lines[i].nodeA))->getDataPointerForPin(uiNodeConnections::lines[i].pinA);
			}
		}
		return pinDataPointers[pinIndex];
	}*/
	// if there was no line connected to this pin we take the default pointer
}

void uiNode::activate()
{
	// execute node function
	nodeFunctionalityPointer(this);
}
