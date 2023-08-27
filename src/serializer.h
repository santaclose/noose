#pragma once
#include <string>

class serializer {
public:

	struct ParsingCallbacks {
		void (*OnStart)(void);
		void (*OnAddNode)(const std::string& nodeName, float coordinatesX, float coordinatesY);
		void (*OnSetNodeInput)(int nodeIndex, int pinIndex, void* data, int flags);
		void (*OnAddConnection)(int nodeAIndex, int pinAIndex, int nodeBIndex, int pinBIndex);
		void (*OnSetNodeEditorState)(int selectedNode, int nodeEditorZoom,
			float nodeEditorViewPositionX, float nodeEditorViewPositionY);
		void (*OnSetViewportState)(int viewportZoom,
			float viewportViewPositionX, float viewportViewPositionY);
	};

	static void LoadFromFile(const std::string& filePath, const ParsingCallbacks& callbacks);
	static void SaveIntoFile(const std::string& filePath);
	static void LoadFromFileJson(const std::string& filePath, const ParsingCallbacks& callbacks);
	static void SaveIntoFileJson(const std::string& filePath);
};