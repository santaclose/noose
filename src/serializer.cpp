#include "serializer.h"
#include <nodeProvider/nodeProvider.h>
#include <interface/uiNodeSystem.h>
#include <interface/uiViewport.h>
#include <interface/uiConnections.h>
#include <pathUtils.h>
#include <fstream>
#include <unordered_map>
#include "types.h"
#include "utils.h"

enum class LoadState { ReadingNodes = 0, ReadingConnections = 1, ReadingSelectedNode = 2, ReadingViews = 3, ReadingEmbeddedImages = 4 };
enum class LoadSubState { ReadingIds = 0, ReadingNodeCoordinates = 1, ReadingPinData = 2, ReadingConnectionNodes = 3, ReadingConnectionPins = 4 };

void serializer::LoadFromFile(const std::string& filePath, const ParsingCallbacks& callbacks)
{
	if (callbacks.OnStart != nullptr) callbacks.OnStart();
	std::string folderPath = pathUtils::getFolderPath(filePath);

	std::ifstream input(filePath);

	LoadState currentState;
	LoadSubState currentSubState;
	std::string currentNodeName;
	int currentPin;
	int currentNode;

	int currentConnectionLeftNode, currentConnectionRightNode;

	int selectedNode;
	int nodeEditorZoom, viewportZoom;
	sf::Vector2f nodeEditorViewPosition, viewportViewPosition;

	std::vector<int> embeddedImageNodes;
	std::vector<int> embeddedImagePins;
	int embeddedImagesLoaded = 0;

	std::string line;
	while (std::getline(input, line))
	{
		if (line.compare("| nodes") == 0)
		{
			currentState = LoadState::ReadingNodes;
			currentSubState = LoadSubState::ReadingIds;
			currentNode = 0;
			continue;
		}
		if (line.compare("| connections") == 0)
		{
			currentState = LoadState::ReadingConnections;
			currentSubState = LoadSubState::ReadingConnectionNodes;
			continue;
		}
		if (line.compare("| selected node") == 0)
		{
			currentState = LoadState::ReadingSelectedNode;
			continue;
		}
		if (line.compare("| views") == 0)
		{
			currentState = LoadState::ReadingViews;
			continue;
		}
		if (line.compare("| embedded images") == 0)
		{
			currentState = LoadState::ReadingEmbeddedImages;
			continue;
		}
		if (line[0] == '=')
		{
			if (currentState == LoadState::ReadingNodes)
			{
				currentNode++;
				currentSubState = LoadSubState::ReadingIds;
			}
			else
				currentSubState = LoadSubState::ReadingConnectionNodes;
			continue;
		}
		if (currentState == LoadState::ReadingNodes)
		{
			if (currentSubState == LoadSubState::ReadingIds)
			{
				currentNodeName = line;
				currentSubState = LoadSubState::ReadingNodeCoordinates;
				continue;
			}
			if (currentSubState == LoadSubState::ReadingNodeCoordinates)
			{
				int commaPos = 0;
				for (; line[commaPos] != ','; commaPos++) {}
				float coordsX = std::stof(line.substr(0, commaPos));
				float coordsY = std::stof(line.substr(commaPos + 1, line.size() - commaPos - 1));
				if (callbacks.OnAddNode != nullptr) callbacks.OnAddNode(currentNodeName, coordsX, coordsY);
				currentSubState = LoadSubState::ReadingPinData;
				currentPin = 0;
				continue;
			}
			if (currentSubState == LoadSubState::ReadingPinData)
			{
				switch (nodeProvider::getNodeDataByName(currentNodeName)->pinTypes[currentPin])
				{
				case NS_TYPE_COLOR:
				{
					sf::Color pinColor;
					int commaPos = 0;
					for (; line[commaPos] != ','; commaPos++) {}
					pinColor.r = std::stoi(line.substr(0, commaPos));
					int afterPrevComma = ++commaPos;
					for (; line[commaPos] != ','; commaPos++) {}
					pinColor.g = std::stoi(line.substr(afterPrevComma, commaPos - afterPrevComma));
					afterPrevComma = ++commaPos;
					for (; line[commaPos] != ','; commaPos++) {}
					pinColor.b = std::stoi(line.substr(afterPrevComma, commaPos - afterPrevComma));
					afterPrevComma = ++commaPos;
					pinColor.a = std::stoi(line.substr(afterPrevComma, line.length() - afterPrevComma));
					if (callbacks.OnSetNodeInput != nullptr) callbacks.OnSetNodeInput(-1, currentPin, &pinColor, 0);
					break;
				}
				case NS_TYPE_FLOAT:
				{
					float pinFloat = std::stof(line);
					if (callbacks.OnSetNodeInput != nullptr) callbacks.OnSetNodeInput(-1, currentPin, &pinFloat, 0);
					break;
				}
				case NS_TYPE_STRING:
					if (callbacks.OnSetNodeInput != nullptr) callbacks.OnSetNodeInput(-1, currentPin, &line, 0);
					break;
				case NS_TYPE_IMAGE:
				{
					if (line[0] == '1') // file path
					{
						std::string pinImagePath = folderPath + line.substr(2);
						if (callbacks.OnSetNodeInput != nullptr) callbacks.OnSetNodeInput(-1, currentPin, &pinImagePath, 0);
					}
					else if (line[0] == '2') // memory
					{
						embeddedImageNodes.push_back(currentNode);
						embeddedImagePins.push_back(currentPin);
					}
					// do nothing for none
					break;
				}
				case NS_TYPE_FONT:
				{
					if (line.compare("None") == 0)
						break;
					std::string pinFontPath = folderPath + line;
					if (callbacks.OnSetNodeInput != nullptr) callbacks.OnSetNodeInput(-1, currentPin, &pinFontPath, 0);
					break;
				}
				case NS_TYPE_INT:
				{
					int pinInt = std::stoi(line);
					if (callbacks.OnSetNodeInput != nullptr) callbacks.OnSetNodeInput(-1, currentPin, &pinInt, 0);
					break;
				}
				case NS_TYPE_VECTOR2I:
				{
					sf::Vector2i pinVector2i;
					utils::vector2iFromString(line, pinVector2i);
					if (callbacks.OnSetNodeInput != nullptr) callbacks.OnSetNodeInput(-1, currentPin, &pinVector2i, 0);
					break;
				}
				}
				currentPin++;
			}
		}
		else if (currentState == LoadState::ReadingConnections)
		{
			if (currentSubState == LoadSubState::ReadingConnectionNodes)
			{
				int commaPos = 0;
				for (; line[commaPos] != ','; commaPos++) {}
				currentConnectionLeftNode = std::stoi(line.substr(0, commaPos));
				currentConnectionRightNode = std::stoi(line.substr(commaPos + 1, line.length() - 1 - commaPos));
				currentSubState = LoadSubState::ReadingConnectionPins;
				continue;
			}
			else // reading connection pins
			{
				int commaPos = 0;
				for (; line[commaPos] != ','; commaPos++) {}
				int leftPin = std::stoi(line.substr(0, commaPos));
				int rightPin = std::stoi(line.substr(commaPos + 1, line.length() - 1 - commaPos));

				if (callbacks.OnAddConnection != nullptr) callbacks.OnAddConnection(currentConnectionLeftNode, leftPin,
					currentConnectionRightNode, rightPin);
			}
		}
		else if (currentState == LoadState::ReadingSelectedNode)
		{
			selectedNode = std::stoi(line);
		}
		else if (currentState == LoadState::ReadingViews)
		{
			int q = 0, p = 0;
			for (; line[p] != ','; p++) {}
			nodeEditorZoom = std::stoi(line.substr(q, p - q));
			q = p = p + 1; for (; line[p] != ','; p++) {}
			nodeEditorViewPosition.x = std::stof(line.substr(q, p - q));
			q = p = p + 1; for (; line[p] != ','; p++) {}
			nodeEditorViewPosition.y = std::stof(line.substr(q, p - q));
			q = p = p + 1; for (; line[p] != ','; p++) {}
			viewportZoom = std::stoi(line.substr(q, p - q));
			q = p = p + 1; for (; line[p] != ','; p++) {}
			viewportViewPosition.x = std::stof(line.substr(q, p - q));
			q = p = p + 1;
			viewportViewPosition.y = std::stof(line.substr(q, line.length() - q));

			if (callbacks.OnSetNodeEditorState != nullptr) callbacks.OnSetNodeEditorState(selectedNode, nodeEditorZoom, nodeEditorViewPosition.x, nodeEditorViewPosition.y);
			if (callbacks.OnSetViewportState != nullptr) callbacks.OnSetViewportState(viewportZoom, viewportViewPosition.x, viewportViewPosition.y);
		}
		else if (currentState == LoadState::ReadingEmbeddedImages)
		{
			if (line.length() > 0)
			{

				sf::Image pinImage;
				utils::imageFromBase64String(line, pinImage);
				if (callbacks.OnSetNodeInput != nullptr) callbacks.OnSetNodeInput(embeddedImageNodes[embeddedImagesLoaded], embeddedImagePins[embeddedImagesLoaded], &pinImage, 1);

				embeddedImagesLoaded++;
			}
		}
	}
	if (callbacks.OnFinish != nullptr) callbacks.OnFinish();
}

