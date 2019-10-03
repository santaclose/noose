#include "uiInputField.h"
#include "uiMath.h"
#include "uiColorPicker.h"

#include "dataController.h"

#include "vendor/nfd.h"
#include <iostream>

#define COLOR_EDITING_SENSITIVITY 0.01
#define INT_EDITING_SENSITIVITY 0.1
#define FLOAT_EDITING_SENSITIVITY 0.001
#define FONT_SIZE 12

const sf::Color INPUT_FIELD_COLOR = sf::Color(0x282828bb);

// static //
static bool bEditingInputField;
static uiInputField* editingInputField;
// int
static float editingInputFieldHelper;
// vector2i
static char editingVectorComponent; // stores if editing x or y component of a vector2i
// color
static int editingColorHue;
static double editingColorSaturation;
static double editingColorValue;

char* getFileNameFromPath(char* string)
{
	int i = 0;
	for (; string[i] != '\0'; i++);
	for (; string[i - 1] != '\\' && string[i - 1] != '/'; i--);
	return &(string[i]);
}

void uiInputField::updateTextPositions()
{
	switch (type)
	{
	case uiNodeSystem::Types::Float:
	case uiNodeSystem::Types::Image:
	case uiNodeSystem::Types::Integer:
		texts[0].setPosition((shapes[0].position + shapes[2].position) / 2.0f - sf::Vector2f(texts[0].getLocalBounds().width, texts[0].getLocalBounds().height) / 2.0f);
		break;
	case uiNodeSystem::Types::Vector2i:
		texts[0].setPosition((shapes[0].position + shapes[2].position) / 2.0f - sf::Vector2f(texts[0].getLocalBounds().width, texts[0].getLocalBounds().height) / 2.0f);
		texts[1].setPosition((shapes[4].position + shapes[6].position) / 2.0f - sf::Vector2f(texts[1].getLocalBounds().width, texts[1].getLocalBounds().height) / 2.0f);
		break;
	case uiNodeSystem::Types::Color:
		break;
	}
}

void onColorPickerSetColor(sf::Color* newColor)
{
	if (editingInputField->type == uiNodeSystem::Types::Color)
	{
		editingInputField->shapes[0].color = editingInputField->shapes[1].color =
			editingInputField->shapes[2].color = editingInputField->shapes[3].color =
			*((sf::Color*)(editingInputField->dataPointer));

		editingInputField->onValueChanged();
	}
}

// static

void uiInputField::onLeftClickReleased()
{
	bEditingInputField = false;
}

void uiInputField::onMouseMoved(sf::Vector2f& displacement)
{
	if (!bEditingInputField)
		return;
	int newValueAux;
	switch (editingInputField->type)
	{
	case uiNodeSystem::Types::Float:
		*((float*)(editingInputField->dataPointer)) += displacement.x * FLOAT_EDITING_SENSITIVITY;
		editingInputField->texts[0].setString(std::to_string(*((float*)(editingInputField->dataPointer))));
		editingInputField->onValueChanged();
		break;
	case uiNodeSystem::Types::Integer:
		editingInputFieldHelper += displacement.x * INT_EDITING_SENSITIVITY;
		newValueAux = (int)editingInputFieldHelper;
		if (newValueAux != *((int*)(editingInputField->dataPointer)))
		{
			*((int*)(editingInputField->dataPointer)) = newValueAux;
			editingInputField->texts[0].setString(std::to_string((int)editingInputFieldHelper));
			editingInputField->onValueChanged();
		}
		break;
	case uiNodeSystem::Types::Vector2i:
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
	/*case uiNodeSystem::Types::Color:
		editingColorHue += displacement.x * COLOR_EDITING_SENSITIVITY;
		if (editingColorHue >= 360) editingColorHue -= 360; else if (editingColorHue <= 0) editingColorHue += 360;
		editingColorSaturation += displacement.y * COLOR_EDITING_SENSITIVITY;
		if (editingColorSaturation > 1.0f) editingColorSaturation = 1.0f; else if (editingColorSaturation < 0.0f) editingColorSaturation = 0.0f;
		uiMath::HSVtoRGB(editingColorHue, editingColorSaturation, editingColorValue, *((sf::Color*)(editingInputField->dataPointer)));

		editingInputField->shapes[0].color = editingInputField->shapes[1].color =
			editingInputField->shapes[2].color = editingInputField->shapes[3].color =
			*((sf::Color*)(editingInputField->dataPointer));

		editingInputField->onValueChanged();
		break;*/
	}
	editingInputField->updateTextPositions();
}
/*
void uiInputField::onMouseScrolled(float delta)
{
	if (!bEditingInputField)
		return;

	if (editingInputField->type == uiNodeSystem::Types::Color)
	{
		editingColorValue += delta * INT_EDITING_SENSITIVITY;
		if (editingColorValue > 1.0) editingColorValue = 1.0; else if (editingColorValue < 0.0) editingColorValue = 0.0;
		uiMath::HSVtoRGB(editingColorHue, editingColorSaturation, editingColorValue, *((sf::Color*)(editingInputField->dataPointer)));

		editingInputField->shapes[0].color = editingInputField->shapes[1].color =
			editingInputField->shapes[2].color = editingInputField->shapes[3].color =
			*((sf::Color*)(editingInputField->dataPointer));
	}
}*/

