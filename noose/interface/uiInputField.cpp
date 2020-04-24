#include "uiInputField.h"
#include "uiColorPicker.h"
#include "uiSelectionBox.h"
#include "uiData.h"

#include "../math/uiMath.h"
#include "../utils.h"
#include "../types.h"

#include "uiFileSelector.h"
#include <iostream>


#define COLOR_EDITING_SENSITIVITY 0.01
#define INT_EDITING_SENSITIVITY 0.1
#define FLOAT_EDITING_SENSITIVITY 0.001
#define FONT_SIZE 12

const sf::Color INPUT_FIELD_COLOR = sf::Color(0x4b4b4bbb);

// static //
static uiInputField* editingInputField = nullptr;
static uiInputField::InteractionMode currentInteractionMode;
static std::string keyboardInputString = "";
static bool incorrectKeyboardInput = false;
// int
static float editingInputFieldHelper;
// vector2i
static char editingVectorComponent; // stores if editing x or y component of a vector2i
// color
static int editingColorHue;
static double editingColorSaturation;
static double editingColorValue;

static sf::Shader loadImageShader;
static bool loadImageShaderLoaded;

uiSelectionBox* uiInputField::selectionBox = nullptr;

void uiInputField::updateTextPositions()
{
	switch (type)
	{
	case NS_TYPE_FLOAT:
	case NS_TYPE_IMAGE:
	case NS_TYPE_INT:
		texts[0].setPosition((shapes[0].position + shapes[2].position) / 2.0f - sf::Vector2f(texts[0].getLocalBounds().width, texts[0].getLocalBounds().height) / 2.0f);
		break;
	case NS_TYPE_VECTOR2I:
		texts[0].setPosition((shapes[0].position + shapes[2].position) / 2.0f - sf::Vector2f(texts[0].getLocalBounds().width, texts[0].getLocalBounds().height) / 2.0f);
		texts[1].setPosition((shapes[4].position + shapes[6].position) / 2.0f - sf::Vector2f(texts[1].getLocalBounds().width, texts[1].getLocalBounds().height) / 2.0f);
		break;
	case NS_TYPE_COLOR:
		break;
	}
}

void onColorPickerSetColor(sf::Color* newColor)
{
	if (editingInputField != nullptr && editingInputField->type == NS_TYPE_COLOR)
	{
		editingInputField->shapes[0].color = editingInputField->shapes[1].color =
			editingInputField->shapes[2].color = editingInputField->shapes[3].color =
			*((sf::Color*)(editingInputField->dataPointer));

		editingInputField->onValueChanged();
	}
}

// static

void uiInputField::unbind()
{
	editingInputField = nullptr;
}

bool uiInputField::isBound()
{
	return editingInputField != nullptr;
}

void uiInputField::onMouseMoved(sf::Vector2f& displacement)
{
	if (editingInputField == nullptr || currentInteractionMode == InteractionMode::Typing)
		return;
	int newValueAux;
	switch (editingInputField->type)
	{
	case NS_TYPE_FLOAT:
		*((float*)(editingInputField->dataPointer)) += displacement.x * FLOAT_EDITING_SENSITIVITY;
		editingInputField->texts[0].setString(std::to_string(*((float*)(editingInputField->dataPointer))));
		editingInputField->onValueChanged();
		break;
	case NS_TYPE_INT:
		if (editingInputField->enumOptions->size() > 0)
			break;
		editingInputFieldHelper += displacement.x * INT_EDITING_SENSITIVITY;
		newValueAux = (int)editingInputFieldHelper;
		if (newValueAux != *((int*)(editingInputField->dataPointer)))
		{
			*((int*)(editingInputField->dataPointer)) = newValueAux;
			editingInputField->texts[0].setString(std::to_string((int)editingInputFieldHelper));
			editingInputField->onValueChanged();
		}
		break;
	case NS_TYPE_VECTOR2I:
		editingInputFieldHelper += displacement.x * INT_EDITING_SENSITIVITY;
		if (editingVectorComponent == 'x')
		{
			newValueAux = (int)editingInputFieldHelper;
			if (newValueAux != ((sf::Vector2i*)(editingInputField->dataPointer))->x)
			{
				((sf::Vector2i*)(editingInputField->dataPointer))->x = (int)editingInputFieldHelper;
				editingInputField->texts[0].setString(std::to_string((int)editingInputFieldHelper));
				editingInputField->onValueChanged();
			}
		}
		else
		{
		newValueAux = (int)editingInputFieldHelper;
		if (newValueAux != ((sf::Vector2i*)(editingInputField->dataPointer))->y)
		{
			((sf::Vector2i*)(editingInputField->dataPointer))->y = (int)editingInputFieldHelper;
			editingInputField->texts[1].setString(std::to_string((int)editingInputFieldHelper));
			editingInputField->onValueChanged();
		}
		}
		break;
	}
	editingInputField->updateTextPositions();
}

