#include "uiSearchBar.h"
#include "uiNodeSystem.h"
#include "uiData.h"
//#include "../dataController.h"
#include "../searcher.h"

#include <iostream>

#define SEARCH_BAR_BUFFER_SIZE 31
#define SEARCH_BAR_WIDTH 400
#define SEARCH_BAR_HEIGHT 40
#define SEARCH_BAR_COLOR 0x323232ff
#define SEARCH_BAR_FONT_SIZE 16
#define SEARCH_BAR_TEXT_MARGIN 8
#define MAX_RESULTS_NUMBER 5
#define RESULTS_BAR_COLOR 0x383838ff
#define RESULT_HEIGHT 40
#define RESULT_FONT_SIZE 14

bool uiSearchBar::searching = false;
sf::RectangleShape uiSearchBar::searchRectangle;
sf::Text uiSearchBar::searchText;
char uiSearchBar::searchBuffer[SEARCH_BAR_BUFFER_SIZE];
int uiSearchBar::searchBufferCurrentChar = 0;

sf::RectangleShape uiSearchBar::resultsBar;
sf::Text uiSearchBar::resultsTexts[MAX_RESULTS_NUMBER];
int uiSearchBar::currentResultCount = 0;

sf::RenderWindow* uiSearchBar::renderWindow;

void uiSearchBar::initialize(sf::RenderWindow& window)
{
	searchRectangle = sf::RectangleShape(sf::Vector2f(SEARCH_BAR_WIDTH, SEARCH_BAR_HEIGHT));
	searchRectangle.setPosition(window.getSize().x / 2.0 - SEARCH_BAR_WIDTH / 2.0, 0);
	searchRectangle.setFillColor(sf::Color(SEARCH_BAR_COLOR));

	resultsBar = sf::RectangleShape(sf::Vector2f(SEARCH_BAR_WIDTH, MAX_RESULTS_NUMBER * RESULT_HEIGHT));
	resultsBar.setPosition(window.getSize().x / 2.0 - SEARCH_BAR_WIDTH / 2.0, SEARCH_BAR_HEIGHT);
	resultsBar.setFillColor(sf::Color(RESULTS_BAR_COLOR));

	searchText.setFillColor(sf::Color::White);
	searchText.setFont(uiData::font);
	searchText.setPosition(window.getSize().x / 2 - SEARCH_BAR_WIDTH / 2.0 + SEARCH_BAR_TEXT_MARGIN, SEARCH_BAR_TEXT_MARGIN);
	searchText.setCharacterSize(SEARCH_BAR_FONT_SIZE);
	searchBuffer[0] = '\0';

	// set result texts positions
	for (int i = 0; i < MAX_RESULTS_NUMBER; i++)
	{
		resultsTexts[i].setFillColor(sf::Color::White);
		resultsTexts[i].setFont(uiData::font);
		resultsTexts[i].setPosition(window.getSize().x / 2 - SEARCH_BAR_WIDTH / 2.0 + SEARCH_BAR_TEXT_MARGIN, SEARCH_BAR_HEIGHT + SEARCH_BAR_TEXT_MARGIN + RESULT_HEIGHT * i);
		resultsTexts[i].setCharacterSize(RESULT_FONT_SIZE);
	}
	renderWindow = &window;
}

void uiSearchBar::performSearch()
{
	//currentResultCount = dataController::search(searchBuffer, SEARCH_BAR_BUFFER_SIZE, MAX_RESULTS_NUMBER);
	currentResultCount = searcher::search(searchBuffer, SEARCH_BAR_BUFFER_SIZE, MAX_RESULTS_NUMBER);
	resultsBar.setSize(sf::Vector2f(SEARCH_BAR_WIDTH, currentResultCount * RESULT_HEIGHT));
	for (int i = 0; i < currentResultCount; i++)
	{
		//std::cout << *(dataController::searchResults[i]) << std::endl;
		resultsTexts[i].setString(*(searcher::searchResults[i]));
	}
}

void uiSearchBar::clearSearch()
{
	searching = false;
	searchBuffer[0] = '\0';
	searchBufferCurrentChar = 0;
	searchText.setString(searchBuffer);
}

void uiSearchBar::onPollEvent(const sf::Event& e, sf::Vector2i& mousePos) // mousePos in window coordinates
{
	if (e.type == sf::Event::KeyPressed)
	{
		if (searching)
		{
			if (e.key.code == sf::Keyboard::Escape)
			{
				clearSearch();
			}
			else if (e.key.code == sf::Keyboard::Enter)
			{
				if (searcher::searchResults.size() > 0)
				{
					void* nodeData = searcher::getDataFor(0);
					if (nodeData == nullptr)
					{
						std::cout << "could not get data\n";
						return;
					}
					uiNodeSystem::pushNewNode(nodeData, mousePos);

					clearSearch();
				}
			}
		}
		else
		{
			if (e.key.code == sf::Keyboard::Space)
			{
				uiNodeSystem::deselectNode();
				searching = true;
				performSearch();
			}
		}
	}
	else if (e.type == sf::Event::TextEntered)
	{
		if (searching) // type on search box
		{
			if (e.text.unicode == '\b')
			{
				if (searchBufferCurrentChar > 0)
				{
					searchBuffer[searchBufferCurrentChar - 1] = '\0';
					searchBufferCurrentChar--;
				}
			}
			else
			{
				if (searchBufferCurrentChar == SEARCH_BAR_BUFFER_SIZE - 1)
					return;
				if (e.text.unicode == ' ' && searchBuffer[0] == '\0')
					return;
				searchBuffer[searchBufferCurrentChar] = e.text.unicode;
				searchBuffer[searchBufferCurrentChar + 1] = '\0';
				searchBufferCurrentChar++;
			}
			//std::cout << searchBuffer << std::endl;
			searchText.setString(searchBuffer);

			// search logic
			performSearch();
		}
	}
	else if (e.type == sf::Event::Resized)
	{
		//sf::FloatRect visibleArea(0, 0, e.size.width, e.size.height);
		//sf::View previousView = theWindow->getView();
		//theWindow->setView(sf::View(visibleArea));
		searchRectangle.setPosition(e.size.width / 2.0 - SEARCH_BAR_WIDTH / 2.0, 0);
		searchText.setPosition(e.size.width / 2.0 - SEARCH_BAR_WIDTH / 2.0 + SEARCH_BAR_TEXT_MARGIN, SEARCH_BAR_TEXT_MARGIN);
		resultsBar.setPosition(e.size.width / 2.0 - SEARCH_BAR_WIDTH / 2.0, SEARCH_BAR_HEIGHT);
		for (int i = 0; i < MAX_RESULTS_NUMBER; i++)
		{
			resultsTexts[i].setPosition(e.size.width / 2.0 - SEARCH_BAR_WIDTH / 2.0 + SEARCH_BAR_TEXT_MARGIN, SEARCH_BAR_HEIGHT + SEARCH_BAR_TEXT_MARGIN + RESULT_HEIGHT * i);
		}
	}
}

void uiSearchBar::draw()
{
	if (searching)
	{
		//sf::View previousView = window.getView();
		sf::FloatRect visibleArea(0, 0, renderWindow->getSize().x, renderWindow->getSize().y);
		renderWindow->setView(sf::View(visibleArea));
		renderWindow->draw(searchRectangle);
		renderWindow->draw(searchText);
		renderWindow->draw(resultsBar);
		for (int i = 0; i < currentResultCount; i++)
		{
			renderWindow->draw(resultsTexts[i]);
		}

		//window.setView(previousView);
	}
}

bool uiSearchBar::userIsSearching()
{
	return searching;
}