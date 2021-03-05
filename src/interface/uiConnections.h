#pragma once
#include <SFML/Graphics.hpp>

struct uiLineInfo
{
	sf::Vector2f posA;
	sf::Vector2f posB;
	int nodeA;
	int nodeB;
	int pinA;
	int pinB;
	bool hidden = false;
};

namespace uiConnections
{
	void initialize(float zoom);

	void createTemporary(const sf::Vector2f& pinPos, const sf::Vector2f& mousePos, const sf::Color& color, int node, int pin, bool startedOnRightSideNode);

	void displacePoint(const sf::Vector2f& displacement, int connectionIndex, bool isOutputPin);
	void displaceTemporary(const sf::Vector2f& displacement);

	int connect(const sf::Vector2f& pinPos, int node, int pin);
	int connect(
		const sf::Vector2f& leftPinPos,
		const sf::Vector2f& rightPinPos,
		int leftNode,
		int rightNode,
		int leftPin,
		int rightPin,
		const sf::Color& color
	);

	void hide(int index);
	void hideTemporary();

	void updateShaderUniform(float zoom);
	void draw(sf::RenderWindow& window);

	void getNodesForLine(int line, int& nodeA, int& nodeB);
	void getPinsForLine(int line, int& pinA, int& pinB);

	int onTryingToRemove(const sf::Vector2f& mousePos);

	void getLines(int& count, const uiLineInfo*& lineArray);

	void clearEverything();
};