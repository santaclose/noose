#include "uiColorPicker.h"
#include <iostream>
#include <cstdint>

#define COLOR_WHEEL_SIZE 220.0
#define INTENSITY_AND_ALPHA_WIDTH 40.0
#define MARGIN_WIDTH 2.0

void (*onColorSelectedCallback)(sf::Color*);
sf::Color* outputPointer = nullptr;

sf::RenderStates rsOverwrite;
//sf::RenderStates rsAlphaBlend;

sf::RenderWindow* theWindow = nullptr;

sf::RenderTexture renderTexture;

sf::Shader colorWheelShader;
sf::Shader gradientShader;

sf::Image colorWheelImage; // copy to get pixel colors
sf::Image gradientImage;

sf::Vector2u lastColorPos;
float lastIntensity = 1.0;
sf::Uint8 lastAlpha = 255;

void (*cpOnCloseWindow)() = nullptr;

uiColorPicker::SelectionState selectionState = uiColorPicker::SelectionState::None;
sf::Vector2u mousePos;

sf::Color operator*(const sf::Color& c, const float f)
{
	return sf::Color(c.r * f, c.g * f, c.b * f, c.a);
}

void setColor()
{
	if (outputPointer == nullptr || onColorSelectedCallback == nullptr)
		return;

	switch (selectionState)
	{
	case uiColorPicker::SelectionState::Color:
		lastColorPos = sf::Vector2u(mousePos.x, mousePos.y);
		break;
	case uiColorPicker::SelectionState::Intensity:
		lastIntensity = gradientImage.getPixel(0, mousePos.y).r / 255.0;
		break;
	case uiColorPicker::SelectionState::Alpha:
		lastAlpha = gradientImage.getPixel(0, mousePos.y).r;
		break;
	}

	*outputPointer = colorWheelImage.getPixel(lastColorPos.x, lastColorPos.y) * lastIntensity;
	outputPointer->a = lastAlpha;
	onColorSelectedCallback(outputPointer);
}

void uiColorPicker::initialize()
{
	rsOverwrite.blendMode = sf::BlendNone;
	//rsAlphaBlend.blendMode = sf::BlendAlpha;

	if (!colorWheelShader.loadFromFile("assets/shaders/colorwheel.shader", sf::Shader::Fragment))
		std::cout << "[UI] Failed to load colorwheel shader\n";
	if (!gradientShader.loadFromFile("assets/shaders/gradient.shader", sf::Shader::Fragment))
		std::cout << "[UI] Failed to load gradient shader\n";

	// get colorwheel image in ram
	sf::VertexArray cva(sf::Quads, 4);
	cva[0].position.x = cva[0].position.y = cva[1].position.x = cva[3].position.y = 0.0;
	cva[2].position.x = cva[1].position.y = cva[3].position.x = cva[2].position.y = COLOR_WHEEL_SIZE;
	cva[0].texCoords.x = cva[0].texCoords.y = cva[1].texCoords.x = cva[3].texCoords.y = 0.0;
	cva[2].texCoords.x = cva[1].texCoords.y = cva[3].texCoords.x = cva[2].texCoords.y = 1.0;

	renderTexture.create(COLOR_WHEEL_SIZE, COLOR_WHEEL_SIZE);
	colorWheelShader.setUniform("limit", 0.0f);
	rsOverwrite.shader = &colorWheelShader;
	renderTexture.draw(cva, rsOverwrite);
	colorWheelImage = renderTexture.getTexture().copyToImage();

	// get intensity/alpha image in ram
	sf::VertexArray gva(sf::Quads, 4);
	gva[0].position.x = gva[1].position.x = gva[0].position.y = gva[3].position.y = 0.0;
	gva[2].position.x = gva[3].position.x = INTENSITY_AND_ALPHA_WIDTH;
	gva[1].position.y = gva[2].position.y = COLOR_WHEEL_SIZE;
	gva[0].texCoords.y = gva[3].texCoords.y = 0.0;
	gva[1].texCoords.y = gva[2].texCoords.y = 1.0;

	renderTexture.create(1, COLOR_WHEEL_SIZE);
	rsOverwrite.shader = &gradientShader;
	renderTexture.draw(gva, rsOverwrite);
	gradientImage = renderTexture.getTexture().copyToImage();

	// get final render texture to display
	renderTexture.create(COLOR_WHEEL_SIZE + INTENSITY_AND_ALPHA_WIDTH * 2 + MARGIN_WIDTH * 2, COLOR_WHEEL_SIZE);
	renderTexture.clear(sf::Color::Black); // margin color

	// draw colorwheel
	colorWheelShader.setUniform("limit", 1.0f);
	rsOverwrite.shader = &colorWheelShader;
	renderTexture.draw(cva, rsOverwrite);

	gva[0].position.x += MARGIN_WIDTH + COLOR_WHEEL_SIZE;
	gva[1].position.x += MARGIN_WIDTH + COLOR_WHEEL_SIZE;
	gva[2].position.x += MARGIN_WIDTH + COLOR_WHEEL_SIZE;
	gva[3].position.x += MARGIN_WIDTH + COLOR_WHEEL_SIZE;

	rsOverwrite.shader = &gradientShader;
	renderTexture.draw(gva, rsOverwrite);

	gva[0].position.x += MARGIN_WIDTH + INTENSITY_AND_ALPHA_WIDTH;
	gva[1].position.x += MARGIN_WIDTH + INTENSITY_AND_ALPHA_WIDTH;
	gva[2].position.x += MARGIN_WIDTH + INTENSITY_AND_ALPHA_WIDTH;
	gva[3].position.x += MARGIN_WIDTH + INTENSITY_AND_ALPHA_WIDTH;

	rsOverwrite.shader = &gradientShader;
	renderTexture.draw(gva, rsOverwrite);
}