// non static //

uiInputField::~uiInputField()
{
	delete[] texts;
	delete[] shapes;
}

void uiInputField::initialize(uiNodeSystem::Types theType, void* pinDataPointer, void (onValueChangedFunc)())
{
	onValueChanged = onValueChangedFunc;
	type = theType;
	switch (type)
	{
	case uiNodeSystem::Types::Integer:
		dataPointer = pinDataPointer;

		shapes = new sf::Vertex[4];
		texts = new sf::Text[1];
		shapes[0].color = shapes[1].color = shapes[2].color = shapes[3].color = INPUT_FIELD_COLOR;
		texts[0].setFont(uiNodeSystem::font);
		texts[0].setCharacterSize(FONT_SIZE);
		texts[0].setString(std::to_string(*((int*)pinDataPointer)));
		break;
	case uiNodeSystem::Types::Float:
		dataPointer = pinDataPointer;

		shapes = new sf::Vertex[4];
		texts = new sf::Text[1];
		shapes[0].color = shapes[1].color = shapes[2].color = shapes[3].color = INPUT_FIELD_COLOR;
		texts[0].setFont(uiNodeSystem::font);
		texts[0].setCharacterSize(FONT_SIZE);
		texts[0].setString(std::to_string(*((float*)pinDataPointer)));
		break;
	case uiNodeSystem::Types::Vector2i:
		dataPointer = pinDataPointer;

		shapes = new sf::Vertex[8];
		texts = new sf::Text[2];
		texts[0].setFont(uiNodeSystem::font);
		texts[1].setFont(uiNodeSystem::font);
		texts[0].setCharacterSize(FONT_SIZE);
		texts[1].setCharacterSize(FONT_SIZE);
		texts[0].setString(std::to_string(((sf::Vector2i*)pinDataPointer)->x));
		texts[1].setString(std::to_string(((sf::Vector2i*)pinDataPointer)->y));
		shapes[0].color = shapes[1].color = shapes[2].color = shapes[3].color = shapes[4].color = shapes[5].color = shapes[6].color = shapes[7].color = INPUT_FIELD_COLOR;
		break;
	case uiNodeSystem::Types::Color:
		dataPointer = pinDataPointer;

		shapes = new sf::Vertex[4];
		texts = nullptr;
		shapes[0].color = shapes[1].color = shapes[2].color = shapes[3].color = sf::Color::Magenta;
		break;
	case uiNodeSystem::Types::Image:
		dataPointer = pinDataPointer;

		shapes = new sf::Vertex[4];
		texts = new sf::Text[1];
		shapes[0].color = shapes[1].color = shapes[2].color = shapes[3].color = INPUT_FIELD_COLOR;
		texts[0].setFont(uiNodeSystem::font);
		texts[0].setCharacterSize(FONT_SIZE);
		texts[0].setString("None");
		break;
	}
}


