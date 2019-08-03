#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "uiLine.h"

namespace uiNodeConnections
{
	extern std::vector<uiLine> lines;

	void init();
	void updateLineQuads(int lineIndex);
	void push(const sf::Vector2f& pinPosition, const sf::Vector2f& mouseWorldPos, sf::Color* pinColor, void* theNodeA, int thePinA, int theDirection);
	bool tryToEstablish(const sf::Vector2f& pinPosition, sf::Color* pinColor, void* theNodeB, int thePinB, int theDirection, void*& nodeA);
	void removeLast();
	void remove(int lineIndex);
	void updateShaderUniform(float zoom);
	void draw(sf::RenderWindow& window);
	void moveLastPoint(sf::Vector2f&& displacement);
	void movePoint(int lineIndex, void* nodeWhereThePointIs, const sf::Vector2f& displacement);
	void getConnectedNodes(int lineIndex, void*& nodeA, void*& nodeB);

	int getLastLineIndex();
	void* getLastLineNodeA();
	int getLastLinePinA();

	int onTryingToRemove(sf::Vector2f& mouseWorldPos, void*& nodeA, void*& nodeB);

	void printAllConnections();
};