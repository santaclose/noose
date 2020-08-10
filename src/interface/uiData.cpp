#include "uiData.h"

sf::Font uiData::font;
sf::Font uiData::monospaceFont;

void uiData::load()
{
	font.loadFromFile("assets/fonts/Lato/Lato-Regular.ttf");
	monospaceFont.loadFromFile("assets/fonts/FiraCode/FiraCode-Regular.ttf");
}