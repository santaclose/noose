#include "uiData.h"

sf::Font uiData::font;
sf::Font uiData::monospaceFont;

void uiData::load()
{
	font.loadFromFile("res/fonts/Lato/Lato-Regular.ttf");
	monospaceFont.loadFromFile("res/fonts/FiraCode/FiraCode-Regular.ttf");
}