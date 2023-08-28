#pragma once
#include <string>

class serializer {
public:

	struct ParsingCallbacks {
		void (*OnStart)(void) = nullptr;
		void (*OnAddNode)(const std::string& nodeName, float coordinatesX, float coordinatesY) = nullptr;
		void (*OnSetNodeInput)(int nodeIndex, int pinIndex, void* data, int flags) = nullptr;
		void (*OnAddConnection)(int nodeAIndex, int pinAIndex, int nodeBIndex, int pinBIndex) = nullptr;
		void (*OnSetNodeEditorState)(int selectedNode, int nodeEditorZoom,
			float nodeEditorViewPositionX, float nodeEditorViewPositionY) = nullptr;
		void (*OnSetViewportState)(int viewportZoom,
			float viewportViewPositionX, float viewportViewPositionY) = nullptr;
	};

	static void LoadFromFile(const std::string& filePath, const ParsingCallbacks& callbacks);
	static void SaveIntoFile(const std::string& filePath);
	static void LoadFromFileJson(const std::string& filePath, const ParsingCallbacks& callbacks);
	static void SaveIntoFileJson(const std::string& filePath);
};