#include "uiNodeConnections.h"
#include "uiMath.h"
#include <iostream>

#define LINE_DELETE_COLLISION_DISTANCE 6.0f

#define CONNECTION_LINE_WIDTH 3.0

#define CONNECTION_LINE_HIDE_COLOR 0x00000000

namespace uiNodeConnections {

	std::vector<sf::Vertex> lineQuads;
	//sf::VertexArray lineQuads;
	std::vector<uiLine> lines;
	sf::Shader shader;

	int lastPushedLineIndex;

	inline int getSlotForLine()
	{
		// log
		/*for (uiLine& l : lines)
		{
			std::cout << l << std::endl;
		}*/


		// find slot
		for (int i = 0; i < lines.size(); i++)
		{
			if (lines[i].available)
			{
				return i;
			}
		}
		return -1;
	}
}

void uiNodeConnections::init()
{
	shader.loadFromFile("res/shaders/connection.shader", sf::Shader::Fragment);
	shader.setUniform("ratio", 0.6f);//shaderFadeRatio);
}

// add a new line
void uiNodeConnections::push(const sf::Vector2f& pinPosition, const sf::Vector2f& mouseWorldPos, sf::Color* pinColor, void* theNodeA, int thePinA, int theDirection)
{
	std::cout << "pushing line\n";
	//lines.push_back(uiLine(pinPosition, theNodeA, thePinA, theDirection));
	int slot = getSlotForLine();
	if (slot > -1) // slot found
	{
		std::cout << "line slot found\n";
		// line data
		lines[slot] = uiLine(pinPosition, theNodeA, thePinA, theDirection);

		// line quad
		int fvi = 4 * slot; // first vertex index
		lineQuads[fvi].color = lineQuads[fvi + 1].color = lineQuads[fvi + 2].color = lineQuads[fvi + 3].color = *pinColor;

		lastPushedLineIndex = slot;
	}
	else // slot not found, push new
	{
		std::cout << "line slot NOT found\n";
		// line data
		lines.emplace_back(pinPosition, theNodeA, thePinA, theDirection);

		// line quad
		sf::Vertex newVertex;
		newVertex.color = *pinColor;
		newVertex.texCoords = sf::Vector2f(0.0, 0.0);
		lineQuads.push_back(newVertex);
		newVertex.texCoords = sf::Vector2f(1.0, 0.0);
		lineQuads.push_back(newVertex);
		newVertex.texCoords = sf::Vector2f(1.0, 1.0);
		lineQuads.push_back(newVertex);
		newVertex.texCoords = sf::Vector2f(0.0, 1.0);
		lineQuads.push_back(newVertex);

		lastPushedLineIndex = lines.size() - 1;
	}

	moveLastPoint(mouseWorldPos - pinPosition);
}

// do some check and keep the line
bool uiNodeConnections::tryToEstablish(const sf::Vector2f& pinPosition, sf::Color* pinColor, void* theNodeB, int thePinB, int theDirection, void*& nodeA)
{
	if (theNodeB == lines[lastPushedLineIndex].nodeA || // both pins are in the same node
		theDirection == lines[lastPushedLineIndex].direction || // both are output or input
		*pinColor != lineQuads[lastPushedLineIndex * 4].color) // not the same type (compare colors)
	{
		std::cout << "not connecting:\n";
		std::cout << "NodeA: " << lines[lastPushedLineIndex].nodeA << std::endl;
		std::cout << "NodeB: " << theNodeB << std::endl;
		std::cout << "Directions: " << theDirection << " " << lines[lastPushedLineIndex].direction << std::endl;
		std::cout << "Colors: " << pinColor->r << ", " << pinColor->g << ", " << pinColor->b << ", " << pinColor->a << " " << lineQuads[lastPushedLineIndex * 4].color.r << ", " << lineQuads[lastPushedLineIndex * 4].color.g << ", " << lineQuads[lastPushedLineIndex * 4].color.b << ", " << lineQuads[lastPushedLineIndex * 4].color.a;
		std::cout << std::endl;
		removeLast();
		return false;
	}

	lines[lastPushedLineIndex].nodeB = theNodeB;
	lines[lastPushedLineIndex].pinB = thePinB;
	lines[lastPushedLineIndex].posB = pinPosition;
	updateLineQuads(lastPushedLineIndex);

	// return nodes (system already has node B so we only pass nodeA)
	nodeA = lines[lastPushedLineIndex].nodeA;
	//nodeB = lines.back().nodeB;
	return true;
}

void uiNodeConnections::removeLast()
{
	remove(lastPushedLineIndex);
}

