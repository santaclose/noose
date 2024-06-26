#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "uiSelectionBox.h"
#include "uiInputField.h"
#include "../nodeData.h"
#include "../serializer.h"

class uiNode
{
	friend serializer;// this is too hacky
private:
	const nodeData* m_nodeData;

	std::vector<sf::Vertex> m_shapes; // top bar, content rect, and pins
	sf::Text* m_title;
	float m_contentHeight;

	std::vector<sf::Text> m_pinNameTexts;
	uiInputField* m_inputFields;
	
	std::vector<int> m_connectedLineIndices;
	std::vector<int> m_connectedLinePins;

public:
	enum MousePos {
		TopBar, Pin, InputField, OtherInside, Outside
	};

	uiNode(
		const nodeData* data,
		bool nodeCenterInPosition,
		sf::Vector2f& initialPosition,
		const std::vector<void*>& inputFieldPointers,
		void(onValueChangedFunc)(int),
		uiSelectionBox* selectionBox);
	~uiNode();

	void setInput(int inputIndex, const void* data, int flags = 0);
	const void* getInput(int index, int* outFlags = nullptr);
	inline const void* getDataPointer(int index) { return m_inputFields[index].getDataPointer(); }

	inline const std::string& getName() const { return m_nodeData->nodeName; }

	inline int getInputPinCount() const { return m_nodeData->inputPinCount; }
	inline int getOutputPinCount() const { return m_nodeData->outputPinCount; }

	void setPosition(sf::Vector2f& newPosition);
	const sf::Vector2f& getPosition() const;
	inline sf::Vector2f getCenterPosition() const { return (m_shapes[3].position + m_shapes[5].position) / 2.0f; }
	sf::Vector2f getPinPosition(int index);
	const sf::Color& getPinColor(int index);
	void draw(sf::RenderWindow& window);

	MousePos mouseOver(const sf::Vector2f& mousePos, int& index, int& subIndex);
	bool mouseOverTopBar(const sf::Vector2f& mousePos);

	void paintAsSelected();
	void paintAsUnselected();
	void displace(const sf::Vector2f& displacement);
	void attachConnectionPoint(int lineIndex, int pin);
	void setLineIndexAsDisconnected(int lineIndex);

	bool canConnectToPin(int pin);

	void bindInputField(int index, int subIndex, uiInputField::InteractionMode interactionMode = uiInputField::InteractionMode::Default);

	// needed to disconnect before deleting the node 
	std::vector<int> getConnectedLines();
};