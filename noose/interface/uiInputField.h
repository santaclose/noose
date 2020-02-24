#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class uiInputField
{
	friend void onColorPickerSetColor(sf::Color* newColor);

private:
	bool enabled = true;
	int type;
	sf::Vertex* shapes = nullptr;
	sf::Text* texts = nullptr;
	const std::vector<std::string>* enumOptions;

	void updateTextPositions();
	void* dataPointer;
	void (*onValueChanged)() = nullptr;

public:

	static void onMouseMoved(sf::Vector2f& displacement);
	static void unbind();

	~uiInputField();
	bool mouseOver(const sf::Vector2f& mousePosInWorld, int& index);

	void create(int theType, void* pinDataPointer, void(onValueChangedFunc)(), const std::vector<std::string>* enumOptions);
	void setPosition(const sf::Vector2f& newPosition, float nodeWidth, float height); // top left corner position
	void draw(sf::RenderWindow& window);
	void setValue(const void* value);

	// the index tells which of the two components of a vector is gonna change
	void bind(int index);

	void* getDataPointer(); // so the data pointer of the node can be changed when a connection is created

	void disable();
	void enable();
	const bool& isEnabled();
};