void uiColorPicker::setOnColorSelectCallback(void (*onSetColor)(sf::Color*))
{
	onColorSelectedCallback = onSetColor;
}

void uiColorPicker::show(sf::Color* newPointer, void (*onCloseWindow)())
{
	cpOnCloseWindow = onCloseWindow;
	if (theWindow != nullptr)
	{
		theWindow->requestFocus();
	}
	else // initial case
	{
		theWindow = new sf::RenderWindow(sf::VideoMode(
				COLOR_WHEEL_SIZE + MARGIN_WIDTH * 2 + INTENSITY_AND_ALPHA_WIDTH * 2,
				COLOR_WHEEL_SIZE),
			"color picker",
			sf::Style::Close);
	}
	outputPointer = newPointer;
}

void uiColorPicker::hide()
{
	if (theWindow == nullptr)
		return;
	
	theWindow->close();
	delete theWindow;
	theWindow = nullptr;
}

void uiColorPicker::tick()
{
	if (theWindow == nullptr)
		return;

	sf::Event e;
	while (theWindow->pollEvent(e))
	{
		switch (e.type)
		{
			case sf::Event::Closed:
			{
				theWindow->close();
				cpOnCloseWindow();
				break;
			}
			case sf::Event::MouseButtonPressed:
			{
				if (e.mouseButton.button == sf::Mouse::Left)
				{
					if (e.mouseButton.x > INTENSITY_AND_ALPHA_WIDTH + MARGIN_WIDTH * 1.5 + COLOR_WHEEL_SIZE)
						selectionState = SelectionState::Alpha;
					else if (e.mouseButton.x < MARGIN_WIDTH * 0.5 + COLOR_WHEEL_SIZE)
						selectionState = SelectionState::Color;
					else
						selectionState = SelectionState::Intensity;

					mousePos = sf::Vector2u(e.mouseButton.x, e.mouseButton.y);
					setColor();
				}
				break;
			}
			case sf::Event::MouseButtonReleased:
			{
				selectionState = SelectionState::None;
				break;
			}
			case sf::Event::MouseMoved:
			{
				if (selectionState == SelectionState::None)
					return;
				mousePos = sf::Vector2u(e.mouseMove.x, e.mouseMove.y);
				setColor();
				break;
			}

		}
	}

	if (!theWindow->isOpen())
	{
		delete theWindow;
		theWindow = nullptr;
	}
	else
	{
		sf::Sprite sprt(renderTexture.getTexture());
		sprt.setPosition(0.0, 0.0);
		theWindow->draw(sprt);

		theWindow->display();
	}
}

void uiColorPicker::terminate()
{
	if (theWindow != nullptr)
	{
		theWindow->close();
		delete theWindow;
	}
}