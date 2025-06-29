#include "uiSearchBar.h"
#include "uiNodeSystem.h"
#include "uiData.h"

#include "../searcher.h"
#include "../nodeData.h"
#include "../math/nooseMath.h"
#include "../pathUtils.h"

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

namespace uiSearchBar {

	sf::RenderWindow* renderWindow;
	const sf::Vector2i* mouseScreenPosPointer;

	bool searching = false;
	sf::RectangleShape searchRectangle;
	sf::Text searchText(uiData::font, "");
	char searchBuffer[SEARCH_BAR_BUFFER_SIZE];
	int searchBufferCurrentChar = 0;

	int selectedSearchResult = 0;
	sf::VertexArray resultsVA;
	std::vector<sf::Text> resultsTexts;
	sf::Shader resultBoxShader;

	int currentResultCount = 0;

	void performSearch();
	void clearSearch();
	void pushSelectedNode(uiNodeSystem::PushMode mode = uiNodeSystem::PushMode::AtCursorPosition);
}

void uiSearchBar::performSearch()
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

void uiSearchBar::clearSearch()
{
	searching = false;
	searchBuffer[0] = '\0';
	searchBufferCurrentChar = 0;
	searchText.setString(searchBuffer);
}

void uiSearchBar::pushSelectedNode(uiNodeSystem::PushMode mode)
{
	const nodeData* nData = searcher::getDataFor(selectedSearchResult);
	if (nData == nullptr)
	{
		std::cout << "[UI] Failed to get node data\n";
		return;
	}

	uiNodeSystem::pushNewNode(nData, mode);

	clearSearch();
}

void uiSearchBar::initialize(sf::RenderWindow& window, const sf::Vector2i* mouseScreenPosPointer)
{
	if (!resultBoxShader.loadFromFile(pathUtils::getAssetsDirectory() + "shaders/searchResults.shader", sf::Shader::Type::Fragment))
		std::cout << "[UI] Failed to load search results shader\n";

	searchRectangle = sf::RectangleShape(sf::Vector2f(SEARCH_BAR_WIDTH, SEARCH_BAR_HEIGHT));
	searchRectangle.setPosition({ window.getSize().x / 2.0f - SEARCH_BAR_WIDTH / 2.0f, 0.0f });
	searchRectangle.setFillColor(sf::Color(SEARCH_BAR_COLOR));

	resultsVA = sf::VertexArray(sf::PrimitiveType::TriangleFan, 4);
	resultsVA[0].color = resultsVA[1].color = resultsVA[2].color = resultsVA[3].color = sf::Color(RESULTS_BAR_COLOR);
	resultsVA[0].position.x = resultsVA[1].position.x = window.getSize().x / 2.0 - SEARCH_BAR_WIDTH / 2.0;
	resultsVA[2].position.x = resultsVA[3].position.x = window.getSize().x / 2.0 + SEARCH_BAR_WIDTH / 2.0;
	resultsVA[0].position.y = resultsVA[3].position.y = SEARCH_BAR_HEIGHT;
	resultsVA[1].position.y = resultsVA[2].position.y = SEARCH_BAR_HEIGHT + MAX_RESULTS_NUMBER * RESULT_HEIGHT;
	resultsVA[0].texCoords.y = resultsVA[3].texCoords.y = 0.0;
	resultsVA[1].texCoords.y = resultsVA[2].texCoords.y = 1.0;

	searchText.setFillColor(sf::Color::White);
	searchText.setFont(uiData::font);
	searchText.setPosition({
		(float)(window.getSize().x / 2.0f - SEARCH_BAR_WIDTH / 2.0f + SEARCH_BAR_TEXT_MARGIN),
		(float)SEARCH_BAR_TEXT_MARGIN });
	searchText.setCharacterSize(SEARCH_BAR_FONT_SIZE);
	searchBuffer[0] = '\0';

	// set result texts positions
	resultsTexts.reserve(MAX_RESULTS_NUMBER);
	for (int i = 0; i < MAX_RESULTS_NUMBER; i++)
	{
		resultsTexts.push_back(sf::Text(uiData::font, "", RESULT_FONT_SIZE));
		resultsTexts[i].setFillColor(sf::Color::White);
		resultsTexts[i].setPosition({
			(float)(window.getSize().x / 2.0f - SEARCH_BAR_WIDTH / 2.0f + SEARCH_BAR_TEXT_MARGIN),
			(float)(SEARCH_BAR_HEIGHT + SEARCH_BAR_TEXT_MARGIN + RESULT_HEIGHT * i) });
	}

	renderWindow = &window;
	uiSearchBar::mouseScreenPosPointer = mouseScreenPosPointer;
}

void uiSearchBar::onSpacebarPressed()
{
	searching = true;
	performSearch();
}

