#pragma once
#include <SFML/Graphics.hpp>

class uiSelectionBox
{
public:
	enum DisplayMode
	{
		TopLeftCorner = 0, BottomLeftCorner = 1, BottomRightCorner = 2, TopRightCorner = 3
	};

private:
	bool m_onScreen = false;
	std::vector<sf::Vertex> m_box;
	std::vector<sf::Text*> m_optionTexts;
	int m_currentOptionCount = 0;
	// need different shaders per object to set different boxcounts
	sf::Shader m_mouseOverShader;

public:
	void initialize();
	void display(const sf::Vector2f& position, const std::vector<std::string>& options, DisplayMode mode = DisplayMode::TopLeftCorner);
	int mouseOver(const sf::Vector2f& position);
	void hide();
	void draw(sf::RenderWindow& window, const sf::Vector2f& mousePos);
	void terminate();

	int getBoxHeight() const;
	int getBoxWidth() const;
	inline const sf::Vector2f& getPosition() const
	{
		return m_box[0].position;
	}
	inline bool isVisible() const
	{
		return m_onScreen;
	}
};