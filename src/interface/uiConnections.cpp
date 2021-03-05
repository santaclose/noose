#include "uiConnections.h"
#include <iostream>
#include <vector>
#include "../math/uiMath.h"
#include "../math/vectorOperators.h"
#include "../utils.h"

#define LINE_DELETE_COLLISION_DISTANCE 6.0f
#define CONNECTION_LINE_WIDTH 3.0
#define CONNECTION_LINE_HIDE_COLOR 0x00000000

namespace uiConnections {

	std::vector<sf::Vertex> lineQuads;
	std::vector<uiLineInfo> linesInfo;
	sf::Shader shader;
	bool startedOnRightSideNodeB;

	void updateLineQuads(int lineIndex);
	void set(const sf::Vector2f& pinPosA, const sf::Vector2f& pinPosB, int nodeA, int nodeB, int pinA, int pinB, const sf::Color& color, int index);
	int findSlot();
	void printArray();
}

void uiConnections::updateLineQuads(int lineIndex)
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

void uiConnections::set(const sf::Vector2f& pinPosA, const sf::Vector2f& pinPosB, int nodeA, int nodeB, int pinA, int pinB, const sf::Color& color, int index)
{
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

int uiConnections::findSlot()
{
	int i = 1;
	while (true)
	{
		if (i == linesInfo.size())
		{
			lineQuads.resize(i * 4 + 4);
			linesInfo.resize(i + 1);
			// set up uvs
			lineQuads[i * 4 + 0].texCoords = sf::Vector2f(0.0, 0.0);
			lineQuads[i * 4 + 1].texCoords = sf::Vector2f(1.0, 0.0);
			lineQuads[i * 4 + 2].texCoords = sf::Vector2f(1.0, 1.0);
			lineQuads[i * 4 + 3].texCoords = sf::Vector2f(0.0, 1.0);
			break;
		}
		if (linesInfo[i].hidden)
		{
			linesInfo[i].hidden = false;
			break;
		}
		i++;
	}
	//for (; i < linesInfo.size() && !linesInfo[i].hidden; i++) {}
	//std::cout << "[UI] found index " << i << std::endl;
	//std::cout << "[UI] vector size " << linesInfo.size() << std::endl;
	return i;
}
void uiConnections::printArray()
{
	for (int i = 0; i < linesInfo.size(); i++)
	{
		std::cout << linesInfo[i].hidden;
	}
	std::cout << std::endl;
}

void uiConnections::initialize(float zoom)
{
	if (!shader.loadFromFile(utils::getProgramDirectory() + "assets/shaders/connection.shader", sf::Shader::Fragment))
		std::cout << "[UI] Failed to load line shader\n";
	//shader.setUniform("ratio", 0.6f);//shaderFadeRatio);
	shader.setUniform("zoom", zoom);

	lineQuads.resize(4);
	// set up uvs
	lineQuads[0].texCoords = sf::Vector2f(0.0, 0.0);
	lineQuads[1].texCoords = sf::Vector2f(1.0, 0.0);
	lineQuads[2].texCoords = sf::Vector2f(1.0, 1.0);
	lineQuads[3].texCoords = sf::Vector2f(0.0, 1.0);

	linesInfo.resize(1);
}

void uiConnections::createTemporary(const sf::Vector2f& pinPos, const sf::Vector2f& mousePos, const sf::Color& color, int node, int pin, bool startedOnRightSideNode)
{
	//std::cout << "creating temporary line\n";
	startedOnRightSideNodeB = startedOnRightSideNode;
	if (!startedOnRightSideNodeB)
		set(pinPos, mousePos, node, -1, pin, -1, color, 0);
	else
		set(mousePos, pinPos, -1, node, -1, pin, color, 0);
}

void uiConnections::displacePoint(const sf::Vector2f& displacement, int connectionIndex, bool isOutputPin)
{
	//std::cout << "displacing point in line " << connectionIndex << std::endl;
	int ri = connectionIndex + 1; // real index
	if (ri >= linesInfo.size())
		std::cout <<"[UI] INVALID LINE INDEX WHEN DISPLACING POINT\n";

	if (isOutputPin)
		linesInfo[ri].posA += displacement;
	else
		linesInfo[ri].posB += displacement;
	updateLineQuads(ri);
}

void uiConnections::displaceTemporary(const sf::Vector2f& displacement)
{
	if (!startedOnRightSideNodeB)
		linesInfo[0].posB += displacement;
	else
		linesInfo[0].posA += displacement;
	updateLineQuads(0);
}

int uiConnections::connect(const sf::Vector2f& pinPos, int node, int pin)
{
	//printArray();
	int index = findSlot();

	//std::cout << "slot found: " << index << std::endl;
	// create new line starting from temporary
	if (startedOnRightSideNodeB) // flip if necessary
		set(pinPos, linesInfo[0].posB, node, linesInfo[0].nodeB, pin, linesInfo[0].pinB, lineQuads[0].color, index);
	else
		set(linesInfo[0].posA, pinPos, linesInfo[0].nodeA, node, linesInfo[0].pinA, pin, lineQuads[0].color, index);
	hideTemporary();
	return index - 1;
}

int uiConnections::connect(const sf::Vector2f& leftPinPos, const sf::Vector2f& rightPinPos, int leftNode, int rightNode, int leftPin, int rightPin, const sf::Color& color)
{
	int index = findSlot();
	set(leftPinPos, rightPinPos, leftNode, rightNode, leftPin, rightPin, color, index);
	return index - 1;
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
	//shader.setUniform("ratio", 1.0f - zoom / 3.0f); // looks good
	shader.setUniform("zoom", zoom);
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

void uiConnections::getLines(int& count, const uiLineInfo*& lineArray)
{
	// first line is temporary line
	count = linesInfo.size() - 1;
	if (count > 0)
		lineArray = &(linesInfo[1]);
}

void uiConnections::clearEverything()
{
	linesInfo.clear();
	lineQuads.clear();

	lineQuads.resize(4);
	// set up uvs
	lineQuads[0].texCoords = sf::Vector2f(0.0, 0.0);
	lineQuads[1].texCoords = sf::Vector2f(1.0, 0.0);
	lineQuads[2].texCoords = sf::Vector2f(1.0, 1.0);
	lineQuads[3].texCoords = sf::Vector2f(0.0, 1.0);

	linesInfo.resize(1);
}
