#include "serializer.h"
#include <nodeProvider/nodeProvider.h>
#include <interface/uiNodeSystem.h>
#include <interface/uiConnections.h>
#include <utils.h>
#include <fstream>
#include <unordered_map>
#include "types.h"

enum class LoadState { ReadingNodes = 0, ReadingConnections = 1 };
enum class LoadSubState { ReadingIds = 0, ReadingNodeCoordinates = 1, ReadingPinData = 2, ReadingConnectionNodes = 3, ReadingConnectionPins = 4 };

void serializer::LoadFromFile(const std::string& filePath)
{
	std::string folderPath = utils::getFolderPath(filePath);

	// clear all nodes and connections before loading file
	uiNodeSystem::clearEverything();

	std::ifstream input(filePath);

	LoadState currentState;
	LoadSubState currentSubState;
	unsigned int currentNodeDataId;
	unsigned int currentPin;

	int currentConnectionLeftNode, currentConnectionRightNode;

	std::vector<uiNode*>& nodes = uiNodeSystem::getUiNodeList();

	std::string line;
	while (std::getline(input, line))
	{
		if (line.compare("-- nodes") == 0)
		{
			currentState = LoadState::ReadingNodes;
			currentSubState = LoadSubState::ReadingIds;
			continue;
		}
		if (line.compare("-- connections") == 0)
		{
			currentState = LoadState::ReadingConnections;
			currentSubState = LoadSubState::ReadingConnectionNodes;
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
				currentNodeDataId = std::stoi(line);
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
				uiNodeSystem::pushNewNode(
					&(nodeProvider::nodeDataList[currentNodeDataId]),
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
				switch (nodeProvider::nodeDataList[currentNodeDataId].pinTypes[currentPin])
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
				case NS_TYPE_IMAGE:
				{
					if (line.compare("None") == 0)
						break;
					std::string fullImagePath = folderPath + line;
					sf::Texture tx;
					if (!tx.loadFromFile(fullImagePath))
						std::cout << "[Serializer] Failed to open image file: " + fullImagePath + "\n";
					nodes.back()->m_inputFields[currentPin].imagePath = fullImagePath;
					nodes.back()->m_inputFields[currentPin].texts[0].setString(utils::getFileNameFromPath(fullImagePath.c_str()));
					uiInputField::loadImageShader.setUniform("tx", tx);

					sf::Sprite spr(tx);
					sf::Vector2u txSize = tx.getSize();
					sf::RenderTexture* pointer = (sf::RenderTexture*) nodes.back()->m_inputFields[currentPin].dataPointer;

					pointer->create(txSize.x, txSize.y);
					pointer->draw(spr, &uiInputField::loadImageShader);
					break;
				}
				case NS_TYPE_INT:
				{
					int intValue = ((int*)nodes.back()->m_inputFields[currentPin].getDataPointer())[0] = std::stoi(line);
					if (nodeProvider::nodeDataList[currentNodeDataId].pinEnumOptions[currentPin].size() == 0)
						nodes.back()->m_inputFields[currentPin].texts[0].setString(line);
					else // enum options
						nodes.back()->m_inputFields[currentPin].texts[0].setString(nodeProvider::nodeDataList[currentNodeDataId].pinEnumOptions[currentPin][intValue]);
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
		else // currentState == LoadState::ReadingConnections
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
	}
}

void serializer::SaveIntoFile(const std::string& filePath)
{
	const std::vector<uiNode*>& nodes = uiNodeSystem::getUiNodeList();
	std::ofstream output(filePath);

	output << "-- nodes\n";
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

		// node type id and editor id
		output << node->m_nodeTypeId << '\n';
		// node position in editor space
		output << node->getPosition().x << ',' << node->getPosition().y << '\n';
		// stored data
		for (int i = 0; i < node->m_inputPinCount; i++)
		{
			switch (nodeProvider::nodeDataList[node->m_nodeTypeId].pinTypes[i])
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
					output << (node->m_inputFields[i].imagePath.length() > 0 ?
						utils::getRelativePath(filePath, node->m_inputFields[i].imagePath) :
						"None") << '\n';
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
	output << "-- connections\n";
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

	output.close();
}

void serializer::LoadImageFile(const std::string& filePath)
{
	std::vector<uiNode*>& nodes = uiNodeSystem::getUiNodeList();
	uiNodeSystem::pushNewNode(&nodeProvider::nodeDataList[0], uiNodeSystem::PushMode::Centered, true);
	// bind to set pin data
	uiNodeSystem::setBoundInputFieldNode(nodes.size() - 1);

	sf::Texture tx;
	if (!tx.loadFromFile(filePath))
	{
		std::cout << "[Serializer] Failed to open image file\n";
		return;
	}
	nodes.back()->m_inputFields[0].imagePath = filePath;
	nodes.back()->m_inputFields[0].texts[0].setString(utils::getFileNameFromPath(filePath.c_str()));
	uiInputField::loadImageShader.setUniform("tx", tx);

	sf::Sprite spr(tx);
	sf::Vector2u txSize = tx.getSize();
	sf::RenderTexture* pointer = (sf::RenderTexture*)nodes.back()->m_inputFields[0].dataPointer;

	pointer->create(txSize.x, txSize.y);
	pointer->draw(spr, &uiInputField::loadImageShader);

	nodes.back()->m_inputFields[0].updateTextPositions();
	nodes.back()->m_inputFields[0].onValueChanged();
}