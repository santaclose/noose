#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

#include "uiNodeSystem.h"
#include "uiNodeConnections.h"

class uiNode
{
private:
	std::vector<sf::Vertex> shapes; // top bar, content rect, interactive boxes and pins
	//sf::VertexArray shapes; 
	sf::Text title;
	int inputPinCount;
	int outputPinCount;
	float contentHeight;
	//int* lineIndices; // one index for each pin. for moving lines on drag
	std::vector<int> lineIndices;

	sf::Text* pinNameTexts;

public:
	uiNode(const std::string& name, int numberOfInputPins, int numberOfOutputPins, const uiNodeSystem::Types* pinTypes, const std::string* pinNames, sf::Vector2f* initialPosition = nullptr);
	//uiNode(const uiNode &other);
	~uiNode();
	void setPosition(sf::Vector2f& newPosition);
	void draw(sf::RenderWindow& window);
	bool mouseOverNode(const sf::Vector2f& mousePos);
	bool mouseOverTopBar(const sf::Vector2f& mousePos);
	int mouseOverPin(const sf::Vector2f& mousePos, sf::Color*& returnPinColor, sf::Vector2f& pinPosition, int& direction);
	void paintAsSelected();
	void paintAsUnselected();
	void move(const sf::Vector2f& displacement);
	void attachConnectionPoint(int lineIndex);
	void setLineIndexAsDisconnected(int lineIndex);
	bool isInputAndAlreadyConnected(int pin);

	void print();
};

