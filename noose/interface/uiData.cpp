#include "uiData.h"

sf::Font uiData::font;

void uiData::load()
{
	font.loadFromFile("res/fonts/Lato/Lato-Regular.ttf");
}