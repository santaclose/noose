#include "uiData.h"
#include "../pathUtils.h"
#include <iostream>

sf::Font uiData::font;
sf::Font uiData::monospaceFont;

void uiData::load()
{
	if (!font.openFromFile(pathUtils::getAssetsDirectory() + "fonts/Lato/Lato-Regular.ttf"))
		std::cout << "[UI] Failed to load font\n";
	if (!monospaceFont.openFromFile(pathUtils::getAssetsDirectory() + "fonts/FiraCode/FiraCode-Regular.ttf"))
		std::cout << "[UI] Failed to laod monospace font\n";
}