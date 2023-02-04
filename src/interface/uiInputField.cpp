#include "uiInputField.h"
#include "uiColorPicker.h"
#include "uiSelectionBox.h"
#include "uiData.h"

#include "../math/nooseMath.h"
#include "../pathUtils.h"
#include "../types.h"
#include "../utils.h"

#include <iostream>
#include <portable-file-dialogs.h>


#define COLOR_EDITING_SENSITIVITY 0.01
#define INT_EDITING_SENSITIVITY 0.1
#define FLOAT_EDITING_SENSITIVITY 0.001
#define FONT_SIZE 12
#define TEXT_OFFSET_Y 3

const sf::Color INPUT_FIELD_COLOR = sf::Color(0x4b4b4bbb);
const sf::Color INPUT_FIELD_HIGHLIGHTED_COLOR = sf::Color(0x5e5e5ebb);

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

sf::Shader uiInputField::loadImageShader;
static bool loadImageShaderLoaded;

uiSelectionBox* uiInputField::selectionBox = nullptr;

void uiInputField::updateTextPositions()
{
	switch (type)
	{
	case NS_TYPE_STRING:
	case NS_TYPE_FONT:
	case NS_TYPE_FLOAT:
	case NS_TYPE_IMAGE:
	case NS_TYPE_INT:
		texts[0].setPosition((shapes[0].position + shapes[2].position) / 2.0f - sf::Vector2f(texts[0].getLocalBounds().width, texts[0].getLocalBounds().height) / 2.0f + sf::Vector2f(0.0, -TEXT_OFFSET_Y));
		break;
	case NS_TYPE_VECTOR2I:
		texts[0].setPosition((shapes[0].position + shapes[2].position) / 2.0f - sf::Vector2f(texts[0].getLocalBounds().width, texts[0].getLocalBounds().height) / 2.0f + sf::Vector2f(0.0, -TEXT_OFFSET_Y));
		texts[1].setPosition((shapes[4].position + shapes[6].position) / 2.0f - sf::Vector2f(texts[1].getLocalBounds().width, texts[1].getLocalBounds().height) / 2.0f + sf::Vector2f(0.0, -TEXT_OFFSET_Y));
		texts[2].setPosition((shapes[8].position + shapes[10].position) / 2.0f - sf::Vector2f(texts[2].getLocalBounds().width, texts[2].getLocalBounds().height) / 2.0f + sf::Vector2f(0.0, -TEXT_OFFSET_Y-1));
		break;
	case NS_TYPE_COLOR:
		break;
	}
}

void uiInputField::paintQuad(bool isHighlighted, int quadIndex)
{
	int base = quadIndex * 4;
	shapes[base + 0].color =
		shapes[base + 1].color =
		shapes[base + 2].color =
		shapes[base + 3].color = isHighlighted ? INPUT_FIELD_HIGHLIGHTED_COLOR : INPUT_FIELD_COLOR;
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
	if (editingInputField->type == NS_TYPE_VECTOR2I)
	{
		editingInputField->paintQuad(false, 0);
		editingInputField->paintQuad(false, 1);
		editingInputField->paintQuad(false, 2);
	}
	else if (editingInputField->type == NS_TYPE_INT ||
			editingInputField->type == NS_TYPE_FLOAT ||
			editingInputField->type == NS_TYPE_STRING)
	{
		editingInputField->paintQuad(false, 0);
	}

	editingInputField = nullptr;
}

bool uiInputField::isBound()
{
	return editingInputField != nullptr;
}

bool uiInputField::typingInteractionOngoing()
{
	return isBound() && currentInteractionMode == InteractionMode::Typing;
}

void uiInputField::setVectorData(const sf::Vector2i& vec)
{
	if (editingInputField != nullptr && editingInputField->type == NS_TYPE_VECTOR2I)
	{
		*((sf::Vector2i*)editingInputField->dataPointer) = vec;
		editingInputField->texts[0].setString(std::to_string(vec.x));
		editingInputField->texts[1].setString(std::to_string(vec.y));
		editingInputField->onValueChanged();
		editingInputField->updateTextPositions();
	}
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
		if (currentInteractionMode != Default)
			break;
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

	switch (editingInputField->type)
	{
	case NS_TYPE_INT:
	{
		if (editingInputField->enumOptions->size() > 0) // selection box type
		{
			int index = selectionBox->mouseOver(mousePos);
			if (index > -1)
			{
				*((int*)(editingInputField->dataPointer)) = index;
				editingInputField->texts[0].setString((*(editingInputField->enumOptions))[index]);
				editingInputField->onValueChanged();
			}
			selectionBox->hide();
			editingInputField->updateTextPositions();
			unbind();
			return true;
		}
		break;
	}
	case NS_TYPE_VECTOR2I:
		if (nooseMath::isPointInsideRect(mousePos, editingInputField->shapes[11].position, editingInputField->shapes[9].position))
		{
			unbind();
			return true;
		}
		break;
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
		if (currentInteractionMode == Default)
			unbind();
		break;
	case NS_TYPE_FLOAT:
		if (currentInteractionMode == Default)
			unbind();
		break;
	case NS_TYPE_INT:
		if (currentInteractionMode == Default && editingInputField->enumOptions->size() == 0) // int without selection box
			unbind();
		break;
	default:
		break;
	}
}

