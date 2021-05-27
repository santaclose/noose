#pragma once
#include <SFML/Graphics.hpp>

#include <string>
#include <vector>

class uiMessageBox
{
private:
	bool m_onScreen = false;
	std::vector<sf::Vertex> m_box;
	sf::Text m_messageText;
	std::vector<sf::Text*> m_optionTexts;
	int m_currentOptionCount = 0;
	float m_buttonWidth;
	// need different shaders per object to set different button counts
	sf::Shader m_shader;

public:
	void initialize();
	void display(const std::string& message, const std::vector<std::string>& options, const sf::Vector2f& centerPos);
	int mouseOver(const sf::Vector2f& position);
	void hide();
	void draw(sf::RenderWindow& window, const sf::Vector2f& mousePos);
	void terminate();

	inline bool isVisible() const
	{
		return m_onScreen;
	}
};

