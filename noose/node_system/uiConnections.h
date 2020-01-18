#pragma once
#include <SFML/Graphics.hpp>

class uiConnections
{
private:
	static std::vector<sf::Vertex> lineQuads;
	static std::vector<sf::Vector2f> linePoints;
	static sf::Shader shader;

	static bool startedOnInputPinb;

private:
	static void updateLineQuads(int lineIndex);
	static void set(sf::Vector2f& pinPosA, const sf::Vector2f& pinPosB, const sf::Color& color, int index);
	static void set(sf::Vector2f&& pinPosA, const sf::Vector2f& pinPosB, const sf::Color& color, int index);

public:
	static void initialize();

	static void createTemporary(sf::Vector2f&& pinPos, const sf::Vector2f& mousePos, const sf::Color& color, bool startedOnInputPin);

	static void displacePoint(const sf::Vector2f& displacement, int connectionIndex, bool isOutputPin);
	static void displaceTemporary(sf::Vector2f&& displacement);

	static void connect(sf::Vector2f&& pinPosB, int index);

	static void hide(int index);
	static void hideTemporary();

	static void updateShaderUniform(float zoom);
	static void draw(sf::RenderWindow& window);

	static int onTryingToRemove(const sf::Vector2f& mousePos);
};