void serializer::SaveIntoFile(const std::string& filePath)
{
	const std::vector<uiNode*>& nodes = uiNodeSystem::getNodeList();
	std::ofstream output(filePath);

	std::vector<sf::Image> embeddedImages;

	output << "| nodes\n";
	unsigned int originalNodeId = 0;
	unsigned int newNodeId = 0;
	std::unordered_map<unsigned int, unsigned int> original2NewId;
	for (const uiNode* node : nodes)
	{
		if (node == nullptr)
		{
			originalNodeId++;
			continue;
		}

		// node unique name (works as id)
		output << node->m_nodeName << '\n';
		// node position in editor space
		output << node->getPosition().x << ',' << node->getPosition().y << '\n';
		// stored data
		for (int i = 0; i < node->m_inputPinCount; i++)
		{
			switch (nodeProvider::getNodeDataByName(node->m_nodeName)->pinTypes[i])
			{
				case NS_TYPE_COLOR:
				{
					sf::Color* c = (sf::Color*)node->m_inputFields[i].getDataPointer();
					output << (int)c->r << ',' << (int)c->g << ',' << (int)c->b << ',' << (int)c->a << '\n';
					break;
				}
				case NS_TYPE_FLOAT:
					output << *(float*)node->m_inputFields[i].getDataPointer() << '\n';
					break;
				case NS_TYPE_IMAGE:
					output << (int)node->m_inputFields[i].imageContent << ' ';
					if (node->m_inputFields[i].imageContent == uiInputField::ImageFieldContent::FromFile)
						output << pathUtils::getRelativePath(filePath, node->m_inputFields[i].imagePath);
					else if (node->m_inputFields[i].imageContent == uiInputField::ImageFieldContent::FromMemory)
						embeddedImages.push_back(((sf::RenderTexture*)(node->m_inputFields[i].dataPointer))->getTexture().copyToImage());
					output << '\n';
					break;
				case NS_TYPE_FONT:
					output << (node->m_inputFields[i].fontPath.length() > 0 ?
						pathUtils::getRelativePath(filePath, node->m_inputFields[i].fontPath) :
						"None") << '\n';
					break;
				case NS_TYPE_STRING:
					output << *(std::string*)node->m_inputFields[i].getDataPointer() << '\n';
					break;
				case NS_TYPE_INT:
					output << *(int*)node->m_inputFields[i].getDataPointer() << '\n';
					break;
				case NS_TYPE_VECTOR2I:
				{
					sf::Vector2i* v = (sf::Vector2i*)node->m_inputFields[i].getDataPointer();
					output << utils::vector2iToString(*v) << '\n';
					break;
				}
			}
		}
		original2NewId[originalNodeId] = newNodeId;

		// separator
		output << "=\n";
		originalNodeId++;
		newNodeId++;
	}

	output << "| connections\n";
	int lineCount;
	const uiLineInfo* lineArray = nullptr;
	uiConnections::getLines(lineCount, lineArray);
	for (int i = 0; i < lineCount; i++)
	{
		const uiLineInfo& lineInfo = lineArray[i];
		if (lineInfo.hidden)
			continue;
		output << original2NewId[lineInfo.nodeA] << ',' << original2NewId[lineInfo.nodeB] << '\n';
		output << lineInfo.pinA << ',' << lineInfo.pinB << '\n';
		// separator
		output << "=\n";
	}

	output << "| selected node\n";
	int selectedNode = uiNodeSystem::getSelectedNode();
	output << (selectedNode < 0 ? -1 : (int)original2NewId[selectedNode]) << '\n';

	int nodeEditorZoom, viewportZoom;
	sf::Vector2f nodeEditorViewPosition, viewportViewPosition;
	uiNodeSystem::getView(nodeEditorZoom, nodeEditorViewPosition);
	uiViewport::getView(viewportZoom, viewportViewPosition);
	output << "| views\n";
	output << nodeEditorZoom << ',' << nodeEditorViewPosition.x << ',' << nodeEditorViewPosition.y << ',' << viewportZoom << ',' << viewportViewPosition.x << ',' << viewportViewPosition.y << '\n';

	if (embeddedImages.size() > 0)
	{
		output << "| embedded images\n";
		for (int i = 0; i < embeddedImages.size(); i++)
		{
			std::string base64Image;
			if (!utils::base64StringFromImage(embeddedImages[i], base64Image))
				std::cout << "[Serializer] Failed to save embedded image to memory\n";
			else
				output << base64Image << '\n';
		}
	}

	output.close();
}