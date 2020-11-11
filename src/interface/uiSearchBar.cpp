#include "uiSearchBar.h"
#include "uiNodeSystem.h"
#include "uiData.h"

#include "../searcher.h"
#include "../nodeData.h"
#include "../math/uiMath.h"
#include "../utils.h"

#include <iostream>

#define SEARCH_BAR_BUFFER_SIZE 31
#define SEARCH_BAR_WIDTH 400
#define SEARCH_BAR_HEIGHT 40
#define SEARCH_BAR_COLOR 0x323232ff
#define SEARCH_BAR_FONT_SIZE 16
#define SEARCH_BAR_TEXT_MARGIN 8
#define MAX_RESULTS_NUMBER 7
#define RESULTS_BAR_COLOR 0x383838ff
#define RESULT_HEIGHT 36
#define RESULT_FONT_SIZE 14

sf::RenderWindow* sbRenderWindow;
const sf::Vector2i* sbMouseScreenPosPointer;

bool searching = false;
sf::RectangleShape searchRectangle;
sf::Text searchText;
char searchBuffer[SEARCH_BAR_BUFFER_SIZE];
int searchBufferCurrentChar = 0;

int selectedSearchResult = 0;
sf::VertexArray resultsVA;
sf::Text resultsTexts[MAX_RESULTS_NUMBER];
sf::Shader resultBoxShader;

int currentResultCount = 0;

void performSearch()
{
	currentResultCount = searcher::search(searchBuffer, SEARCH_BAR_BUFFER_SIZE, MAX_RESULTS_NUMBER);
	resultsVA[1].position.y = resultsVA[2].position.y = SEARCH_BAR_HEIGHT + currentResultCount * RESULT_HEIGHT;
	for (int i = 0; i < currentResultCount; i++)
	{
		resultsTexts[i].setString(*(searcher::searchResults[i]));
	}
	selectedSearchResult = 0;
	resultBoxShader.setUniform("count", (float)currentResultCount);
	resultBoxShader.setUniform("sel", 0.0f);
}

void clearSearch()
{
	searching = false;
	searchBuffer[0] = '\0';
	searchBufferCurrentChar = 0;
	searchText.setString(searchBuffer);
}

void pushSelectedNode()
{
	const nodeData* nData = searcher::getDataFor(selectedSearchResult);
	if (nData == nullptr)
	{
		std::cout << "[Search Bar] Failed to get node data\n";
		return;
	}

	uiNodeSystem::pushNewNode(nData, uiNodeSystem::PushMode::AtCursorPosition);

	clearSearch();
}

void uiSearchBar::initialize(sf::RenderWindow& window, const sf::Vector2i* mouseScreenPosPointer)
{
	if (!resultBoxShader.loadFromFile(utils::getProgramDirectory() + "assets/shaders/searchResults.shader", sf::Shader::Fragment))
		std::cout << "[UI] Failed to load search results shader\n";

	searchRectangle = sf::RectangleShape(sf::Vector2f(SEARCH_BAR_WIDTH, SEARCH_BAR_HEIGHT));
	searchRectangle.setPosition(window.getSize().x / 2.0 - SEARCH_BAR_WIDTH / 2.0, 0);
	searchRectangle.setFillColor(sf::Color(SEARCH_BAR_COLOR));

	resultsVA = sf::VertexArray(sf::Quads, 4);
	resultsVA[0].color = resultsVA[1].color = resultsVA[2].color = resultsVA[3].color = sf::Color(RESULTS_BAR_COLOR);
	resultsVA[0].position.x = resultsVA[1].position.x = window.getSize().x / 2.0 - SEARCH_BAR_WIDTH / 2.0;
	resultsVA[2].position.x = resultsVA[3].position.x = window.getSize().x / 2.0 + SEARCH_BAR_WIDTH / 2.0;
	resultsVA[0].position.y = resultsVA[3].position.y = SEARCH_BAR_HEIGHT;
	resultsVA[1].position.y = resultsVA[2].position.y = SEARCH_BAR_HEIGHT + MAX_RESULTS_NUMBER * RESULT_HEIGHT;
	resultsVA[0].texCoords.y = resultsVA[3].texCoords.y = 0.0;
	resultsVA[1].texCoords.y = resultsVA[2].texCoords.y = 1.0;

	searchText.setFillColor(sf::Color::White);
	searchText.setFont(uiData::font);
	searchText.setPosition(
		window.getSize().x / 2 - SEARCH_BAR_WIDTH / 2.0 + SEARCH_BAR_TEXT_MARGIN,
		SEARCH_BAR_TEXT_MARGIN);
	searchText.setCharacterSize(SEARCH_BAR_FONT_SIZE);
	searchBuffer[0] = '\0';

	// set result texts positions
	for (int i = 0; i < MAX_RESULTS_NUMBER; i++)
	{
		resultsTexts[i].setFillColor(sf::Color::White);
		resultsTexts[i].setFont(uiData::font);
		resultsTexts[i].setPosition(
			window.getSize().x / 2 - SEARCH_BAR_WIDTH / 2.0 + SEARCH_BAR_TEXT_MARGIN,
			SEARCH_BAR_HEIGHT + SEARCH_BAR_TEXT_MARGIN + RESULT_HEIGHT * i);
		resultsTexts[i].setCharacterSize(RESULT_FONT_SIZE);
	}

	sbRenderWindow = &window;
	sbMouseScreenPosPointer = mouseScreenPosPointer;
}

