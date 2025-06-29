#include "uiColorPicker.h"
#include <iostream>
#include <cstdint>
#include <cmath>
#include "../math/nooseMath.h"
#include "../utils.h"
#include "../pathUtils.h"

#define COLOR_WHEEL_SIZE 220.0
#define COLOR_WHEEL_RADIUS 0.45
#define INTENSITY_AND_ALPHA_WIDTH 40.0
#define MARGIN_WIDTH 2.0

namespace uiColorPicker {

	void (*onColorSelectedCallback)(sf::Color*);
	sf::Color* outputPointer = nullptr;

	sf::RenderStates rsOverwrite;

	sf::RenderWindow* theWindow = nullptr;

	sf::RenderTexture renderTexture;

	sf::Shader colorWheelShader;
	sf::Shader gradientShader;

	sf::Image colorWheelImage; // copy to get pixel colors
	sf::Image gradientImage;

	sf::Texture marker;
	sf::Sprite* markerSprite;

	sf::Vector2i lastColorPos;
	int lastIntensityPos;
	int lastAlphaPos;

	void (*onCloseWindowCallback)() = nullptr;

	uiColorPicker::SelectionState selectionState = uiColorPicker::SelectionState::None;
	sf::Vector2i mousePos;

	const sf::Image* iconImage;

	void setColor();
}

sf::Color operator*(const sf::Color& c, const float f)
{
	return sf::Color(c.r * f, c.g * f, c.b * f, c.a);
}

void uiColorPicker::setColor()
{
	if (outputPointer == nullptr || onColorSelectedCallback == nullptr)
		return;

	// clamp between 0 and image size
	switch (selectionState)
	{
	case uiColorPicker::SelectionState::Color:
		lastColorPos = mousePos;
		if (lastColorPos.x < 0) lastColorPos.x = 0; else if (lastColorPos.x > gradientImage.getSize().y - 1) lastColorPos.x = gradientImage.getSize().y - 1;
		if (lastColorPos.y < 0) lastColorPos.y = 0; else if (lastColorPos.y > gradientImage.getSize().y - 1) lastColorPos.y = gradientImage.getSize().y - 1;
		break;
	case uiColorPicker::SelectionState::Intensity:
		lastIntensityPos = mousePos.y;
		if (lastIntensityPos < 0) lastIntensityPos = 0; else if (lastIntensityPos > gradientImage.getSize().y - 1) lastIntensityPos = gradientImage.getSize().y - 1;
		break;
	case uiColorPicker::SelectionState::Alpha:
		lastAlphaPos = mousePos.y;
		if (lastAlphaPos < 0) lastAlphaPos = 0; else if (lastAlphaPos > gradientImage.getSize().y - 1) lastAlphaPos = gradientImage.getSize().y - 1;
		break;
	}

	unsigned int colorwheelX = utils::clamp(lastColorPos.x, 0, colorWheelImage.getSize().x);
	unsigned int colorwheelY = utils::clamp(lastColorPos.y, 0, colorWheelImage.getSize().y);
	unsigned int intensityY = utils::clamp(lastIntensityPos, 0, gradientImage.getSize().y);
	unsigned int alphaY = utils::clamp(lastAlphaPos, 0, gradientImage.getSize().y);
	*outputPointer = colorWheelImage.getPixel({ colorwheelX, colorwheelY }) *
		(gradientImage.getPixel({ 0U, intensityY }).r / 255.0f);
	outputPointer->a = gradientImage.getPixel({ 0U, alphaY }).r;
	onColorSelectedCallback(outputPointer);
}