void uiInputField::setPosition(const sf::Vector2f& newPosition, float nodeWidth, float height)
{
	float margin = nodeWidth / 10.0f;

	switch (type)
	{
	case uiNodeSystem::Types::Float:
	case uiNodeSystem::Types::Image:
	case uiNodeSystem::Types::Integer:
		shapes[0].position.x = shapes[3].position.x = newPosition.x + margin;
		shapes[1].position.x = shapes[2].position.x = newPosition.x + nodeWidth - margin;
		shapes[0].position.y = shapes[1].position.y = newPosition.y + height;
		shapes[2].position.y = shapes[3].position.y = newPosition.y;
		texts[0].setPosition((shapes[0].position + shapes[2].position) / 2.0f - sf::Vector2f(texts[0].getLocalBounds().width, texts[0].getLocalBounds().height) / 2.0f);
		break;
	case uiNodeSystem::Types::Vector2i:
		shapes[0].position.x = shapes[3].position.x = newPosition.x + margin;
		shapes[1].position.x = shapes[2].position.x = newPosition.x + nodeWidth / 2.0f - margin / 2.0f;
		shapes[0].position.y = shapes[1].position.y = newPosition.y + height;
		shapes[2].position.y = shapes[3].position.y = newPosition.y;
		shapes[4].position.x = shapes[7].position.x = newPosition.x + nodeWidth / 2.0f + margin / 2.0f;
		shapes[5].position.x = shapes[6].position.x = newPosition.x + nodeWidth - margin;
		shapes[4].position.y = shapes[5].position.y = newPosition.y + height;
		shapes[6].position.y = shapes[7].position.y = newPosition.y;
		texts[0].setPosition((shapes[0].position + shapes[2].position) / 2.0f - sf::Vector2f(texts[0].getLocalBounds().width, texts[0].getLocalBounds().height) / 2.0f);
		texts[1].setPosition((shapes[4].position + shapes[6].position) / 2.0f - sf::Vector2f(texts[1].getLocalBounds().width, texts[1].getLocalBounds().height) / 2.0f);
		break;
	case uiNodeSystem::Types::Color:
		shapes[0].position.x = shapes[3].position.x = newPosition.x + margin;
		shapes[1].position.x = shapes[2].position.x = newPosition.x + nodeWidth - margin;
		shapes[0].position.y = shapes[1].position.y = newPosition.y + height;
		shapes[2].position.y = shapes[3].position.y = newPosition.y;
		break;
	}
}

void uiInputField::draw(sf::RenderWindow& window)
{
	switch (type)
	{
	case uiNodeSystem::Types::Integer:
		window.draw(shapes, 4, sf::Quads);
		window.draw(texts[0]);
		break;
	case uiNodeSystem::Types::Float:
		window.draw(shapes, 4, sf::Quads);
		window.draw(texts[0]);
		break;
	case uiNodeSystem::Types::Vector2i:
		window.draw(shapes, 8, sf::Quads);
		window.draw(texts[0]);
		window.draw(texts[1]);
		break;
	case uiNodeSystem::Types::Color:
		window.draw(shapes, 4, sf::Quads);
		break;
	case uiNodeSystem::Types::Image:
		window.draw(shapes, 4, sf::Quads);
		window.draw(texts[0]);

		/*sf::RenderTexture* pointer = (sf::RenderTexture*)dataPointer;
		sf::Sprite spr(pointer->getTexture());
		spr.setPosition(shapes[1].position);
		window.draw(spr);*/
		break;
	}
}

