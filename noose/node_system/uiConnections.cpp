#include "uiConnections.h"
#include <iostream>
#include "../math/uiMath.h"
#include "../math/vectorOperators.h"

#define LINE_DELETE_COLLISION_DISTANCE 6.0f
#define CONNECTION_LINE_WIDTH 3.0
#define CONNECTION_LINE_HIDE_COLOR 0x00000000

std::vector<sf::Vertex> uiConnections::lineQuads;
std::vector<sf::Vector2f> uiConnections::linePoints;
sf::Shader uiConnections::shader;
bool uiConnections::startedOnInputPinb = false;

void uiConnections::updateLineQuads(int lineIndex)
{
	sf::Vector2f theRightVector = uiMath::normalized(uiMath::rightVector(linePoints[lineIndex * 2 + 1] - linePoints[lineIndex * 2 + 0]));

	int fvi = 4 * lineIndex; // first vertex index
	lineQuads[fvi + 1].position = linePoints[lineIndex * 2 + 1] + theRightVector * (CONNECTION_LINE_WIDTH / 2.0);
	lineQuads[fvi + 2].position = linePoints[lineIndex * 2 + 1] - theRightVector * (CONNECTION_LINE_WIDTH / 2.0);
	lineQuads[fvi + 3].position = linePoints[lineIndex * 2 + 0] - theRightVector * (CONNECTION_LINE_WIDTH / 2.0);
	lineQuads[fvi + 0].position = linePoints[lineIndex * 2 + 0] + theRightVector * (CONNECTION_LINE_WIDTH / 2.0);
}

void uiConnections::set(sf::Vector2f& pinPosA, const sf::Vector2f& pinPosB, const sf::Color& color, int index)
{
	int ri = index + 1; // real index

	if (ri >= linePoints.size() / 2) // need to allocate more space
	{
		lineQuads.resize(ri * 4 + 4);
		linePoints.resize(ri * 2 + 2);
		// set up uvs
		lineQuads[ri * 4 + 0].texCoords = sf::Vector2f(0.0, 0.0);
		lineQuads[ri * 4 + 1].texCoords = sf::Vector2f(1.0, 0.0);
		lineQuads[ri * 4 + 2].texCoords = sf::Vector2f(1.0, 1.0);
		lineQuads[ri * 4 + 3].texCoords = sf::Vector2f(0.0, 1.0);
		std::cout << "uvs set up\n";
	}

	lineQuads[ri * 4 + 0].color =
		lineQuads[ri * 4 + 1].color =
		lineQuads[ri * 4 + 2].color =
		lineQuads[ri * 4 + 3].color = color;

	linePoints[ri * 2 + 0] = pinPosA;
	linePoints[ri * 2 + 1] = pinPosB;

	updateLineQuads(ri);
}

void uiConnections::set(sf::Vector2f&& pinPosA, const sf::Vector2f& pinPosB, const sf::Color& color, int index)
{
	set(pinPosA, pinPosB, color, index);
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

	linePoints.resize(2);
}

void uiConnections::createTemporary(sf::Vector2f&& pinPos, const sf::Vector2f& mousePos, const sf::Color& color, bool startedOnInputPin)
{
	std::cout << "creating temporary line\n";
	set(pinPos, mousePos, color, -1);
	startedOnInputPinb = startedOnInputPin;
}

void uiConnections::displacePoint(const sf::Vector2f& displacement, int connectionIndex, bool isOutputPin)
{
	int ri = connectionIndex + 1; // real index
	if (isOutputPin)
		linePoints[ri * 2 + 0] += displacement;
	else
		linePoints[ri * 2 + 1] += displacement;
	updateLineQuads(ri);
}

void uiConnections::displaceTemporary(sf::Vector2f&& displacement)
{
	linePoints[1] += displacement;
	updateLineQuads(0);
}

void uiConnections::connect(sf::Vector2f&& pinPosB, int index)
{
	if (startedOnInputPinb) // flip if necessary
		set(pinPosB, linePoints[0], lineQuads[0].color, index);
	else
		set(linePoints[0], pinPosB, lineQuads[0].color, index);
	hideTemporary();
}

void uiConnections::hide(int index)
{
	int ri = index + 1; // real index

	// hide line quads by setting alpha to zero
	int fvi = 4 * ri; // first vertex index
	lineQuads[fvi].color =
		lineQuads[fvi + 1].color =
		lineQuads[fvi + 2].color =
		lineQuads[fvi + 3].color = sf::Color(CONNECTION_LINE_HIDE_COLOR);
}

void uiConnections::hideTemporary()
{
	hide(-1);
}

// line effect for antialiasing
void uiConnections::updateShaderUniform(float zoom)
{
	//shaderFadeRatio = 
	shader.setUniform("ratio", 1.0f - zoom / 3.0f); // looks good
}

void uiConnections::draw(sf::RenderWindow& window)
{
	window.draw(&lineQuads[0], lineQuads.size(), sf::Quads, &shader);
}

int uiConnections::onTryingToRemove(const sf::Vector2f& mousePos)
{
	for (int lineIndex = 1; lineIndex < linePoints.size() / 2; lineIndex++)
	{
		if (lineQuads[lineIndex * 4].color.a == 0.0)
			continue; // skip dead lines

		std::cout << "trying to remove line index: " << lineIndex << std::endl;
		if (uiMath::minimumDistance(linePoints[lineIndex * 2], linePoints[lineIndex * 2 + 1], mousePos) < LINE_DELETE_COLLISION_DISTANCE)
			return lineIndex-1;
	}
	return -1;
}