void uiInputField::keyboardInput(std::uint32_t unicode)
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
				editingInputField->paintQuad(false, 0);
				editingInputField->paintQuad(true, 1);
				editingVectorComponent = 'y';
				editingInputField->texts[1].setString("");
				((sf::Vector2i*)(editingInputField->dataPointer))->y = 0;
			}
			else
			{
				editingInputField->paintQuad(true, 0);
				editingInputField->paintQuad(false, 1);
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
		editingInputField->onValueChanged();
		unbind();
		return;
	}

	if (editingInputField->type == NS_TYPE_STRING)
	{
		if (unicode == '\b')
		{
			if (keyboardInputString.length() > 0)
				keyboardInputString.resize(keyboardInputString.length() - 1);
			else
				return;
		}
		else if (unicode == 127) // ctrl backspace
		{
			if (keyboardInputString.length() > 0)
			{
				int i = keyboardInputString.length() - 1;
				for (; i > 0 && keyboardInputString[i - 1] != ' '; i--);
				keyboardInputString = keyboardInputString.substr(0, i);
			}
		}
		else
			keyboardInputString += unicode;

		*((std::string*)(editingInputField->dataPointer)) = keyboardInputString;

		editingInputField->texts[0].setString(keyboardInputString);
		editingInputField->updateTextPositions();
	}
	else if (editingInputField->type == NS_TYPE_FLOAT)
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
	case NS_TYPE_STRING:
	case NS_TYPE_FONT:
	case NS_TYPE_IMAGE:
	case NS_TYPE_COLOR:
	case NS_TYPE_FLOAT:
	case NS_TYPE_INT:
		return nooseMath::isPointInsideRect(mousePosInWorld, shapes[3].position, shapes[1].position);
	case NS_TYPE_VECTOR2I:
		if (nooseMath::isPointInsideRect(mousePosInWorld, shapes[3].position, shapes[1].position))
		{
			index = 0;
			return true;
		}
		if (nooseMath::isPointInsideRect(mousePosInWorld, shapes[7].position, shapes[5].position))
		{
			index = 1;
			return true;
		}
		if (nooseMath::isPointInsideRect(mousePosInWorld, shapes[11].position, shapes[9].position))
		{
			index = 2;
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
	case NS_TYPE_STRING:
		shapes = new sf::Vertex[4];
		texts = new sf::Text[1];
		shapes[0].color = shapes[1].color = shapes[2].color = shapes[3].color = INPUT_FIELD_COLOR;
		texts[0].setFont(uiData::font);
		texts[0].setCharacterSize(FONT_SIZE);
		texts[0].setString(*((std::string*)pinDataPointer));
		break;
	case NS_TYPE_VECTOR2I:
		shapes = new sf::Vertex[12];
		texts = new sf::Text[3];
		texts[0].setFont(uiData::font);
		texts[1].setFont(uiData::font);
		texts[2].setFont(uiData::font);
		texts[0].setCharacterSize(FONT_SIZE);
		texts[1].setCharacterSize(FONT_SIZE);
		texts[2].setCharacterSize(FONT_SIZE);
		texts[0].setString(std::to_string(((sf::Vector2i*)pinDataPointer)->x));
		texts[1].setString(std::to_string(((sf::Vector2i*)pinDataPointer)->y));
		texts[2].setString("×");
		shapes[0].color = shapes[1].color = shapes[2].color = shapes[3].color =
			shapes[4].color = shapes[5].color = shapes[6].color = shapes[7].color =
			shapes[8].color = shapes[9].color = shapes[10].color = shapes[11].color = INPUT_FIELD_COLOR;
		break;
	case NS_TYPE_COLOR:
		shapes = new sf::Vertex[4];
		shapes[0].color = shapes[1].color = shapes[2].color = shapes[3].color = *((sf::Color*)pinDataPointer);
		break;
	case NS_TYPE_FONT:
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
		if (!loadImageShader.loadFromFile(pathUtils::getAssetsDirectory() + "shaders/loadImage.shader", sf::Shader::Fragment))
			std::cout << "[UI] Failed to load image loading shader\n";
		loadImageShaderLoaded = true;
	}
}

