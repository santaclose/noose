#include "uiData.h"
#include "../utils.h"

sf::Font uiData::font;
sf::Font uiData::monospaceFont;

void uiData::load()
{
	font.loadFromFile(utils::getProgramDirectory() + "assets/fonts/Lato/Lato-Regular.ttf");
	monospaceFont.loadFromFile(utils::getProgramDirectory() + "assets/fonts/FiraCode/FiraCode-Regular.ttf");
}