bool uiInputField::onMouseDown(sf::Vector2f& mousePos)
{
	if (editingInputField == nullptr)
		return false;

	if (editingInputField->type == NS_TYPE_INT &&
		editingInputField->enumOptions->size() > 0) // enum type
	{
		int index = selectionBox->mouseOver(mousePos);
		if (index > -1)
		{
			//std::cout << "INDEX: " << index << std::endl;
			*((int*)(editingInputField->dataPointer)) = index;
			editingInputField->texts[0].setString((*(editingInputField->enumOptions))[index]);
			editingInputField->onValueChanged();
		}
		selectionBox->hide();
		editingInputField->updateTextPositions();
		unbind();
		return true;
	}
	return false;
}

void uiInputField::onMouseUp()
{
	if (editingInputField == nullptr)
		return;

	switch (editingInputField->type)
	{
	case NS_TYPE_VECTOR2I:
	case NS_TYPE_FLOAT:
		unbind();
		break;
	case NS_TYPE_INT:
		if (editingInputField->enumOptions->size() == 0) // not an enum
			unbind();
		break;
	default:
		break;
	}
}

void uiInputField::keyboardInput(sf::Uint32 unicode)
{
	if (editingInputField == nullptr || currentInteractionMode == Default)
		return;

	if (unicode == '\t')
	{
		editingInputField->onValueChanged();
		if (editingInputField->type == NS_TYPE_VECTOR2I)
		{
			if (editingVectorComponent == 'x')
			{
				editingVectorComponent = 'y';
				editingInputField->texts[1].setString("");
				((sf::Vector2i*)(editingInputField->dataPointer))->y = 0;
			}
			else
			{
				editingVectorComponent = 'x';
				editingInputField->texts[0].setString("");
				((sf::Vector2i*)(editingInputField->dataPointer))->x = 0;
			}
			keyboardInputString = "";
			incorrectKeyboardInput = true;
		}
		return;
	}

	if (unicode == '\r' || unicode == '\n')
	{
		if (editingInputField->type == NS_TYPE_INT)
		{
			if (incorrectKeyboardInput)
			{
				*((int*)(editingInputField->dataPointer)) = 0;
				editingInputField->texts[0].setString("0");
			}
		}
		else if (editingInputField->type == NS_TYPE_FLOAT)
		{
			if (incorrectKeyboardInput)
			{
				*((float*)(editingInputField->dataPointer)) = 0.0;
				editingInputField->texts[0].setString("0.0");
			}
		}
		else if (editingInputField->type == NS_TYPE_VECTOR2I)
		{
			if (incorrectKeyboardInput)
			{
				if (editingVectorComponent == 'x')
				{
					((sf::Vector2i*)(editingInputField->dataPointer))->x = 0;
					editingInputField->texts[0].setString("0");
				}
				else
				{
					((sf::Vector2i*)(editingInputField->dataPointer))->y = 0;
					editingInputField->texts[1].setString("0");
				}
			}
		}
		//editingInputField->updateTextPositions();
		editingInputField->onValueChanged();
		unbind();
		return;
	}

	if (editingInputField->type == NS_TYPE_FLOAT)
	{
		if (unicode == '\b')
		{
			if (keyboardInputString.length() > 0)
				keyboardInputString.resize(keyboardInputString.length() - 1);
			else
				return;
		}
		else if (
			unicode <= '9' && unicode >= '0' || unicode == '-' && keyboardInputString.length() == 0 ||
			unicode == '.' && keyboardInputString.find('.') == -1)
			keyboardInputString += unicode;
		else
			return;

		try
		{
			*((float*)(editingInputField->dataPointer)) = std::stof(keyboardInputString);
			incorrectKeyboardInput = false;
		}
		catch (...)
		{
			incorrectKeyboardInput = true;
		}

		if (keyboardInputString.length() == 0)
			incorrectKeyboardInput = true;

		editingInputField->texts[0].setString(keyboardInputString);
		editingInputField->updateTextPositions();
	}
	else if (editingInputField->type == NS_TYPE_INT || editingInputField->type == NS_TYPE_VECTOR2I)
	{
		if (unicode == '\b')
		{
			if (keyboardInputString.length() > 0)
				keyboardInputString.resize(keyboardInputString.length() - 1);
			else
				return;
		}
		else if (unicode <= '9' && unicode >= '0' || unicode == '-' && keyboardInputString.length() == 0)
			keyboardInputString += unicode;
		else
			return;

		try
		{
			if (editingInputField->type == NS_TYPE_INT)
				*((int*)(editingInputField->dataPointer)) = std::stoi(keyboardInputString);
			else if (editingVectorComponent == 'x')
				((sf::Vector2i*)(editingInputField->dataPointer))->x = std::stoi(keyboardInputString);
			else
				((sf::Vector2i*)(editingInputField->dataPointer))->y = std::stoi(keyboardInputString);
			incorrectKeyboardInput = false;
		}
		catch (...)
		{
			incorrectKeyboardInput = true;
		}

		if (keyboardInputString.length() == 0)
			incorrectKeyboardInput = true;

		if (editingInputField->type == NS_TYPE_INT || editingVectorComponent == 'x')
			editingInputField->texts[0].setString(keyboardInputString);
		else
			editingInputField->texts[1].setString(keyboardInputString);

		editingInputField->updateTextPositions();
	}
}

