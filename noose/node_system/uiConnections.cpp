#include "uiConnections.h"
#include <iostream>
#include "../math/uiMath.h"
#include "../math/vectorOperators.h"

#define LINE_DELETE_COLLISION_DISTANCE 6.0f
#define CONNECTION_LINE_WIDTH 3.0
#define CONNECTION_LINE_HIDE_COLOR 0x00000000

std::vector<sf::Vertex> lineQuads;
std::vector<uiLineInfo> linesInfo;
sf::Shader shader;
bool startedOnInputPinb = false;

void updateLineQuads(int lineIndex)
{
	/*std::cout << "updating line quads for " << lineIndex << std::endl;
	std::cout << linesInfo[lineIndex].posA.x << ", " << linesInfo[lineIndex].posA.y << std::endl;
	std::cout << linesInfo[lineIndex].posB.x << ", " << linesInfo[lineIndex].posB.y << std::endl;*/

	sf::Vector2f theRightVector = uiMath::normalized(uiMath::rightVector(linesInfo[lineIndex].posB - linesInfo[lineIndex].posA));

	int fvi = 4 * lineIndex; // first vertex index
	lineQuads[fvi + 1].position = linesInfo[lineIndex].posB + theRightVector * (CONNECTION_LINE_WIDTH / 2.0);
	lineQuads[fvi + 2].position = linesInfo[lineIndex].posB - theRightVector * (CONNECTION_LINE_WIDTH / 2.0);
	lineQuads[fvi + 3].position = linesInfo[lineIndex].posA - theRightVector * (CONNECTION_LINE_WIDTH / 2.0);
	lineQuads[fvi + 0].position = linesInfo[lineIndex].posA + theRightVector * (CONNECTION_LINE_WIDTH / 2.0);
}

// weird bug when passing const reference
//void set(const sf::Vector2f& pinPosA, const sf::Vector2f& pinPosB, const sf::Color& color, int index, int nodeA, int nodeB, int pinA, int pinB)
void set(sf::Vector2f pinPosA, sf::Vector2f pinPosB, sf::Color color, int index, int nodeA, int nodeB, int pinA, int pinB)
{
	/*if (index > linesInfo.size())
		std::cout << "NOT POSSIBLE\n";*/

	if (index == linesInfo.size()) // need to allocate more space
	{
		lineQuads.resize(index * 4 + 4);
		std::cout << "--------after lineQuads resize " << index << std::endl;
		std::cout << pinPosA.x << ", " << pinPosA.y << std::endl;
		std::cout << pinPosB.x << ", " << pinPosB.y << std::endl;
		linesInfo.resize(index + 1);
		std::cout << "--------after linesInfo resize " << index << std::endl;
		std::cout << pinPosA.x << ", " << pinPosA.y << std::endl;
		std::cout << pinPosB.x << ", " << pinPosB.y << std::endl;

		// set up uvs
		lineQuads[index * 4 + 0].texCoords = sf::Vector2f(0.0, 0.0);
		lineQuads[index * 4 + 1].texCoords = sf::Vector2f(1.0, 0.0);
		lineQuads[index * 4 + 2].texCoords = sf::Vector2f(1.0, 1.0);
		lineQuads[index * 4 + 3].texCoords = sf::Vector2f(0.0, 1.0);
		//std::cout << "uvs set up\n";
	}

	lineQuads[index * 4 + 0].color =
		lineQuads[index * 4 + 1].color =
		lineQuads[index * 4 + 2].color =
		lineQuads[index * 4 + 3].color = color;

	linesInfo[index].posA = pinPosA;
	linesInfo[index].posB = pinPosB;
	linesInfo[index].nodeA = nodeA;
	linesInfo[index].nodeB = nodeB;
	linesInfo[index].pinA = pinA;
	linesInfo[index].pinB = pinB;

	updateLineQuads(index);
}

int findSlot()
{
	int i = 1;
	for (; i < linesInfo.size() && !linesInfo[i].hidden; i++) {}
	return i;
}
void printArray()
{
	for (int i = 0; i < linesInfo.size(); i++)
	{
		std::cout << linesInfo[i].hidden;
	}
	std::cout << std::endl;
}