void uiNodeConnections::remove(int lineIndex)
{
	std::cout << "marking line " << lineIndex << "as available\n";
	// mark line as available
	lines[lineIndex].available = true;

	// hide line quads by setting alpha to zero
	int fvi = 4 * lineIndex; // first vertex index
	lineQuads[fvi].color = lineQuads[fvi + 1].color = lineQuads[fvi + 2].color = lineQuads[fvi + 3].color = sf::Color(CONNECTION_LINE_HIDE_COLOR);
}

// line effect for antialiasing
void uiNodeConnections::updateShaderUniform(float zoom)
{
	//shaderFadeRatio = 
	shader.setUniform("ratio", 1.0f - zoom / 3.0f); // looks good
}

void uiNodeConnections::draw(sf::RenderWindow& window)
{
	window.draw(&lineQuads[0], lineQuads.size(), sf::Quads, &shader);
}

// update quad after changing the line
void uiNodeConnections::updateLineQuads(int lineIndex)
{
	sf::Vector2f theRightVector = uiMath::normalized(uiMath::rightVector(lines[lineIndex].posB - lines[lineIndex].posA));

	int fvi = 4 * lineIndex; // first vertex index
	lineQuads[fvi + 1].position = lines[lineIndex].posB + theRightVector * (CONNECTION_LINE_WIDTH / 2.0);
	lineQuads[fvi + 2].position = lines[lineIndex].posB - theRightVector * (CONNECTION_LINE_WIDTH / 2.0);
	lineQuads[fvi + 3].position = lines[lineIndex].posA - theRightVector * (CONNECTION_LINE_WIDTH / 2.0);
	lineQuads[fvi + 0].position = lines[lineIndex].posA + theRightVector * (CONNECTION_LINE_WIDTH / 2.0);
}

// last may not be the one at the end of the list
void uiNodeConnections::moveLastPoint(sf::Vector2f&& displacement)
{
	lines[lastPushedLineIndex].posB += displacement;
	updateLineQuads(lastPushedLineIndex);
}

// access the line, check what point has to be moved and move it
void uiNodeConnections::movePoint(int lineIndex, void* nodeWhereThePointIs, const sf::Vector2f& displacement)
{
	if (lines[lineIndex].nodeA == nodeWhereThePointIs)
	{
		lines[lineIndex].posA += displacement;
	}
	else
	{
		lines[lineIndex].posB += displacement;
	}
	updateLineQuads(lineIndex);
}
void uiNodeConnections::getConnectedNodes(int lineIndex, void*& nodeA, void*& nodeB)
{
	nodeA = lines[lineIndex].nodeA;
	nodeB = lines[lineIndex].nodeB;
}


// last line is not always lines[lines.size() - 1]
int uiNodeConnections::getLastLineIndex()
{
	return lastPushedLineIndex;
}

void* uiNodeConnections::getLastLineNodeA()
{
	return lines[lastPushedLineIndex].nodeA;
}

int uiNodeConnections::getLastLinePinA()
{
	return lines[lastPushedLineIndex].pinA;
}

// system needs to retrieve the two nodes to detach the line
int uiNodeConnections::onTryingToRemove(sf::Vector2f& mouseWorldPos, void*& nodeA, void*& nodeB)
{
	int lineIndex = 0;
	for (uiLine& l : lines)
	{
		/*sf::Vector2f newMousePos = lastMouseWorldPos + displacement;

		//std::cout << "checking intersection:\n\tmouse: " << lastMouseWorldPos.x << ", " << lastMouseWorldPos.y << "   " << newMousePos.x << ", " << newMousePos.y << std::endl;
		//std::cout << "\tline: " << l.posA.x << ", " << l.posA.y << "   " << l.posB.x << ", " << l.posB.y << std::endl;
		if (uiMath::linesIntersect(lastMouseWorldPos, newMousePos, l.posA, l.posB))
		{
			std::cout << "intersection detected\n";
		}*/


		//std::cout << "iterating at line " << lineIndex << std::endl;
		if (l.available)
		{
			lineIndex++;
			continue; // skip dead lines
		}

		if (uiMath::minimumDistance(l.posA, l.posB, mouseWorldPos) < LINE_DELETE_COLLISION_DISTANCE)
		{
			remove(lineIndex);
			nodeA = l.nodeA;
			nodeB = l.nodeB;
			return lineIndex;
		}
		lineIndex++;
	}
	return -1;
}

void uiNodeConnections::printAllConnections()
{
	for (uiLine& l : lines)
	{
		if (l.available)
			continue;
		std::cout << '\t' << l.nodeA << '.' << l.pinA << " -> " << l.nodeB << '.' << l.pinB << std::endl;
	}
}