void uiInputField::setPosition(const sf::Vector2f& newPosition, float nodeWidth, float height)
{
	float margin = nodeWidth / 10.0f;

	switch (type)
	{
	case NS_TYPE_STRING:
	case NS_TYPE_FONT:
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
		float numberBoxWidth = nodeWidth * 0.3;
		shapes[0].position.x = shapes[3].position.x = newPosition.x + nodeWidth - margin * 1.25f - numberBoxWidth * 2.0f;
		shapes[1].position.x = shapes[2].position.x = newPosition.x + nodeWidth - margin * 1.25f - numberBoxWidth;
		shapes[0].position.y = shapes[1].position.y = newPosition.y + height;
		shapes[2].position.y = shapes[3].position.y = newPosition.y;
		shapes[4].position.x = shapes[7].position.x = newPosition.x + nodeWidth - margin - numberBoxWidth;
		shapes[5].position.x = shapes[6].position.x = newPosition.x + nodeWidth - margin;
		shapes[4].position.y = shapes[5].position.y = newPosition.y + height;
		shapes[6].position.y = shapes[7].position.y = newPosition.y;
		shapes[8].position.x = shapes[11].position.x = newPosition.x + margin;
		shapes[9].position.x = shapes[10].position.x = newPosition.x + margin + height;
		shapes[8].position.y = shapes[9].position.y = newPosition.y + height;
		shapes[10].position.y = shapes[11].position.y = newPosition.y;
		break;
	}
	updateTextPositions();
}

void uiInputField::draw(sf::RenderWindow& window)
{
	switch (type)
	{
	case NS_TYPE_STRING:
	case NS_TYPE_FONT:
	case NS_TYPE_INT:
	case NS_TYPE_FLOAT:
	case NS_TYPE_IMAGE:
		utils::drawQuads(window, shapes, 4);
		window.draw(texts[0]);
		break;
	case NS_TYPE_VECTOR2I:
		utils::drawQuads(window, shapes, 12);
		window.draw(texts[0]);
		window.draw(texts[1]);
		window.draw(texts[2]);
		break;
	case NS_TYPE_COLOR:
		utils::drawQuads(window, shapes, 4);
		break;
	}
}

void uiInputField::setValue(const void* data)
{
	switch (type)
	{
	case NS_TYPE_INT:
	{
		*((int*)(dataPointer)) = *((int*)(data));
		texts[0].setString(std::to_string(*((int*)data)));
		break;
	}
	case NS_TYPE_FLOAT:
	{
		*((float*)(dataPointer)) = *((float*)(data));
		texts[0].setString(std::to_string(*((float*)data)));
		break;
	}
	case NS_TYPE_VECTOR2I:
	{
		*((sf::Vector2f*)(dataPointer)) = *((sf::Vector2f*)(data));
		sf::Vector2f* pointer = (sf::Vector2f*)data;
		texts[0].setString(std::to_string(pointer->x));
		texts[1].setString(std::to_string(pointer->y));
		break;
	}
	case NS_TYPE_COLOR:
	{
		*((sf::Color*)(dataPointer)) = *((sf::Color*)(data));
		shapes[0].color =
			shapes[1].color =
			shapes[2].color =
			shapes[3].color =
			*((sf::Color*)(dataPointer));
		break;
	}
	case NS_TYPE_STRING:
	{
		*((std::string*)(dataPointer)) = *((std::string*)(data));
		texts[0].setString(*((std::string*)data));
		break;
	}
	case NS_TYPE_IMAGE:
	{
		sf::Texture tx;
		const std::string& filePath = *((const std::string*)data);
		if (!tx.loadFromFile(filePath))
		{
			std::cout << "[UI] Failed to open image file\n";
			return;
		}
		imagePath = std::string(filePath);
		texts[0].setString(pathUtils::getFileNameFromPath(filePath.c_str()));
		loadImageShader.setUniform("tx", tx);

		sf::Sprite spr(tx);
		sf::Vector2u txSize = tx.getSize();
		sf::RenderTexture* pointer = (sf::RenderTexture*)dataPointer;

		pointer->create(txSize);
		sf::RenderStates rs;
		rs.shader = &loadImageShader;
		rs.blendMode = sf::BlendNone;
		pointer->draw(spr, rs);
		break;
	}
	case NS_TYPE_FONT:
	{
		sf::Font* pointer = (sf::Font*)dataPointer;
		const std::string& filePath = *((const std::string*)data);
		if (!pointer->loadFromFile(filePath))
		{
			std::cout << "[UI] Failed to open font file\n";
			return;
		}
		fontPath = std::string(filePath);
		texts[0].setString(pathUtils::getFileNameFromPath(filePath.c_str()));
		break;
	}
	}

	updateTextPositions();
	onValueChanged();
}