// non static //

uiInputField::~uiInputField()
{
	if (texts != nullptr)
		delete[] texts;
	if (shapes != nullptr)
		delete[] shapes;
}

bool uiInputField::mouseOver(const sf::Vector2f& mousePosInWorld, int& index)
{
	switch (type)
	{
	case NS_TYPE_IMAGE:
	case NS_TYPE_COLOR:
	case NS_TYPE_FLOAT:
	case NS_TYPE_INT:
		return uiMath::isPointInsideRect(mousePosInWorld, shapes[3].position.y, shapes[0].position.y, shapes[0].position.x, shapes[1].position.x);
	case NS_TYPE_VECTOR2I: // edit vectors as separate integers
		if (uiMath::isPointInsideRect(mousePosInWorld, shapes[3].position.y, shapes[0].position.y, shapes[0].position.x, shapes[1].position.x))
		{
			index = 0;
			return true;
		}
		if (uiMath::isPointInsideRect(mousePosInWorld, shapes[7].position.y, shapes[4].position.y, shapes[4].position.x, shapes[5].position.x))
		{
			index = 1;
			return true;
		}
		return false;
	default:
		return false;
	}
}

// TODO: think of a better way for getting the selection box pointer
void uiInputField::create(int theType, void* pinDataPointer, void(onValueChangedFunc)(), const std::vector<std::string>* theEnumOptions, uiSelectionBox* theSelectionBox)
{
	selectionBox = theSelectionBox;
	enumOptions = theEnumOptions;
	onValueChanged = onValueChangedFunc;
	type = theType;
	dataPointer = pinDataPointer;
	switch (type)
	{
	case NS_TYPE_INT:
		shapes = new sf::Vertex[4];
		texts = new sf::Text[1];
		shapes[0].color = shapes[1].color = shapes[2].color = shapes[3].color = INPUT_FIELD_COLOR;
		texts[0].setFont(uiData::font);
		texts[0].setCharacterSize(FONT_SIZE);
		if (enumOptions->size() == 0)
			texts[0].setString(std::to_string(*((int*)pinDataPointer)));
		else
			texts[0].setString((*enumOptions)[(*((int*)pinDataPointer)) % enumOptions->size()]);

		break;
	case NS_TYPE_FLOAT:
		shapes = new sf::Vertex[4];
		texts = new sf::Text[1];
		shapes[0].color = shapes[1].color = shapes[2].color = shapes[3].color = INPUT_FIELD_COLOR;
		texts[0].setFont(uiData::font);
		texts[0].setCharacterSize(FONT_SIZE);
		texts[0].setString(std::to_string(*((float*)pinDataPointer)));
		break;
	case NS_TYPE_VECTOR2I:
		shapes = new sf::Vertex[8];
		texts = new sf::Text[2];
		texts[0].setFont(uiData::font);
		texts[1].setFont(uiData::font);
		texts[0].setCharacterSize(FONT_SIZE);
		texts[1].setCharacterSize(FONT_SIZE);
		texts[0].setString(std::to_string(((sf::Vector2i*)pinDataPointer)->x));
		texts[1].setString(std::to_string(((sf::Vector2i*)pinDataPointer)->y));
		shapes[0].color = shapes[1].color = shapes[2].color = shapes[3].color = shapes[4].color = shapes[5].color = shapes[6].color = shapes[7].color = INPUT_FIELD_COLOR;
		break;
	case NS_TYPE_COLOR:
		shapes = new sf::Vertex[4];
		shapes[0].color = shapes[1].color = shapes[2].color = shapes[3].color = sf::Color::Magenta;
		break;
	case NS_TYPE_IMAGE:
		shapes = new sf::Vertex[4];
		texts = new sf::Text[1];
		shapes[0].color = shapes[1].color = shapes[2].color = shapes[3].color = INPUT_FIELD_COLOR;
		texts[0].setFont(uiData::font);
		texts[0].setCharacterSize(FONT_SIZE);
		texts[0].setString("Open file...");
		break;
	}

	if (!loadImageShaderLoaded)
	{
		if (!loadImageShader.loadFromFile("res/shaders/loadImage.shader", sf::Shader::Fragment))
			std::cout << "[UI] Failed to load image loading shader\n";
		loadImageShaderLoaded = true;
	}
}