void uiSearchBar::onPollEvent(const sf::Event& e)
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
					pushSelectedNode();
			}
			else if (e.key.code == sf::Keyboard::Down)
			{
				if (selectedSearchResult < currentResultCount - 1)
				{
					selectedSearchResult++;
					resultBoxShader.setUniform("sel", (float) selectedSearchResult);
				}
			}
			else if (e.key.code == sf::Keyboard::Up)
			{
				if (selectedSearchResult > 0)
				{
					selectedSearchResult--;
					resultBoxShader.setUniform("sel", (float)selectedSearchResult);
				}
			}
		}
		else
		{
			if (e.key.code == sf::Keyboard::Space)
			{
				searching = true;
				performSearch();
			}
		}
	}
	else if (e.type == sf::Event::TextEntered)
	{
		if (searching) // type on search box
		{
			if (e.text.unicode >= 128)
			{
				std::cout << "[UI] Non ASCII character typed on the search bar\n";
				return;
			}
			switch (e.text.unicode)
			{
			case '\r':
				return;
			case '\b':
				if (searchBufferCurrentChar > 0)
				{
					searchBuffer[searchBufferCurrentChar - 1] = '\0';
					searchBufferCurrentChar--;
				}
				break;
			default:
				if (searchBufferCurrentChar == SEARCH_BAR_BUFFER_SIZE - 1)
					return;
				if (e.text.unicode == ' ' && searchBuffer[0] == '\0')
					return;
				searchBuffer[searchBufferCurrentChar] = e.text.unicode;
				searchBuffer[searchBufferCurrentChar + 1] = '\0';
				searchBufferCurrentChar++;
				break;
			}

			searchText.setString(searchBuffer);

			// search logic
			performSearch();
		}
	}
	else if (e.type == sf::Event::Resized)
	{
		searchRectangle.setPosition(e.size.width / 2.0 - SEARCH_BAR_WIDTH / 2.0, 0);
		searchText.setPosition(
			e.size.width / 2.0 - SEARCH_BAR_WIDTH / 2.0 + SEARCH_BAR_TEXT_MARGIN,
			SEARCH_BAR_TEXT_MARGIN);

		resultsVA[0].position.x = resultsVA[1].position.x = e.size.width / 2.0 - SEARCH_BAR_WIDTH / 2.0;
		resultsVA[2].position.x = resultsVA[3].position.x = e.size.width / 2.0 + SEARCH_BAR_WIDTH / 2.0;

		for (int i = 0; i < MAX_RESULTS_NUMBER; i++)
		{
			resultsTexts[i].setPosition(
				e.size.width / 2.0 - SEARCH_BAR_WIDTH / 2.0 + SEARCH_BAR_TEXT_MARGIN,
				SEARCH_BAR_HEIGHT + SEARCH_BAR_TEXT_MARGIN + RESULT_HEIGHT * i);
		}
	}
	else if (e.type == sf::Event::MouseMoved)
	{
		if (!searching)
			return;

		sf::Vector2f mousePos = sf::Vector2f(e.mouseMove.x, e.mouseMove.y);
		if (uiMath::isPointInsideRect(mousePos, resultsVA[0].position, resultsVA[2].position))
		{
			selectedSearchResult = (int)((mousePos - resultsVA[0].position).y / RESULT_HEIGHT);
			resultBoxShader.setUniform("sel", (float)selectedSearchResult);
		}
	}
	else if (e.type == sf::Event::MouseButtonPressed)
	{
		if (e.mouseButton.button != sf::Mouse::Left || !searching)
			return;

		if (uiMath::isPointInsideRect((sf::Vector2f) * sbMouseScreenPosPointer, resultsVA[0].position, resultsVA[2].position))
			pushSelectedNode();
		else
			clearSearch();
	}
}

void uiSearchBar::draw()
{
	if (searching)
	{
		sf::FloatRect visibleArea(0, 0, sbRenderWindow->getSize().x, sbRenderWindow->getSize().y);
		sbRenderWindow->setView(sf::View(visibleArea));
		sbRenderWindow->draw(searchRectangle);
		sbRenderWindow->draw(searchText);
		sbRenderWindow->draw(resultsVA, &resultBoxShader);
		for (int i = 0; i < currentResultCount; i++)
		{
			sbRenderWindow->draw(resultsTexts[i]);
		}
	}
}

bool uiSearchBar::isActive()
{
	return searching;
}