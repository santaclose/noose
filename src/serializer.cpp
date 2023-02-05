#include "serializer.h"
#include <nodeProvider/nodeProvider.h>
#include <interface/uiNodeSystem.h>
#include <interface/uiViewport.h>
#include <interface/uiConnections.h>
#include <pathUtils.h>
#include <fstream>
#include <unordered_map>
#include <base64.h>
#include "types.h"

enum class LoadState { ReadingNodes = 0, ReadingConnections = 1, ReadingSelectedNode = 2, ReadingViews = 3, ReadingEmbeddedImages = 4 };
enum class LoadSubState { ReadingIds = 0, ReadingNodeCoordinates = 1, ReadingPinData = 2, ReadingConnectionNodes = 3, ReadingConnectionPins = 4 };

void serializer::LoadFromFile(const std::string& filePath)
{
	std::string folderPath = pathUtils::getFolderPath(filePath);

	// clear all nodes and connections before loading file
	uiNodeSystem::clearEverything();

	std::ifstream input(filePath);

	LoadState currentState;
	LoadSubState currentSubState;
	std::string currentNodeName;
	unsigned int currentPin;

	int currentConnectionLeftNode, currentConnectionRightNode;

	std::vector<uiNode*>& nodes = uiNodeSystem::getNodeList();

	std::vector<uiInputField*> embeddedImageInputFields;
	std::vector<int> embeddedImageNodes;
	int embeddedImagesLoaded = 0;

	std::string line;
	while (std::getline(input, line))
	{
		if (line.compare("| nodes") == 0)
		{
			currentState = LoadState::ReadingNodes;
			currentSubState = LoadSubState::ReadingIds;
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
			currentSubState =
				currentState == LoadState::ReadingNodes ?
				LoadSubState::ReadingIds :
				LoadSubState::ReadingConnectionNodes;
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
				sf::Vector2f nodeCoordinates;
				nodeCoordinates.x = std::stof(line.substr(0, commaPos));
				nodeCoordinates.y = std::stof(line.substr(commaPos + 1, line.size() - commaPos - 1));
				const nodeData* nodeDataToAdd = nodeProvider::getNodeDataByName(currentNodeName);
				if (nodeDataToAdd == nullptr)
				{
					std::cout << "[Serializer] Node not found for name: " + currentNodeName + "\n";
					continue;
				}
				uiNodeSystem::pushNewNode(
					nodeDataToAdd,
					uiNodeSystem::PushMode::AtPosition,
					false,
					nodeCoordinates
					);
				// bind to set pin data
				uiNodeSystem::setBoundInputFieldNode(nodes.size() - 1);
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
					int commaPos = 0;
					for (; line[commaPos] != ','; commaPos++) {}
					((sf::Color*)nodes.back()->m_inputFields[currentPin].getDataPointer())->r = std::stoi(line.substr(0, commaPos));
					int afterPrevComma = ++commaPos;
					for (; line[commaPos] != ','; commaPos++) {}
					((sf::Color*)nodes.back()->m_inputFields[currentPin].getDataPointer())->g = std::stoi(line.substr(afterPrevComma, commaPos - afterPrevComma));
					afterPrevComma = ++commaPos;
					for (; line[commaPos] != ','; commaPos++) {}
					((sf::Color*)nodes.back()->m_inputFields[currentPin].getDataPointer())->b = std::stoi(line.substr(afterPrevComma, commaPos - afterPrevComma));
					afterPrevComma = ++commaPos;
					((sf::Color*)nodes.back()->m_inputFields[currentPin].getDataPointer())->a = std::stoi(line.substr(afterPrevComma, line.length() - afterPrevComma));
					nodes.back()->m_inputFields[currentPin].shapes[0].color = nodes.back()->m_inputFields[currentPin].shapes[1].color =
						nodes.back()->m_inputFields[currentPin].shapes[2].color = nodes.back()->m_inputFields[currentPin].shapes[3].color =
						*((sf::Color*)(nodes.back()->m_inputFields[currentPin].dataPointer));
					break;
				}
				case NS_TYPE_FLOAT:
					((float*)nodes.back()->m_inputFields[currentPin].getDataPointer())[0] = std::stof(line);
					nodes.back()->m_inputFields[currentPin].texts[0].setString(line);
					break;
				case NS_TYPE_STRING:
					((std::string*)nodes.back()->m_inputFields[currentPin].getDataPointer())[0] = line;
					nodes.back()->m_inputFields[currentPin].texts[0].setString(line);
					break;
				case NS_TYPE_IMAGE:
				{
					uiInputField::ImageFieldContent imageContentType = (uiInputField::ImageFieldContent)(line[0] - '0'); // char to int/enum
					switch (imageContentType)
					{
					case uiInputField::ImageFieldContent::None:
					{
						break;
					}
					case uiInputField::ImageFieldContent::FromFile:
					{
						std::string fullImagePath = folderPath + line.substr(2);
						nodes.back()->m_inputFields[currentPin].setValue(&fullImagePath);
						break;
					}
					case uiInputField::ImageFieldContent::FromMemory:
					{
						embeddedImageNodes.push_back(nodes.size() - 1);
						embeddedImageInputFields.push_back(& (nodes.back()->m_inputFields[currentPin]));
						break;
					}
					}
					break;
				}
				case NS_TYPE_FONT:
				{
					if (line.compare("None") == 0)
						break;
					std::string fullFontPath = folderPath + line;
					sf::Font* pointer = (sf::Font*)nodes.back()->m_inputFields[currentPin].dataPointer;
					if (!pointer->loadFromFile(fullFontPath))
						std::cout << "[Serializer] Failed to open font file: " + fullFontPath + "\n";
					nodes.back()->m_inputFields[currentPin].fontPath = fullFontPath;
					nodes.back()->m_inputFields[currentPin].texts[0].setString(pathUtils::getFileNameFromPath(fullFontPath.c_str()));
					break;
				}
				case NS_TYPE_INT:
				{
					int intValue = ((int*)nodes.back()->m_inputFields[currentPin].getDataPointer())[0] = std::stoi(line);
					if (nodeProvider::getNodeDataByName(currentNodeName)->pinEnumOptions[currentPin].size() == 0)
						nodes.back()->m_inputFields[currentPin].texts[0].setString(line);
					else // enum options
						nodes.back()->m_inputFields[currentPin].texts[0].setString(nodeProvider::getNodeDataByName(currentNodeName)->pinEnumOptions[currentPin][intValue]);
					break;
				}
				case NS_TYPE_VECTOR2I:
				{
					int commaPos = 0;
					for (; line[commaPos] != ','; commaPos++) {}
					std::string xString = line.substr(0, commaPos);
					nodes.back()->m_inputFields[currentPin].texts[0].setString(xString);
					((sf::Vector2i*)nodes.back()->m_inputFields[currentPin].getDataPointer())->x = std::stoi(xString);
					commaPos++;
					std::string yString = line.substr(commaPos, line.length() - commaPos);
					nodes.back()->m_inputFields[currentPin].texts[1].setString(yString);
					((sf::Vector2i*)nodes.back()->m_inputFields[currentPin].getDataPointer())->y = std::stoi(yString);
					break;
				}
				}
				nodes.back()->m_inputFields[currentPin].updateTextPositions();
				nodes.back()->m_inputFields[currentPin].onValueChanged();
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
				
				uiNodeSystem::createConnection(
					currentConnectionLeftNode,
					currentConnectionRightNode,
					leftPin,
					rightPin
				);
			}
		}
		else if (currentState == LoadState::ReadingSelectedNode)
		{
			uiNodeSystem::setSelectedNode(std::stoi(line));
		}
		else if (currentState == LoadState::ReadingViews)
		{
			int nodeEditorZoom, viewportZoom;
			sf::Vector2f nodeEditorViewPosition, viewportViewPosition;

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

			uiNodeSystem::setView(nodeEditorZoom, nodeEditorViewPosition);
			uiViewport::setView(viewportZoom, viewportViewPosition);
		}
		else if (currentState == LoadState::ReadingEmbeddedImages)
		{
			if (line.length() > 0)
			{
				std::string pngBytes = base64::decode(line);
				sf::Image embeddedImage;
				embeddedImage.loadFromMemory(&(pngBytes[0]), pngBytes.length());
				embeddedImageInputFields[embeddedImagesLoaded]->setValue(&embeddedImage, 1); // flag to let it know it's not a file path

				// execute the nodes
				uiNodeSystem::setBoundInputFieldNode(embeddedImageNodes[embeddedImagesLoaded]);
				embeddedImageInputFields[embeddedImagesLoaded]->updateTextPositions();
				embeddedImageInputFields[embeddedImagesLoaded]->onValueChanged();

				embeddedImagesLoaded++;
			}
		}
	}
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
					output << v->x << ',' << v->y << '\n';
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
			std::vector<uint8_t> pngImage;
			// allocate max we would need for assert not to show up in debug mode
			// https://stackoverflow.com/questions/35310117/debug-assertion-failed-expression-acrt-first-block-header
			pngImage.reserve(embeddedImages[i].getSize().x * embeddedImages[i].getSize().y * 4);
			if (!embeddedImages[i].saveToMemory(pngImage, "png"))
				std::cout << "[Serializer] Failed to save embedded image to memory\n";
			else
			{
				std::string base64Encoded = base64::encode(&(pngImage[0]), pngImage.size());
				output << base64Encoded << '\n';
			}
		}
	}

	output.close();
}