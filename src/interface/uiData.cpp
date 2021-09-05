#include "uiData.h"
#include "../pathUtils.h"

sf::Font uiData::font;
sf::Font uiData::monospaceFont;

void uiData::load()
{
	font.loadFromFile(pathUtils::getAssetsDirectory() + "fonts/Lato/Lato-Regular.ttf");
	monospaceFont.loadFromFile(pathUtils::getAssetsDirectory() + "fonts/FiraCode/FiraCode-Regular.ttf");
}