#include "uiColorPicker.h"
#include <iostream>
#include <cstdint>

void (*uiColorPicker::onSetColor)(sf::Color*);
sf::RenderStates uiColorPicker::rs;
sf::RenderWindow* uiColorPicker::theWindow = nullptr;
sf::Shader uiColorPicker::colorWheelVerticesShader;
sf::VertexArray uiColorPicker::colorWheelVertices = sf::VertexArray(sf::Quads, 4);
sf::RenderTexture uiColorPicker::colorWheel;
sf::Image uiColorPicker::colorWheelImage; // copy to get pixel colors
sf::Shader uiColorPicker::gradientVerticesShader;
sf::VertexArray uiColorPicker::gradientVertices = sf::VertexArray(sf::Quads, 4);
sf::RenderTexture uiColorPicker::gradient;
sf::Image uiColorPicker::gradientImage;
sf::Vector2u uiColorPicker::lastColorPos;
float uiColorPicker::lastIntensity = 1.0;
sf::Color* uiColorPicker::outputPointer = nullptr;
MouseState uiColorPicker::mouseState = MouseState::None;
sf::Vector2u uiColorPicker::mousePos;

sf::Color operator*(const sf::Color& c, const float f)
{
	return sf::Color(c.r * f, c.g * f, c.b * f, c.a);
}

void uiColorPicker::initialize()
{
	rs.blendMode = sf::BlendNone;

	if (!colorWheelVerticesShader.loadFromFile("res/shaders/colorwheel.shader", sf::Shader::Fragment))
		std::cout << "[UI] Failed to load colorwheel shader\n";
	colorWheelVertices[0].position.x = colorWheelVertices[0].position.y = colorWheelVertices[1].position.x = colorWheelVertices[3].position.y = 0.0;
	colorWheelVertices[2].position.x = colorWheelVertices[1].position.y = colorWheelVertices[3].position.x = colorWheelVertices[2].position.y = 220.0;
	colorWheelVertices[0].texCoords.x = colorWheelVertices[0].texCoords.y = colorWheelVertices[1].texCoords.x = colorWheelVertices[3].texCoords.y = 0.0;
	colorWheelVertices[2].texCoords.x = colorWheelVertices[1].texCoords.y = colorWheelVertices[3].texCoords.x = colorWheelVertices[2].texCoords.y = 1.0;

	colorWheel.create(220, 220);
	colorWheelVerticesShader.setUniform("limit", 0.0f);
	rs.shader = &colorWheelVerticesShader;
	colorWheel.draw(colorWheelVertices, rs);
	colorWheelImage = colorWheel.getTexture().copyToImage();
	colorWheelVerticesShader.setUniform("limit", 1.0f);
	colorWheel.draw(colorWheelVertices, rs);

	if (!gradientVerticesShader.loadFromFile("res/shaders/gradient.shader", sf::Shader::Fragment))
		std::cout << "[UI] Failed to load gradient shader\n";
	gradientVertices[0].position.x = gradientVertices[1].position.x = gradientVertices[0].position.y = gradientVertices[3].position.y = 0.0;
	gradientVertices[2].position.x = gradientVertices[3].position.x = 40.0;
	gradientVertices[1].position.y = gradientVertices[2].position.y = 220.0;
	gradientVertices[0].texCoords.y = gradientVertices[3].texCoords.y = 0.0;
	gradientVertices[1].texCoords.y = gradientVertices[2].texCoords.y = 1.0;

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
		sf::Sprite cw(colorWheel.getTexture());
		sf::Sprite in(gradient.getTexture());
		cw.setPosition(0.0, 0.0);
		in.setPosition(220.0, 0.0);
		theWindow->draw(cw);
		theWindow->draw(in);

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

void uiColorPicker::setColor()
{
	if (outputPointer == nullptr || onSetColor == nullptr)
		return;

	if (mouseState == MouseState::ColorSide)
	{
		lastColorPos = sf::Vector2u(mousePos.x, mousePos.y);
	}
	else if (mouseState == MouseState::IntensitySide)
	{
		lastIntensity = gradientImage.getPixel(20, mousePos.y).r / 255.0;
		//std::cout << "selected intensity: " << lastIntensity << std::endl;
	}

	*outputPointer = colorWheelImage.getPixel(lastColorPos.x, lastColorPos.y) * lastIntensity;
	onSetColor(outputPointer);
}