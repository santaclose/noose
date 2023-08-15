#include "serializer.h"
#include <nodeProvider/nodeProvider.h>
#include <interface/uiNodeSystem.h>
#include <interface/uiViewport.h>
#include <interface/uiConnections.h>
#include <pathUtils.h>
#include <utils.h>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <base64.h>
#include <json.hpp>
#include "types.h"

static std::unordered_map<std::string, uiInputField::ImageFieldContent> IMAGE_CONTENT_TYPE_SERIALIZER = {
	{"memory", uiInputField::ImageFieldContent::FromMemory },
	{"file", uiInputField::ImageFieldContent::FromFile },
	{"none", uiInputField::ImageFieldContent::None }
};

void serializer::LoadFromFileJson(const std::string& filePath)
{
	std::string folderPath = pathUtils::getFolderPath(filePath);

	// clear all nodes and connections before loading file
	uiNodeSystem::clearEverything();

	using json = nlohmann::ordered_json;
	std::ifstream input(filePath);
	json jsonObject = json::parse(input);

	unsigned int currentPin;
	int currentConnectionLeftNode, currentConnectionRightNode;
	std::vector<uiNode*>& nodes = uiNodeSystem::getNodeList();
	std::vector<uiInputField*> embeddedImageInputFields;
	std::vector<int> embeddedImageNodes;

	for (const auto& nodeItem : jsonObject["nodes"])
	{
		std::string currentNodeName = nodeItem["name"];
		sf::Vector2f nodeCoordinates;
		nodeCoordinates.x = nodeItem["posX"];
		nodeCoordinates.y = nodeItem["posY"];
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

		int currentPin = 0;
		for (const auto& pinItem : nodeItem["pinData"])
		{
			switch (nodeProvider::getNodeDataByName(currentNodeName)->pinTypes[currentPin])
			{
			case NS_TYPE_COLOR:
			{
				sf::Color pinColor;
				if (!utils::colorFromHexString((std::string)pinItem["value"], pinColor))
					std::cout << "[Serializer] Failed to parse color\n";
				((sf::Color*)nodes.back()->m_inputFields[currentPin].getDataPointer())[0] = pinColor;
				nodes.back()->m_inputFields[currentPin].shapes[0].color = nodes.back()->m_inputFields[currentPin].shapes[1].color =
					nodes.back()->m_inputFields[currentPin].shapes[2].color = nodes.back()->m_inputFields[currentPin].shapes[3].color = pinColor;
				break;
			}
			case NS_TYPE_FLOAT:
				((float*)nodes.back()->m_inputFields[currentPin].getDataPointer())[0] = (float)pinItem["value"];
				nodes.back()->m_inputFields[currentPin].texts[0].setString(std::to_string((float)pinItem["value"]));
				break;
			case NS_TYPE_STRING:
				((std::string*)nodes.back()->m_inputFields[currentPin].getDataPointer())[0] = (std::string)pinItem["value"];
				nodes.back()->m_inputFields[currentPin].texts[0].setString((std::string)pinItem["value"]);
				break;
			case NS_TYPE_IMAGE:
			{
				uiInputField::ImageFieldContent imageContentType = IMAGE_CONTENT_TYPE_SERIALIZER[(std::string)(pinItem["content"])];
				switch (imageContentType)
				{
				case uiInputField::ImageFieldContent::None:
				{
					break;
				}
				case uiInputField::ImageFieldContent::FromFile:
				{
					std::string fullImagePath = folderPath + (std::string)pinItem["path"];
					nodes.back()->m_inputFields[currentPin].setValue(&fullImagePath);
					break;
				}
				case uiInputField::ImageFieldContent::FromMemory:
				{
					embeddedImageNodes.push_back(nodes.size() - 1);
					embeddedImageInputFields.push_back(&(nodes.back()->m_inputFields[currentPin]));
					break;
				}
				}
				break;
			}
			case NS_TYPE_FONT:
			{
				if (((std::string)pinItem["path"]).compare("none") == 0)
					break;
				std::string fullFontPath = folderPath + (std::string)pinItem["path"];
				sf::Font* pointer = (sf::Font*)nodes.back()->m_inputFields[currentPin].dataPointer;
				if (!pointer->loadFromFile(fullFontPath))
					std::cout << "[Serializer] Failed to open font file: " + fullFontPath + "\n";
				nodes.back()->m_inputFields[currentPin].fontPath = fullFontPath;
				nodes.back()->m_inputFields[currentPin].texts[0].setString(pathUtils::getFileNameFromPath(fullFontPath.c_str()));
				break;
			}
			case NS_TYPE_INT:
			{
				int intValue = ((int*)nodes.back()->m_inputFields[currentPin].getDataPointer())[0] = (int)pinItem["value"];
				if (nodeProvider::getNodeDataByName(currentNodeName)->pinEnumOptions[currentPin].size() == 0)
					nodes.back()->m_inputFields[currentPin].texts[0].setString(std::to_string((int)pinItem["value"]));
				else // enum options
					nodes.back()->m_inputFields[currentPin].texts[0].setString(nodeProvider::getNodeDataByName(currentNodeName)->pinEnumOptions[currentPin][intValue]);
				break;
			}
			case NS_TYPE_VECTOR2I:
			{
				nodes.back()->m_inputFields[currentPin].texts[0].setString(((std::string)pinItem["x"]));
				((sf::Vector2i*)nodes.back()->m_inputFields[currentPin].getDataPointer())->x = std::stoi(((std::string)pinItem["x"]));
				nodes.back()->m_inputFields[currentPin].texts[1].setString(((std::string)pinItem["y"]));
				((sf::Vector2i*)nodes.back()->m_inputFields[currentPin].getDataPointer())->y = std::stoi(((std::string)pinItem["y"]));
				break;
			}
			}
			nodes.back()->m_inputFields[currentPin].updateTextPositions();
			nodes.back()->m_inputFields[currentPin].onValueChanged();
			currentPin++;
		}
	}

	for (const auto& connectionItem : jsonObject["connections"])
	{
		uiNodeSystem::createConnection(
			(int)connectionItem["nodeA"],
			(int)connectionItem["nodeB"],
			(int)connectionItem["pinA"],
			(int)connectionItem["pinB"]
		);
	}

	uiNodeSystem::setSelectedNode((int)jsonObject["selectedNode"]);
	uiNodeSystem::setView((int)jsonObject["views"]["nodeEditorZoom"], { (float)jsonObject["views"]["nodeEditorViewPositionX"], (float)jsonObject["views"]["nodeEditorViewPositionY"] });
	uiViewport::setView((int)jsonObject["views"]["viewportZoom"], { (float)jsonObject["views"]["viewportViewPositionX"], (float)jsonObject["views"]["viewportViewPositionY"] });

	int embeddedImagesLoaded = 0;
	for (const std::string& embeddedImageItem : jsonObject["embeddedImages"])
	{
		std::string pngBytes = base64::decode(embeddedImageItem);
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

void serializer::SaveIntoFileJson(const std::string& filePath)
{
	const std::vector<uiNode*>& nodes = uiNodeSystem::getNodeList();

	std::vector<sf::Image> embeddedImages;

	using json = nlohmann::ordered_json;
	json jsonObject;
	jsonObject["nodes"] = json::array();

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

		auto jsonPinArray = json::array();

		// stored data
		for (int i = 0; i < node->m_inputPinCount; i++)
		{
			uiInputField::ImageFieldContent imageContentType = node->m_inputFields[i].imageContent;
			switch (nodeProvider::getNodeDataByName(node->m_nodeName)->pinTypes[i])
			{
			case NS_TYPE_COLOR:
			{
				sf::Color* c = (sf::Color*)node->m_inputFields[i].getDataPointer();
				
				jsonPinArray.push_back({
					{"value", utils::hexStringFromColor(*c)}
					});
				break;
			}
			case NS_TYPE_FLOAT:
				jsonPinArray.push_back({
					{"value",  *(float*)node->m_inputFields[i].getDataPointer()}
					});
				break;
			case NS_TYPE_IMAGE:
				jsonPinArray.push_back({
					{"content", imageContentType == uiInputField::ImageFieldContent::FromFile ? "file" : (imageContentType == uiInputField::ImageFieldContent::FromMemory ? "memory" : "none")},
					{"path", imageContentType == uiInputField::ImageFieldContent::FromFile ? pathUtils::getRelativePath(filePath, node->m_inputFields[i].imagePath) : "none"}
					});
				if (node->m_inputFields[i].imageContent == uiInputField::ImageFieldContent::FromMemory)
					embeddedImages.push_back(((sf::RenderTexture*)(node->m_inputFields[i].dataPointer))->getTexture().copyToImage());
				break;
			case NS_TYPE_FONT:
				jsonPinArray.push_back({
					{"path", node->m_inputFields[i].fontPath.length() > 0 ? pathUtils::getRelativePath(filePath, node->m_inputFields[i].fontPath) : "none"}
					});
				break;
			case NS_TYPE_STRING:
				jsonPinArray.push_back({
					{"value",  *(std::string*)node->m_inputFields[i].getDataPointer()}
					});
				break;
			case NS_TYPE_INT:
				jsonPinArray.push_back({
					{"value",  *(int*)node->m_inputFields[i].getDataPointer()}
					});
				break;
			case NS_TYPE_VECTOR2I:
			{
				sf::Vector2i* v = (sf::Vector2i*)node->m_inputFields[i].getDataPointer();
				jsonPinArray.push_back({
					{"x",  v->x},
					{"y",  v->y}
					});
				break;
			}
			}
		}

		jsonObject["nodes"].push_back({
			{"name", node->m_nodeName }, // node unique name (works as id)
			{"posX", node->getPosition().x }, // node position in editor space
			{"posY", node->getPosition().y },
			{"pinData", jsonPinArray }
			});

		original2NewId[originalNodeId] = newNodeId;

		originalNodeId++;
		newNodeId++;
	}

	jsonObject["connections"] = json::array();

	int lineCount;
	const uiLineInfo* lineArray = nullptr;
	uiConnections::getLines(lineCount, lineArray);
	for (int i = 0; i < lineCount; i++)
	{
		const uiLineInfo& lineInfo = lineArray[i];
		if (lineInfo.hidden)
			continue;

		jsonObject["connections"].push_back({
			{"nodeA", original2NewId[lineInfo.nodeA] }, // node unique name (works as id)
			{"nodeB", original2NewId[lineInfo.nodeB] }, // node position in editor space
			{"pinA", lineInfo.pinA },
			{"pinB", lineInfo.pinB }
		});
	}

	int selectedNode = uiNodeSystem::getSelectedNode();
	jsonObject["selectedNode"] = (selectedNode < 0 ? -1 : (int)original2NewId[selectedNode]);

	int nodeEditorZoom, viewportZoom;
	sf::Vector2f nodeEditorViewPosition, viewportViewPosition;
	uiNodeSystem::getView(nodeEditorZoom, nodeEditorViewPosition);
	uiViewport::getView(viewportZoom, viewportViewPosition);
	jsonObject["views"]["nodeEditorZoom"] = nodeEditorZoom;
	jsonObject["views"]["nodeEditorViewPositionX"] = nodeEditorViewPosition.x;
	jsonObject["views"]["nodeEditorViewPositionY"] = nodeEditorViewPosition.y;
	jsonObject["views"]["viewportZoom"] = viewportZoom;
	jsonObject["views"]["viewportViewPositionX"] = viewportViewPosition.x;
	jsonObject["views"]["viewportViewPositionY"] = viewportViewPosition.y;

	std::vector<std::string> embeddedImagesVector;
	if (embeddedImages.size() > 0)
	{
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
				embeddedImagesVector.push_back(base64Encoded);
			}
		}
		jsonObject["embeddedImages"] = embeddedImagesVector;
	}

	std::ofstream output(filePath);
	output << jsonObject.dump(1, '\t');
	output.close();
}