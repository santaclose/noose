#pragma once
#include <SFML/Graphics.hpp>

enum MouseState {
	None, ColorSide, IntensitySide
};

class uiColorPicker
{
public:
	static void (*onSetColor)(sf::Color*);

private:
	static sf::RenderStates rs;

	static sf::RenderWindow* theWindow;

	static sf::Shader colorWheelVerticesShader;
	static sf::VertexArray colorWheelVertices;
	static sf::RenderTexture colorWheel;
	static sf::Image colorWheelImage; // copy to get pixel colors

	static sf::Shader gradientVerticesShader;
	static sf::VertexArray gradientVertices;
	static sf::RenderTexture gradient;
	static sf::Image gradientImage;

	static sf::Vector2u lastColorPos;
	static float lastIntensity;
	static sf::Color* outputPointer;

	static MouseState mouseState;

	static sf::Vector2u mousePos;

public:
	static void initialize();
	static void tick();
	static void terminate();
	static void show(sf::Color* newPointer);

private:
	static void setColor();
};