void uiInputField::setPosition(const sf::Vector2f& newPosition, float nodeWidth, float height)
{
	float margin = nodeWidth / 10.0f;

	switch (type)
	{
	case NS_TYPE_FLOAT:
	case NS_TYPE_IMAGE:
	case NS_TYPE_INT:
	case NS_TYPE_COLOR:
		shapes[0].position.x = shapes[3].position.x = newPosition.x + margin;
		shapes[1].position.x = shapes[2].position.x = newPosition.x + nodeWidth - margin;
		shapes[0].position.y = shapes[1].position.y = newPosition.y + height;
		shapes[2].position.y = shapes[3].position.y = newPosition.y;
		break;
	case NS_TYPE_VECTOR2I:
		shapes[0].position.x = shapes[3].position.x = newPosition.x + margin;
		shapes[1].position.x = shapes[2].position.x = newPosition.x + nodeWidth / 2.0f - margin / 2.0f;
		shapes[0].position.y = shapes[1].position.y = newPosition.y + height;
		shapes[2].position.y = shapes[3].position.y = newPosition.y;
		shapes[4].position.x = shapes[7].position.x = newPosition.x + nodeWidth / 2.0f + margin / 2.0f;
		shapes[5].position.x = shapes[6].position.x = newPosition.x + nodeWidth - margin;
		shapes[4].position.y = shapes[5].position.y = newPosition.y + height;
		shapes[6].position.y = shapes[7].position.y = newPosition.y;
		break;
	}
	updateTextPositions();
}

void uiInputField::draw(sf::RenderWindow& window)
{
	switch (type)
	{
	case NS_TYPE_INT:
	case NS_TYPE_FLOAT:
	case NS_TYPE_IMAGE:
		window.draw(shapes, 4, sf::Quads);
		window.draw(texts[0]);
		break;
	case NS_TYPE_VECTOR2I:
		window.draw(shapes, 8, sf::Quads);
		window.draw(texts[0]);
		window.draw(texts[1]);
		break;
	case NS_TYPE_COLOR:
		window.draw(shapes, 4, sf::Quads);
		break;
	}
}