// the index tells which of the two components of a vector is gonna change
void uiInputField::bind(int index, InteractionMode interactionMode)
{
	if (editingInputField != nullptr)
		unbind();

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
	case NS_TYPE_FONT:
	{
		if (currentInteractionMode == InteractionMode::Typing)
		{
			// no typing interaction to load fonts
			editingInputField = nullptr;
			return;
		}

		std::vector<std::string> selection = pfd::open_file("Open font", "", { "Font Files", "*.ttf *.otf" }).result();
		if (selection.size() != 0)
		{
			sf::Font* pointer = (sf::Font*)dataPointer;
			if (!pointer->loadFromFile(selection[0]))
			{
				std::cout << "[UI] Failed to open font file\n";
				return;
			}
			fontPath = selection[0];
			texts[0].setString(pathUtils::getFileNameFromPath(selection[0].c_str()));

			editingInputField->updateTextPositions();
			editingInputField->onValueChanged();
		}

		unbind();
		return;
	}
	case NS_TYPE_IMAGE:
	{
		if (currentInteractionMode == InteractionMode::Typing)
		{
			// no typing interaction to load an image
			editingInputField = nullptr;
			return;
		}

		std::vector<std::string> selection = pfd::open_file("Open image", "", { "Image Files", "*.png *.jpg *.jpeg *.bmp *.tga *.gif *.psd *.hdr *.pic *.tif *.tiff" }).result();
		if (selection.size() != 0)
		{
			sf::Texture tx;
			if (!tx.loadFromFile(selection[0]))
			{
				std::cout << "[UI] Failed to open image file\n";
				return;
			}
			imagePath = selection[0];
			texts[0].setString(pathUtils::getFileNameFromPath(selection[0].c_str()));
			loadImageShader.setUniform("tx", tx);

			sf::Sprite spr(tx);
			sf::Vector2u txSize = tx.getSize();
			sf::RenderTexture* pointer = (sf::RenderTexture*) dataPointer;

			pointer->create(txSize);
			sf::RenderStates rs;
			rs.shader = &loadImageShader;
			rs.blendMode = sf::BlendNone;
			pointer->draw(spr, rs);

			editingInputField->updateTextPositions();
			editingInputField->onValueChanged();
		}

		unbind();
		return;
	}
	case NS_TYPE_COLOR:
	{
		if (currentInteractionMode == InteractionMode::Default)
		{
			uiColorPicker::setOnColorSelectCallback(onColorPickerSetColor);
			uiColorPicker::show((sf::Color*) editingInputField->dataPointer, unbind);
			uiColorPicker::updatePositionsFromColor();
		}
		return;
	}
	case NS_TYPE_FLOAT:
	{
		paintQuad(true, 0);
		if (currentInteractionMode == InteractionMode::Typing)
		{
			this->texts[0].setString("");
			*((float*)(this->dataPointer)) = 0;
		}
		return;
	}
	case NS_TYPE_STRING:
	{
		if (currentInteractionMode == InteractionMode::Typing)
		{
			paintQuad(true, 0);
			keyboardInputString = "";
			this->texts[0].setString("");
			*((std::string*)(this->dataPointer)) = "";
		}
		else
			unbind();
		return;
	}
	case NS_TYPE_INT:
	{
		paintQuad(true, 0);
		if (currentInteractionMode == InteractionMode::Typing)
		{
			this->texts[0].setString("");
			*((int*)(this->dataPointer)) = 0;
			return;
		}

		if (this->enumOptions->size() == 0) // int with no selection box case
		{
			editingInputFieldHelper = (float)*((int*)(editingInputField->dataPointer));
		} // selection box case
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
			paintQuad(true, 0);
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
			paintQuad(true, 1);
			editingInputFieldHelper = (float)((sf::Vector2i*)(editingInputField->dataPointer))->y;
			editingVectorComponent = 'y';
			if (currentInteractionMode == InteractionMode::Typing)
			{
				this->texts[1].setString("");
				((sf::Vector2i*)(this->dataPointer))->y = 0;
			}
		}
		else if (index == 2)
		{
			paintQuad(true, 2);
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