void uiSearchBar::onPollEvent(const std::optional<sf::Event>& e)
{
	if (e->is<sf::Event::KeyPressed>())
	{
		sf::Keyboard::Key keyCode = e->getIf<sf::Event::KeyPressed>()->code;
		if (keyCode == sf::Keyboard::Key::Escape)
		{
			clearSearch();
		}
		else if (keyCode == sf::Keyboard::Key::Enter)
		{
			if (searcher::searchResults.size() > 0)
				pushSelectedNode();
		}
		else if (keyCode == sf::Keyboard::Key::Down)
		{
			if (selectedSearchResult < currentResultCount - 1)
			{
				selectedSearchResult++;
				resultBoxShader.setUniform("sel", (float) selectedSearchResult);
			}
		}
		else if (keyCode == sf::Keyboard::Key::Up)
		{
			if (selectedSearchResult > 0)
			{
				selectedSearchResult--;
				resultBoxShader.setUniform("sel", (float) selectedSearchResult);
			}
		}
		else if (keyCode == sf::Keyboard::Key::Backspace && e->getIf<sf::Event::KeyPressed>()->control)
		{
			int i = searchBufferCurrentChar > 0 ? searchBufferCurrentChar - 1 : 0;
			for (; i > 0 && searchBuffer[i - 1] != ' '; i--);
			searchBuffer[i] = '\0';
			searchBufferCurrentChar = i;
			searchText.setString(searchBuffer);

			// search logic
			performSearch();
		}
	}
	else if (e->is<sf::Event::TextEntered>())
	{
		char32_t unicode = e->getIf<sf::Event::TextEntered>()->unicode;
		if (unicode >= 128)
		{
			std::cout << "[UI] Non ASCII character typed on the search bar\n";
			return;
		}
		switch (unicode)
		{
		case 127:
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
			if (unicode == ' ' && searchBuffer[0] == '\0')
				return;
			searchBuffer[searchBufferCurrentChar] = unicode;
			searchBuffer[searchBufferCurrentChar + 1] = '\0';
			searchBufferCurrentChar++;
			break;
		}

		searchText.setString(searchBuffer);

		// search logic
		performSearch();
	}
	else if (e->is<sf::Event::Resized>())
	{
		sf::Vector2u newSize = e->getIf<sf::Event::Resized>()->size;
		searchRectangle.setPosition({ newSize.x / 2.0f - SEARCH_BAR_WIDTH / 2.0f, 0.0f });
		searchText.setPosition({
			(float)(newSize.x / 2.0 - SEARCH_BAR_WIDTH / 2.0 + SEARCH_BAR_TEXT_MARGIN),
			(float)SEARCH_BAR_TEXT_MARGIN });

		resultsVA[0].position.x = resultsVA[1].position.x = newSize.x / 2.0 - SEARCH_BAR_WIDTH / 2.0;
		resultsVA[2].position.x = resultsVA[3].position.x = newSize.x / 2.0 + SEARCH_BAR_WIDTH / 2.0;

		for (int i = 0; i < MAX_RESULTS_NUMBER; i++)
		{
			resultsTexts[i].setPosition({
				(float)(newSize.x / 2.0 - SEARCH_BAR_WIDTH / 2.0 + SEARCH_BAR_TEXT_MARGIN),
				(float)(SEARCH_BAR_HEIGHT + SEARCH_BAR_TEXT_MARGIN + RESULT_HEIGHT * i) });
		}
	}
	else if (e->is<sf::Event::MouseMoved>())
	{
		sf::Vector2i eventMousePos = e->getIf<sf::Event::MouseMoved>()->position;
		sf::Vector2f mousePos = sf::Vector2f(eventMousePos.x, eventMousePos.y);
		if (nooseMath::isPointInsideRect(mousePos, resultsVA[0].position, resultsVA[2].position))
		{
			selectedSearchResult = (int)((mousePos - resultsVA[0].position).y / RESULT_HEIGHT);
			resultBoxShader.setUniform("sel", (float)selectedSearchResult);
		}
	}
	else if (
		e->is<sf::Event::MouseButtonPressed>() &&
		e->getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left &&
		searching)
	{
		if (nooseMath::isPointInsideRect((sf::Vector2f) * mouseScreenPosPointer, resultsVA[0].position, resultsVA[2].position))
			pushSelectedNode(uiNodeSystem::PushMode::Centered);
		else
			clearSearch();
	}
}

void uiSearchBar::draw()
{
	if (searching)
	{
		sf::FloatRect visibleArea({ 0.0f, 0.0f }, { (float)renderWindow->getSize().x, (float)renderWindow->getSize().y });
		renderWindow->setView(sf::View(visibleArea));
		renderWindow->draw(searchRectangle);
		renderWindow->draw(searchText);
		renderWindow->draw(resultsVA, &resultBoxShader);
		for (int i = 0; i < currentResultCount; i++)
		{
			renderWindow->draw(resultsTexts[i]);
		}
	}
}

bool uiSearchBar::isActive()
{
	return searching;
}