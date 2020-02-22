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
/*private:
	static std::vector<sf::Vertex> lineQuads;
	static std::vector<uiLineInfo> linesInfo;
	static sf::Shader shader;

	static bool startedOnInputPinb;

	static sf::CircleShape debugCircleB;
	static sf::CircleShape debugCircleG;

private:
	static void printArray();
	static void updateLineQuads(int lineIndex);
	static void set(const sf::Vector2f& pinPosA, const sf::Vector2f& pinPosB, const sf::Color& color, int index, int nodeA, int nodeB, int pinA, int pinB);
	//static void set(sf::Vector2f pinPosA, sf::Vector2f pinPosB, const sf::Color& color, int index, int nodeA, int nodeB, int pinA, int pinB);
	static int findSlot();

public:*/
	void initialize();

	void createTemporary(const sf::Vector2f& pinPos, const sf::Vector2f& mousePos, const sf::Color& color, bool startedOnInputPin, int node, int pin);

	void displacePoint(const sf::Vector2f& displacement, int connectionIndex, bool isOutputPin);
	void displaceTemporary(const sf::Vector2f& displacement);

	int connect(const sf::Vector2f& pinPos, int node, int pin);

	void hide(int index);
	void hideTemporary();

	void updateShaderUniform(float zoom);
	void draw(sf::RenderWindow& window);

	void getNodesForLine(int line, int& nodeA, int& nodeB);
	void getPinsForLine(int line, int& pinA, int& pinB);

	int onTryingToRemove(const sf::Vector2f& mousePos);
};