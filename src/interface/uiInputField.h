#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "uiSelectionBox.h"
#include "../serializer.h"

class uiInputField
{
	friend void onColorPickerSetColor(sf::Color* newColor);
	friend serializer;// this is too hacky

public:
	enum class ImageFieldContent { None = 0, FromFile = 1, FromMemory = 2 };
	enum InteractionMode { Default = 0, Typing = 1, PickPosition = 2 };

	static void onMouseMoved(sf::Vector2f& displacement);
	// returns true if there was a collision
	static bool onMouseDown(sf::Vector2f& mousePos);
	static void onMouseUp();
	static bool keyboardInput(std::uint32_t unicode);
	static void unbind();
	static bool isBound();
	static bool typingInteractionOngoing();
	static void setVectorData(const sf::Vector2i& vec);

	~uiInputField();
	bool mouseOver(const sf::Vector2f& mousePosInWorld, int& index);
	void create(int thePin, int theType, void* pinDataPointer, void(onValueChangedFunc)(int), const std::vector<std::string>* theEnumOptions, uiSelectionBox* theSelectionBox, float theSensitivity = -1.0f);
	void setPosition(const sf::Vector2f& newPosition, float nodeWidth, float height); // top left corner position
	void draw(sf::RenderWindow& window);
	void setValue(const void* data, int flags = 0);

	// the index tells which of the two components of a vector is gonna change
	void bind(int index, InteractionMode interactionMode = InteractionMode::Default);

	void* getDataPointer(); // so the data pointer of the node can be changed when a connection is created
	const void* getValue(int* outFlags = nullptr);

	void disable();
	void enable();
	bool isEnabled();

private:
	int pin;
	bool enabled = true;
	int type;
	float sensitivity;
	sf::Vertex* shapes = nullptr;
	std::vector<sf::Text> texts;
	std::string imagePath = "";
	ImageFieldContent imageContent = ImageFieldContent::None;
	std::string fontPath = "";
	const std::vector<std::string>* enumOptions;
	static uiSelectionBox* selectionBox;

	void updateTextPositions();
	void paintQuad(bool isHighlighted, int quadIndex);
	void* dataPointer;
	void (*onValueChanged)(int) = nullptr;
};