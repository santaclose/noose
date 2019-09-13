#include "uiColorPicker.h"
#include <iostream>

sf::Color operator*(const sf::Color& c, const float f)
{
	return sf::Color(c.r * f, c.g * f, c.b * f, c.a);
}

namespace uiColorPicker
{
	void (*onSetColor)(sf::Color*) = nullptr;

	// private

	sf::RenderWindow* theWindow = nullptr;

	sf::Shader colorWheelVerticesShader;
	sf::VertexArray colorWheelVertices(sf::Quads, 4);
	sf::RenderTexture colorWheel;
	sf::Image colorWheelImage; // copy to get pixel colors

	sf::Shader gradientVerticesShader;
	sf::VertexArray gradientVertices(sf::Quads, 4);
	sf::RenderTexture gradient;
	sf::Image gradientImage;

	sf::Vector2u lastColorPos;
	float lastIntensity;
	sf::Color* outputPointer = nullptr;

	enum MouseState {
		None, ColorSide, IntensitySide
	};
	MouseState mouseState = MouseState::None;

	sf::Vector2u mousePos;

	void setColor()
	{
		if (outputPointer == nullptr || onSetColor == nullptr)
			return;

		if (mouseState == MouseState::ColorSide)
		{
			lastColorPos = sf::Vector2u(mousePos.x, 220u - mousePos.y);
		}
		else if (mouseState == MouseState::IntensitySide)
		{
			std::cout << "getting pixel " << mousePos.x - 220u << ", " << mousePos.y << std::endl;
			lastIntensity = gradientImage.getPixel(mousePos.x - 220u, mousePos.y).r;
			std::cout << lastIntensity << std::endl;
		}

		*outputPointer = colorWheelImage.getPixel(lastColorPos.x, lastColorPos.y) * lastIntensity;
		onSetColor(outputPointer);
	}
}


void uiColorPicker::initialize()
{
	colorWheelVerticesShader.loadFromFile("res/shaders/colorwheel.shader", sf::Shader::Fragment);
	colorWheelVertices[0].position.x = colorWheelVertices[0].position.y = colorWheelVertices[1].position.x = colorWheelVertices[3].position.y = 0.0;
	colorWheelVertices[2].position.x = colorWheelVertices[1].position.y = colorWheelVertices[3].position.x = colorWheelVertices[2].position.y = 220.0;
	colorWheelVertices[0].texCoords.x = colorWheelVertices[0].texCoords.y = colorWheelVertices[1].texCoords.x = colorWheelVertices[3].texCoords.y = 0.0;
	colorWheelVertices[2].texCoords.x = colorWheelVertices[1].texCoords.y = colorWheelVertices[3].texCoords.x = colorWheelVertices[2].texCoords.y = 1.0;

	colorWheel.create(220, 220);
	colorWheelVerticesShader.setUniform("limit", 0);
	colorWheel.draw(colorWheelVertices, &colorWheelVerticesShader);
	colorWheelImage = colorWheel.getTexture().copyToImage();
	colorWheelVerticesShader.setUniform("limit", 1);
	colorWheel.draw(colorWheelVertices, &colorWheelVerticesShader);

	gradientVerticesShader.loadFromFile("res/shaders/gradient.shader", sf::Shader::Fragment);
	gradientVertices[0].position.x = gradientVertices[1].position.x = 220.0;
	gradientVertices[2].position.x = gradientVertices[3].position.x = 260.0;
	gradientVertices[0].position.y = gradientVertices[3].position.y = 0.0;
	gradientVertices[1].position.y = gradientVertices[2].position.y = 220.0;
	gradientVertices[0].texCoords.y = gradientVertices[3].texCoords.y = 1.0;
	gradientVertices[1].texCoords.y = gradientVertices[2].texCoords.y = 0.0;

	gradient.create(40, 220);
	gradient.draw(gradientVertices, &gradientVerticesShader);
	gradientImage = gradient.getTexture().copyToImage();
}

void uiColorPicker::show(sf::Color* newPointer)
{
	if (theWindow != nullptr)
	{
		theWindow->requestFocus();
	}
	else // initial case
	{
		theWindow = new sf::RenderWindow(sf::VideoMode(260, 220), "color", sf::Style::Close);
	}
	outputPointer = newPointer;
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
				break;
			}
			case sf::Event::MouseButtonPressed:
			{
				if (e.mouseButton.button == sf::Mouse::Left)
				{
					//mousePressed = true;
					if (e.mouseButton.x > 220)
						mouseState = MouseState::IntensitySide;
					else
						mouseState = MouseState::ColorSide;

					mousePos = sf::Vector2u(e.mouseButton.x, e.mouseButton.y);
					setColor();
				}
				break;
			}
			case sf::Event::MouseButtonReleased:
			{
				mouseState = MouseState::None;
				break;
			}
			case sf::Event::MouseMoved:
			{
				if (mouseState == MouseState::None)
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
		theWindow->draw(colorWheelVertices, &colorWheelVerticesShader);
		theWindow->draw(gradientVertices, &gradientVerticesShader);
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