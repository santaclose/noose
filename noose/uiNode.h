#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

#include "uiNodeSystem.h"
#include "uiNodeConnections.h"
#include "uiInputField.h"

class uiNode
{
private:
	std::vector<sf::Vertex> shapes; // top bar, content rect, and pins
	sf::Text title;
	int inputPinCount;
	int outputPinCount;
	float contentHeight;
	std::vector<int> lineIndices;

	sf::Text* pinNameTexts;

	uiNodeSystem::Types* pinTypes;

	uiInputField* inputFields;
	//void** pinDataPointers;
	std::vector<void*> pinDataPointers;
	std::vector<void*> receivedDataPointers;

	std::vector<uiNode*> propagationList;

	// function pointer with node functionality
	void (*nodeFunctionalityPointer)(uiNode* theNode);

	void addNodeToPropagationList(uiNode* theNode);
	void removeNodeFromPropagationList(uiNode* theNode);

public:
	uiNode(const void* nodeData, sf::Vector2f& initialPosition);
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

	bool onClickOverInputField(const sf::Vector2f& mousePosInWorld); // we return whether the mouse was or not over an input field
	//bool mouseOverInputField(const sf::Vector2f& mousePosInWorld, uiNodeSystem::Types& returnType, void*& returnPointer, sf::Text*& returnText);
	
	sf::RenderTexture* getFirstInputImage();

	void* getDataPointerForPin(int pinIndex, bool acceptReceivedPointers);
	void activate(); // executes node functionality and propagates to right hand side nodes
};