void uiConnections::initialize()
{
	shader.loadFromFile("res/shaders/connection.shader", sf::Shader::Fragment);
	shader.setUniform("ratio", 0.6f);//shaderFadeRatio);

	lineQuads.resize(4);
	// set up uvs
	lineQuads[0].texCoords = sf::Vector2f(0.0, 0.0);
	lineQuads[1].texCoords = sf::Vector2f(1.0, 0.0);
	lineQuads[2].texCoords = sf::Vector2f(1.0, 1.0);
	lineQuads[3].texCoords = sf::Vector2f(0.0, 1.0);

	linesInfo.resize(1);
}

void uiConnections::createTemporary(const sf::Vector2f& pinPos, const sf::Vector2f& mousePos, const sf::Color& color, bool startedOnInputPin, int node, int pin)
{
	//std::cout << "creating temporary line\n";
	set(pinPos, mousePos, color, 0, node, -1, pin, -1);
	startedOnInputPinb = startedOnInputPin;
}

void uiConnections::displacePoint(const sf::Vector2f& displacement, int connectionIndex, bool isOutputPin)
{
	//std::cout << "displacing point in line " << connectionIndex << std::endl;
	int ri = connectionIndex + 1; // real index
	if (isOutputPin)
		linesInfo[ri].posA += displacement;
	else
		linesInfo[ri].posB += displacement;
	updateLineQuads(ri);
}

void uiConnections::displaceTemporary(const sf::Vector2f& displacement)
{
	linesInfo[0].posB += displacement;
	updateLineQuads(0);
}

int uiConnections::connect(const sf::Vector2f& pinPos, int node, int pin)
{
	printArray();
	int index = findSlot();
	
	// mark as not hidden if hidden
	linesInfo[index].hidden = false;

	std::cout << "slot found: " << index << std::endl;
	if (startedOnInputPinb) // flip if necessary
		set(pinPos, linesInfo[0].posA, lineQuads[0].color, index, node, linesInfo[0].nodeA, pin, linesInfo[0].pinA);
	else
		set(linesInfo[0].posA, pinPos, lineQuads[0].color, index, linesInfo[0].nodeA, node, linesInfo[0].pinA, pin);
	hideTemporary();
	return index-1;
}

void uiConnections::hide(int index)
{
	int ri = index + 1; // real index
	//std::cout << "hiding at " << ri << std::endl;

	// hide line quads by setting alpha to zero
	int fvi = 4 * ri; // first vertex index
	lineQuads[fvi].color =
		lineQuads[fvi + 1].color =
		lineQuads[fvi + 2].color =
		lineQuads[fvi + 3].color = sf::Color(CONNECTION_LINE_HIDE_COLOR);
	linesInfo[ri].hidden = true;
}

void uiConnections::hideTemporary()
{
	hide(-1);
}

// line effect for antialiasing
void uiConnections::updateShaderUniform(float zoom)
{
	shader.setUniform("ratio", 1.0f - zoom / 3.0f); // looks good
}

void uiConnections::draw(sf::RenderWindow& window)
{
	window.draw(&lineQuads[0], lineQuads.size(), sf::Quads, &shader);
}

void uiConnections::getNodesForLine(int line, int& nodeA, int& nodeB)
{
	nodeA = linesInfo[line + 1].nodeA;
	nodeB = linesInfo[line + 1].nodeB;
}

void uiConnections::getPinsForLine(int line, int& pinA, int& pinB)
{
	pinA = linesInfo[line + 1].pinA;
	pinB = linesInfo[line + 1].pinB;
}

int uiConnections::onTryingToRemove(const sf::Vector2f& mousePos)
{
	for (int lineIndex = 1; lineIndex < linesInfo.size(); lineIndex++)
	{
		if (lineQuads[lineIndex * 4].color.a == 0.0)
			continue; // skip dead lines

		//std::cout << "trying to remove line index: " << lineIndex << std::endl;
		if (uiMath::minimumDistance(linesInfo[lineIndex].posA, linesInfo[lineIndex].posB, mousePos) < LINE_DELETE_COLLISION_DISTANCE)
			return lineIndex-1;
	}
	return -1;
}
