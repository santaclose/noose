#include "uiHelp.h"

#include "uiData.h"

#include "../math/uiMath.h"
#include "../math/vectorOperators.h"
#include "../utils.h"

#include <iostream>
#include <fstream>

#include "serializer.h"

#define BUTTON_RADIUS 0.46f // uv space
#define BUTTON_COLOR 0x5a5a5aff
#define BUTTON_SIZE 50
#define MARGIN 18

#define INSTRUCTIONS_BOX_COLOR 0x3a3a3ae0

sf::RenderWindow* hlpRenderWindow;
const sf::Vector2i* hlpMouseScreenPosPointer;
sf::VertexArray helpButtonVA;
sf::Text hlpQuestionMark;
sf::Shader floatingButtonShader;

inline void openHTMLFile()
{
	std::cout << "[Help] Opening help HTML\n";
	std::string command;
	std::string localPath;
#ifdef NOOSE_PLATFORM_LINUX
	command = "xdg-open ";
	localPath = "help/help.html";
	//system("xdg-open " + utils::getProgramDirectory() + "help/help.html");
#endif
#ifdef NOOSE_PLATFORM_MACOS
	command = "open ";
	localPath = "help/help.html";
	//system("open " + utils::getProgramDirectory() + "help/help.html");
#endif
#ifdef NOOSE_PLATFORM_WINDOWS
	command = "start \"\" ";
	localPath = "help\\help.html";
	//system("start " + utils::getProgramDirectory() + "help\\help.html");
#endif
	command += "\"" + utils::getProgramDirectory() + localPath + "\"";
	system(command.c_str());
}

void uiHelp::initialize(sf::RenderWindow& window, const sf::Vector2i* mouseScreenPosPointer)
{
	if (!floatingButtonShader.loadFromFile(utils::getProgramDirectory() + "assets/shaders/floatingButton.shader", sf::Shader::Fragment))
		std::cout << "[UI] Failed to load floating button shader\n";
	floatingButtonShader.setUniform("radius", BUTTON_RADIUS);

	hlpRenderWindow = &window;
	hlpMouseScreenPosPointer = mouseScreenPosPointer;

	helpButtonVA = sf::VertexArray(sf::Quads, 4);
	helpButtonVA[0].color = helpButtonVA[1].color = helpButtonVA[2].color = helpButtonVA[3].color = sf::Color(BUTTON_COLOR);

	helpButtonVA[0].texCoords.x = helpButtonVA[1].texCoords.x = helpButtonVA[0].texCoords.y = helpButtonVA[3].texCoords.y = 0.0;
	helpButtonVA[2].texCoords.x = helpButtonVA[3].texCoords.x = helpButtonVA[1].texCoords.y = helpButtonVA[2].texCoords.y = 1.0;

	helpButtonVA[0].position.x = helpButtonVA[1].position.x = MARGIN;
	helpButtonVA[2].position.x = helpButtonVA[3].position.x = MARGIN + BUTTON_SIZE;
	helpButtonVA[0].position.y = helpButtonVA[3].position.y = hlpRenderWindow->getSize().y - BUTTON_SIZE - MARGIN;
	helpButtonVA[1].position.y = helpButtonVA[2].position.y = hlpRenderWindow->getSize().y - MARGIN;

	hlpQuestionMark = sf::Text(sf::String("?"), uiData::font, 30);
	hlpQuestionMark.setPosition(
		MARGIN + BUTTON_SIZE / 2.0 - hlpQuestionMark.getLocalBounds().width / 2.0,
		hlpRenderWindow->getSize().y - MARGIN - BUTTON_SIZE / 2.0 - hlpQuestionMark.getLocalBounds().height / 1.2);
}

void uiHelp::terminate()
{
}

void uiHelp::onPollEvent(const sf::Event& e)
{
	switch (e.type)
	{
	case sf::Event::Resized:
		helpButtonVA[0].position.y = helpButtonVA[3].position.y = e.size.height - BUTTON_SIZE - MARGIN;
		helpButtonVA[1].position.y = helpButtonVA[2].position.y = e.size.height - MARGIN;
		hlpQuestionMark.setPosition(hlpQuestionMark.getPosition().x, e.size.height - MARGIN - BUTTON_SIZE / 2.0 - hlpQuestionMark.getLocalBounds().height / 1.2);
		break;
	case sf::Event::MouseButtonPressed:
	{
		if (e.mouseButton.button != sf::Mouse::Left)
			break;

		sf::Vector2f mousePosInUVSpace =
			(sf::Vector2f(hlpMouseScreenPosPointer->x, hlpMouseScreenPosPointer->y) - helpButtonVA[0].position) /
			BUTTON_SIZE;
		if (uiMath::distance(sf::Vector2f(0.5, 0.5), mousePosInUVSpace) < BUTTON_RADIUS)
			openHTMLFile();
		break;
	}
	}
}

void uiHelp::draw()
{
	sf::FloatRect visibleArea(0, 0, hlpRenderWindow->getSize().x, hlpRenderWindow->getSize().y);
	hlpRenderWindow->setView(sf::View(visibleArea));
	sf::Vector2f mousePos = sf::Vector2f(hlpMouseScreenPosPointer->x, hlpMouseScreenPosPointer->y);
	hlpRenderWindow->draw(helpButtonVA, &floatingButtonShader);
	hlpRenderWindow->draw(hlpQuestionMark);
}