void uiInputField::setValue(const void* value)
{
	if (type == NS_TYPE_INT)
	{
		texts[0].setString(std::to_string(*((int*)value)));
	}
	else if (type == NS_TYPE_FLOAT)
	{
		texts[0].setString(std::to_string(*((float*)value)));
	}
	else if (type == NS_TYPE_VECTOR2I)
	{
		sf::Vector2f* pointer = (sf::Vector2f*) value;

		texts[0].setString(std::to_string(pointer->x));
		texts[1].setString(std::to_string(pointer->y));
	}
}

// the index tells which of the two components of a vector is gonna change
void uiInputField::bind(int index, InteractionMode interactionMode)
{
	if (type != NS_TYPE_COLOR)
		uiColorPicker::hide();

	currentInteractionMode = interactionMode;
	editingInputField = this;

	if (currentInteractionMode == InteractionMode::Typing)
	{
		keyboardInputString = "";
		incorrectKeyboardInput = true;
	}

	switch (type)
	{
	case NS_TYPE_IMAGE:
	{
		if (currentInteractionMode == InteractionMode::Typing)
		{
			// no typing interaction to load an image
			editingInputField = nullptr;
			return;
		}

		char* filePath = uiFileSelector::openFileDialog();
		if (filePath != nullptr)
		{
			sf::Texture tx;
			if (!tx.loadFromFile(filePath))
			{
				std::cout << "[UI] Failed to open image file\n";
				return;
			}
			texts[0].setString(utils::getFileNameFromPath(filePath));
			loadImageShader.setUniform("tx", tx);

			sf::Sprite spr(tx);
			sf::Vector2u txSize = tx.getSize();
			sf::RenderTexture* pointer = (sf::RenderTexture*) dataPointer;

			pointer->create(txSize.x, txSize.y);
			pointer->draw(spr, &loadImageShader);
			editingInputField->onValueChanged();
			editingInputField->updateTextPositions();

			free(filePath);
		}

		editingInputField = nullptr;
		return;
	}
	case NS_TYPE_COLOR:
	{
		if (currentInteractionMode == InteractionMode::Default)
		{
			uiColorPicker::setOnColorSelectCallback(onColorPickerSetColor);
			uiColorPicker::show((sf::Color*) editingInputField->dataPointer, unbind);
		}
		return;
	}
	case NS_TYPE_FLOAT:
	{
		if (currentInteractionMode == InteractionMode::Typing)
		{
			this->texts[0].setString("");
			*((float*)(this->dataPointer)) = 0;
		}
		return;
	}
	case NS_TYPE_INT:
	{
		if (currentInteractionMode == InteractionMode::Typing)
		{
			this->texts[0].setString("");
			*((int*)(this->dataPointer)) = 0;
			return;
		}

		if (this->enumOptions->size() == 0) // normal int case
		{
			editingInputFieldHelper = (float)*((int*)(editingInputField->dataPointer));
		} // enum case
		else
		{
			selectionBox->display(shapes[3].position, *(editingInputField->enumOptions));
		}
		return;
	}
	case NS_TYPE_VECTOR2I: // edit vectors as separate integers
	{
		if (index == 0)
		{
			editingInputFieldHelper = (float)((sf::Vector2i*)(editingInputField->dataPointer))->x;
			editingVectorComponent = 'x';
			if (currentInteractionMode == InteractionMode::Typing)
			{
				this->texts[0].setString("");
				((sf::Vector2i*)(this->dataPointer))->x = 0;
			}
		}
		else if (index == 1)
		{
			editingInputFieldHelper = (float)((sf::Vector2i*)(editingInputField->dataPointer))->y;
			editingVectorComponent = 'y';
			if (currentInteractionMode == InteractionMode::Typing)
			{
				this->texts[1].setString("");
				((sf::Vector2i*)(this->dataPointer))->y = 0;
			}
		}
		return;
	}
	}
}

void* uiInputField::getDataPointer()
{
	return dataPointer;
}

void uiInputField::disable()
{
	enabled = false;
}

void uiInputField::enable()
{
	enabled = true;
}

bool uiInputField::isEnabled()
{
	return enabled;
}