void uiColorPicker::initialize(const sf::Image& iconImage)
{
	uiColorPicker::iconImage = &iconImage;
	rsOverwrite.blendMode = sf::BlendNone;

	if (!marker.loadFromFile(pathUtils::getAssetsDirectory() + "images/imageLimit.png"))
		std::cout << "[UI] Failed to load image limit image\n";
	markerSprite = new sf::Sprite(marker);

	if (!colorWheelShader.loadFromFile(pathUtils::getAssetsDirectory() + "shaders/colorwheel.shader", sf::Shader::Type::Fragment))
		std::cout << "[UI] Failed to load colorwheel shader\n";
	if (!gradientShader.loadFromFile(pathUtils::getAssetsDirectory() + "shaders/gradient.shader", sf::Shader::Type::Fragment))
		std::cout << "[UI] Failed to load gradient shader\n";

	// get colorwheel image in ram
	sf::VertexArray cva(sf::PrimitiveType::TriangleFan, 4);
	cva[0].position.x = cva[0].position.y = cva[1].position.x = cva[3].position.y = 0.0;
	cva[2].position.x = cva[1].position.y = cva[3].position.x = cva[2].position.y = COLOR_WHEEL_SIZE;
	cva[0].texCoords.x = cva[0].texCoords.y = cva[1].texCoords.x = cva[3].texCoords.y = 0.0;
	cva[2].texCoords.x = cva[1].texCoords.y = cva[3].texCoords.x = cva[2].texCoords.y = 1.0;

	renderTexture = sf::RenderTexture({ (unsigned int)COLOR_WHEEL_SIZE, (unsigned int)COLOR_WHEEL_SIZE });
	colorWheelShader.setUniform("circleRadius", (float)COLOR_WHEEL_RADIUS);
	colorWheelShader.setUniform("limit", 0.0f);
	rsOverwrite.shader = &colorWheelShader;
	renderTexture.draw(cva, rsOverwrite);
	colorWheelImage = renderTexture.getTexture().copyToImage();

	// get intensity/alpha image in ram
	sf::VertexArray gva(sf::PrimitiveType::TriangleFan, 4);
	gva[0].position.x = gva[1].position.x = gva[0].position.y = gva[3].position.y = 0.0;
	gva[2].position.x = gva[3].position.x = INTENSITY_AND_ALPHA_WIDTH;
	gva[1].position.y = gva[2].position.y = COLOR_WHEEL_SIZE;
	gva[0].texCoords.y = gva[3].texCoords.y = 0.0;
	gva[1].texCoords.y = gva[2].texCoords.y = 1.0;

	renderTexture = sf::RenderTexture({ (unsigned int)1, (unsigned int)COLOR_WHEEL_SIZE });
	rsOverwrite.shader = &gradientShader;
	renderTexture.draw(gva, rsOverwrite);
	gradientImage = renderTexture.getTexture().copyToImage();

	// get final render texture to display
	renderTexture = sf::RenderTexture({ (unsigned int)(COLOR_WHEEL_SIZE + INTENSITY_AND_ALPHA_WIDTH * 2 + MARGIN_WIDTH * 2), (unsigned int)COLOR_WHEEL_SIZE });
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
	onCloseWindowCallback = onCloseWindow;
	if (theWindow != nullptr)
	{
		theWindow->requestFocus();
	}
	else // initial case
	{
		theWindow = new sf::RenderWindow(sf::VideoMode(
			{ (unsigned int)(COLOR_WHEEL_SIZE + MARGIN_WIDTH * 2 + INTENSITY_AND_ALPHA_WIDTH * 2),
				(unsigned int)COLOR_WHEEL_SIZE }),
			"color picker",
			sf::Style::Close);
		theWindow->setIcon({ iconImage->getSize().x, iconImage->getSize().y }, iconImage->getPixelsPtr());
	}
	outputPointer = newPointer;
}

void uiColorPicker::updatePositionsFromColor()
{
	float h, s, v;
	utils::rgb2hsv(*outputPointer, h, s, v);
	lastIntensityPos = (COLOR_WHEEL_SIZE) - v * (COLOR_WHEEL_SIZE);
	lastAlphaPos = (COLOR_WHEEL_SIZE) - outputPointer->a * (COLOR_WHEEL_SIZE) / 255.0f;
	float angle = h * nooseMath::DEG2RAD;
	lastColorPos.x = (COLOR_WHEEL_SIZE / 2.0f) + std::cos(angle) * s * COLOR_WHEEL_RADIUS * (COLOR_WHEEL_SIZE);
	lastColorPos.y = (COLOR_WHEEL_SIZE / 2.0f) - std::sin(angle) * s * COLOR_WHEEL_RADIUS * (COLOR_WHEEL_SIZE);
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

	while (const std::optional e = theWindow->pollEvent())
	{
		if (e->is<sf::Event::Closed>())
		{
			theWindow->close();
			onCloseWindowCallback();
		}
		else if (e->is<sf::Event::MouseButtonPressed>() && e->getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left)
		{
			mousePos = e->getIf<sf::Event::MouseButtonPressed>()->position;

			if (mousePos.x > INTENSITY_AND_ALPHA_WIDTH + MARGIN_WIDTH * 1.5 + COLOR_WHEEL_SIZE)
				selectionState = SelectionState::Alpha;
			else if (mousePos.x < MARGIN_WIDTH * 0.5 + COLOR_WHEEL_SIZE)
				selectionState = SelectionState::Color;
			else
				selectionState = SelectionState::Intensity;

			setColor();
		}
		else if (e->is<sf::Event::MouseButtonReleased>())
		{
			selectionState = SelectionState::None;
		}
		else if (e->is<sf::Event::MouseMoved>())
		{
			if (selectionState == SelectionState::None)
				return;
			mousePos = e->getIf<sf::Event::MouseMoved>()->position;
			setColor();
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
		sprt.setPosition({ 0.0, 0.0 });
		theWindow->draw(sprt);

		// colorwheel marker
		markerSprite->setPosition({ (float)(lastColorPos.x - 8), (float)(lastColorPos.y - 8) });
		theWindow->draw(*markerSprite);

		// intensity marker
		markerSprite->setPosition({ (float)(COLOR_WHEEL_SIZE + INTENSITY_AND_ALPHA_WIDTH * 0.5f - 8), (float)(lastIntensityPos - 8) });
		theWindow->draw(*markerSprite);

		// alpha marker
		markerSprite->setPosition({ (float)(COLOR_WHEEL_SIZE + INTENSITY_AND_ALPHA_WIDTH * 1.5f + MARGIN_WIDTH - 8), (float)(lastAlphaPos - 8) });
		theWindow->draw(*markerSprite);

		theWindow->display();
	}
}

void uiColorPicker::terminate()
{
	delete markerSprite;
	if (theWindow != nullptr)
	{
		theWindow->close();
		delete theWindow;
	}
}