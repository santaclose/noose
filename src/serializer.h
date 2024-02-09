#pragma once
#include <string>

class serializer {
public:

	struct ParsingCallbacks {
		void (*OnStartParsing)(void) = nullptr;
		void (*OnParseNode)(const std::string& nodeName, float coordinatesX, float coordinatesY) = nullptr;
		void (*OnParseNodeInput)(int nodeIndex, int pinIndex, void* data, int flags) = nullptr;
		void (*OnParseConnection)(int nodeAIndex, int pinAIndex, int nodeBIndex, int pinBIndex) = nullptr;
		void (*OnParseNodeEditorState)(int selectedNode, int nodeEditorZoom,
			float nodeEditorViewPositionX, float nodeEditorViewPositionY) = nullptr;
		void (*OnParseViewportState)(int viewportZoom,
			float viewportViewPositionX, float viewportViewPositionY) = nullptr;
		void (*OnParseCustomNodeInput)(int inPin, int subgraphNode, int subgraphPin) = nullptr;
		void (*OnParseCustomNodeOutput)(int outPin, int subgraphNode, int subgraphPin) = nullptr;
		void (*OnFinishParsing)(void) = nullptr;
	};

	static void LoadFromFile(const std::string& filePath, const ParsingCallbacks& callbacks);
	static void SaveIntoFile(const std::string& filePath);
	static void LoadFromFileJson(const std::string& filePath, const ParsingCallbacks& callbacks);
	static void SaveIntoFileJson(const std::string& filePath);
};