void uiInputField::setValue(const void* value)
{
	if (type == uiNodeSystem::Types::Integer)
	{
		texts[0].setString(std::to_string(*((int*)value)));
	}
	else if (type == uiNodeSystem::Types::Float)
	{
		texts[0].setString(std::to_string(*((float*)value)));
	}
	else if (type == uiNodeSystem::Types::Vector2i)
	{
		sf::Vector2f* pointer = (sf::Vector2f*) value;

		texts[0].setString(std::to_string(pointer->x));
		texts[1].setString(std::to_string(pointer->y));
	}
}

bool uiInputField::onClick(const sf::Vector2f& mousePosInWorld)
{
	switch (type)
	{
	case uiNodeSystem::Types::Image:
		if (uiMath::isPointInsideRect(mousePosInWorld, shapes[3].position.y, shapes[0].position.y, shapes[0].position.x, shapes[1].position.x))
		{
			editingInputField = this;
			nfdchar_t* outPath = nullptr;
			nfdchar_t filter[] = "png;jpg;psd";
			nfdresult_t result = NFD_OpenDialog(filter, NULL, &outPath);

			if (result == NFD_OKAY) {
				puts("Success!");
				puts(outPath);
				texts[0].setString(getFileNameFromPath(outPath));

				sf::Texture tx;
				if (!tx.loadFromFile(outPath))
				{
					std::cout << "Could not open the image" << std::endl;
					return true;
				}
				dataController::loadImageShader.setUniform("tx", tx);

				sf::Sprite spr(tx);
				sf::Vector2u txSize = tx.getSize();
				sf::RenderTexture* pointer = (sf::RenderTexture*) dataPointer;

				pointer->create(txSize.x, txSize.y);
				pointer->draw(spr, &dataController::loadImageShader);
				editingInputField->onValueChanged();

				free(outPath);
			}
			else if (result == NFD_CANCEL)
			{
				puts("User pressed cancel.");
			}
			else
			{
				printf("Error: %s\n", NFD_GetError());
			}

			editingInputField = nullptr;
			return true;
		}
		return false;
	case uiNodeSystem::Types::Color:
		if (uiMath::isPointInsideRect(mousePosInWorld, shapes[3].position.y, shapes[0].position.y, shapes[0].position.x, shapes[1].position.x))
		{
			editingInputField = this;
			bEditingInputField = true;

			///
			uiColorPicker::onSetColor = onColorPickerSetColor;
			uiColorPicker::show((sf::Color*) editingInputField->dataPointer);
			///
			return true;
		}
		return false;
	case uiNodeSystem::Types::Float:
		if (uiMath::isPointInsideRect(mousePosInWorld, shapes[3].position.y, shapes[0].position.y, shapes[0].position.x, shapes[1].position.x))
		{
			editingInputField = this;
			bEditingInputField = true;
			return true;
		}
		return false;
	case uiNodeSystem::Types::Integer:
		if (uiMath::isPointInsideRect(mousePosInWorld, shapes[3].position.y, shapes[0].position.y, shapes[0].position.x, shapes[1].position.x))
		{
			editingInputField = this;
			editingInputFieldHelper = (float) *((int*)(editingInputField->dataPointer));
			bEditingInputField = true;
			return true;
		}
		return false;
	case uiNodeSystem::Types::Vector2i: // edit vectors as separate integers
		if (uiMath::isPointInsideRect(mousePosInWorld, shapes[3].position.y, shapes[0].position.y, shapes[0].position.x, shapes[1].position.x))
		{
			editingInputField = this;
			editingInputFieldHelper = (float) ((sf::Vector2i*)(editingInputField->dataPointer))->x;
			editingVectorComponent = 'x';
			bEditingInputField = true;
			return true;
		}
		if (uiMath::isPointInsideRect(mousePosInWorld, shapes[7].position.y, shapes[4].position.y, shapes[4].position.x, shapes[5].position.x))
		{
			editingInputField = this;
			editingInputFieldHelper = (float)((sf::Vector2i*)(editingInputField->dataPointer))->y;
			editingVectorComponent = 'y';
			bEditingInputField = true;
			return true;
		}
		return false;
	}
}

void* uiInputField::getDataPointer()
{
	return dataPointer;
}