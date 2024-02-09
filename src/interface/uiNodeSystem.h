#pragma once

#include <SFML/Graphics.hpp>
#include "../nodeData.h"
#include "uiNode.h"

namespace uiNodeSystem
{
	enum class PushMode { AtCursorPosition = 0, Centered = 1, AtPosition = 2 };

	void initialize(sf::RenderWindow& theRenderWindow, const sf::Vector2i* mouseScreenPosPointer);
	void terminate();

	int pushNewNode(
		const nodeData* nData,
		PushMode mode = PushMode::AtCursorPosition,
		bool nodeCenterInPosition = true,
		sf::Vector2f worldPos = { 0.0, 0.0 });

	int pushImageNodeFromFile(
		const std::string& filePath,
		PushMode mode = PushMode::AtCursorPosition,
		bool nodeCenterInPosition = true,
		sf::Vector2f worldPos = { 0.0, 0.0 });
	int pushFontNodeFromFile(
		const std::string& filePath,
		PushMode mode = PushMode::AtCursorPosition,
		bool nodeCenterInPosition = true,
		sf::Vector2f worldPos = { 0.0, 0.0 });

	void onPollEvent(const sf::Event& e);
	void draw();

	void setOnNodeSelectedCallback(void (*functionPointer)(int));
	void setOnNodeDeletedCallback(void (*functionPointer)(int)); // called just before deleting the node
	void setOnNodeChangedCallback(void (*functionPointer)(int));

	std::vector<uiNode*>& getNodeList();
	int getSelectedNode();
	void setSelectedNode(int nodeIndex);
	void getView(int& zoom, sf::Vector2f& position);
	void setView(int zoom, const sf::Vector2f& position);

	void setBoundInputFieldNode(int node);

	void clearEverything();
	void createConnection(int leftNode, int rightNode, int leftPin, int rightPin);

	void clearNodeSelection();
	bool isEmpty();

	void copyNode();
	bool pasteNode();

	int getLogicalNodeId(int uiNodeId);

	// project file loading
	void onProjectFileLoadingStart();
	void onProjectFileLoadingAddNode(const std::string& nodeName, float coordinatesX, float coordinatesY);
	void onProjectFileLoadingSetNodeInput(int nodeIndex, int pinIndex, void* data, int flags = 0);
	void onProjectFileLoadingAddConnection(int nodeAIndex, int pinAIndex, int nodeBIndex, int pinBIndex);
	void onProjectFileLoadingSetEditorState(int selectedNode, int nodeEditorZoom,
			float nodeEditorViewPositionX, float nodeEditorViewPositionY);
}