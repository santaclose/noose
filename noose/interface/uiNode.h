#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "uiSelectionBox.h"
#include "../interface/uiInputField.h"
#include "../nodeData.h"


class uiNode
{
private:
	int m_inputPinCount;
	int m_outputPinCount;

	std::vector<sf::Vertex> m_shapes; // top bar, content rect, and pins
	sf::Text m_title;
	float m_contentHeight;

	sf::Text* m_pinNameTexts;
	uiInputField* m_inputFields;
	
	std::vector<int> m_connectedLineIndices;
	std::vector<int> m_connectedLinePins;

public:
	enum MousePos {
		TopBar, Pin, InputField, OtherInside, Outside
	};

	uiNode(const nodeData* data, sf::Vector2f& initialPosition, const std::vector<void*>& inputFieldPointers, void(onValueChangedFunc)(), uiSelectionBox* selectionBox);
	~uiNode();

	void setPosition(sf::Vector2f& newPosition);
	void draw(sf::RenderWindow& window);

	MousePos mouseOver(const sf::Vector2f& mousePos, int& index, int& subIndex);
	bool mouseOverTopBar(const sf::Vector2f& mousePos);

	void paintAsSelected();
	void paintAsUnselected();
	void displace(const sf::Vector2f& displacement);
	void attachConnectionPoint(int lineIndex, int pin);
	void setLineIndexAsDisconnected(int lineIndex);

	bool canConnectToPin(int pin);

	void bindInputField(int index, int subIndex);

	sf::Vector2f getPinPosition(int index);
	const sf::Color& getPinColor(int index);
	int getInputPinCount();

	// needed to disconnect before deleting the node 
	std::vector<int> getConnectedLines();
};