#pragma once
#include <SFML/Graphics.hpp>

namespace uiFloatingButtonLayer {

	extern bool active;
	enum class ButtonPosition { TopLeft = 0, TopRight = 1, BottomLeft = 2, BottomRight = 3, None = -1};
	struct FloatingButton {
		ButtonPosition pos;
		sf::Shader* customShader;
		sf::Text* text;
		sf::VertexArray va;
	};

	void updateButtonElementPositions(FloatingButton& fb);

	void initialize(sf::RenderWindow& window, const sf::Vector2i* mouseScreenPosPointer);
	void addButton(ButtonPosition position, const std::string& customShaderPath);
	void addButton(ButtonPosition position, char symbol, const std::string* customShaderPath = nullptr);
	void draw();
	sf::Vector2f getButtonCenterCoords(ButtonPosition pos);
	void terminate();

	ButtonPosition onPollEvent(const sf::Event& e);
}