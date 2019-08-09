#pragma once
#include <SFML/Graphics.hpp>
#include "uiNodeSystem.h"

class uiInputField
{
private:
	uiNodeSystem::Types type;
	sf::Vertex* shapes;
	sf::Text* texts;

	void updateTextPositions();
	void* dataPointer;
public:

	static void onMouseMoved(sf::Vector2f& displacement);
	static void onLeftClickReleased();
	static void onMouseScrolled(float delta);

	~uiInputField();
	void initialize(uiNodeSystem::Types theType, void* pinDataPointer);
	void setPosition(const sf::Vector2f& newPosition, float nodeWidth, float height); // top left corner position
	void draw(sf::RenderWindow& window);
	void setValue(const void* value);
	bool onClick(const sf::Vector2f& mousePosInWorld);
	void* getDataPointer(); // so the data pointer of the node can be changed when a connection is created
	//bool isMouseOver(const sf::Vector2f& mousePosInWorld, uiNodeSystem::Types& returnType, void*& returnPointer, sf::Text*& returnText);
};