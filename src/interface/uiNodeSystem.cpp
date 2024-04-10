#include "uiNodeSystem.h"

#include "uiConnections.h"
#include "uiSelectionBox.h"
#include "../logic/nodeSystem.h"
#include "../type2color.h"
#include "../utils.h"

#include "nodeProvider/nodeProvider.h"

#include "uiViewport.h"

#include <iostream>
#include <vector>
#include <math.h>

#define MAX_ZOOM 5
#define MIN_ZOOM 20

namespace uiNodeSystem {

	sf::RenderWindow* renderWindow;
	const sf::Vector2i* mouseScreenPosPointer;
	sf::Vector2f mouseWorldPos;
	uiSelectionBox nodeSelectionBox;

	std::vector<uiNode*> uiNodeList;
	int selectedNodeIndex = -1;
	int draggingNodeIndex = -1;
	std::unordered_map<int, int> uiNodeIdToLogicalNodeId;
	std::unordered_map<int, int> uiConnectionIdToLogicalConnectionId;

	bool panning = false;

	bool creatingConnection = false;
	bool connectionStartedOnRightSideNode;
	int connectionTempNode;
	int connectionTempPin;

	bool removingConnections = false;

	sf::Vector2f lastMouseScreenPos;
	sf::Vector2f currentMouseScreenPos;

	sf::View theView; // view with panning and zoom transformations
	sf::Vector2f viewPosition;
	int zoomInt = 10;
	float currentZoom = 1.0f;

	// do not call these with invalid node index
	void (*onNodeSelectedCallback)(int) = nullptr;
	void (*onNodeDeletedCallback)(int) = nullptr;
	void (*onNodeChangedCallback)(int) = nullptr;

	int boundInputFieldNode = -1;

	void onInputFieldValueChanged(int pin);
	void deleteLine(int lineToDelete);
	void updateView();
	//int findSlotForNode();
}

void uiNodeSystem::onInputFieldValueChanged(int pin)
{
	nodeSystem::onNodeChanged(uiNodeIdToLogicalNodeId[boundInputFieldNode], pin);
#ifdef TEST
	std::cout << "onNodeChanged:\n\tbound input field node: " << boundInputFieldNode << std::endl;
#endif
	onNodeChangedCallback(boundInputFieldNode);
}

void uiNodeSystem::deleteLine(int lineToDelete)
{
	int nA, nB;
	uiConnections::getNodesForLine(lineToDelete, nA, nB);

	uiNodeList[nA]->setLineIndexAsDisconnected(lineToDelete);
	uiNodeList[nB]->setLineIndexAsDisconnected(lineToDelete);

	//std::cout << "hiding line " << lineToDelete << std::endl;
	uiConnections::hide(lineToDelete);
}

void uiNodeSystem::updateView()
{
	currentZoom = zoomInt / 10.0f;
	theView = sf::View(viewPosition, (sf::Vector2f)renderWindow->getSize());
	theView.zoom(currentZoom);
}


void uiNodeSystem::initialize(sf::RenderWindow& theRenderWindow, const sf::Vector2i* mouseScreenPosPointer)
{
	uiConnections::initialize(currentZoom);
	nodeSystem::initialize();
	renderWindow = &theRenderWindow;
	uiNodeSystem::mouseScreenPosPointer = mouseScreenPosPointer;
	updateView();
	nodeSelectionBox.initialize();
	uiNodeList.reserve(64);
	uiNodeIdToLogicalNodeId.reserve(64);
}

void uiNodeSystem::terminate()
{
	for (uiNode* n : uiNodeList)
	{
		if (n != nullptr)
			delete n;
	}
	nodeSelectionBox.terminate();
	nodeSystem::terminate();
}

int uiNodeSystem::pushNewNode(const nodeData* nData, PushMode mode, bool nodeCenterInPosition, sf::Vector2f worldPos)
{
	renderWindow->setView(theView);
	switch (mode)
	{
	case PushMode::Centered:
		worldPos = renderWindow->mapPixelToCoords(
			sf::Vector2i(
				renderWindow->getSize().x / 2,
				renderWindow->getSize().y / 2));
		break;
	case PushMode::AtCursorPosition:
		worldPos = renderWindow->mapPixelToCoords(*mouseScreenPosPointer);
		break;
	}

	int logicalNodeId = nodeSystem::createNode(nData);
#ifdef TEST
	std::cout << "createNode:\n\tnode id: " << newNodeID << "\n\tnode name: " << nData->nodeName << std::endl;
#endif

	uiNodeList.push_back(new uiNode(
			nData,
			nodeCenterInPosition,
			worldPos,
			nodeSystem::getDataPointersForNode(logicalNodeId),
			onInputFieldValueChanged,
			&nodeSelectionBox));
	int uiNodeId = uiNodeList.size() - 1;
	uiNodeIdToLogicalNodeId.insert({ uiNodeId, logicalNodeId });

	if (selectedNodeIndex == -1)
	{
		selectedNodeIndex = uiNodeId;
		uiNodeList[uiNodeId]->paintAsSelected();
		if (onNodeSelectedCallback != nullptr)
			onNodeSelectedCallback(uiNodeId);
	}
	return uiNodeId;
}

int uiNodeSystem::pushImageNodeFromFile(const std::string& filePath, PushMode mode, bool nodeCenterInPosition, sf::Vector2f worldPos, sf::Vector2u* outSize)
{
	int nodeID = uiNodeSystem::pushNewNode(nodeProvider::getNodeDataByName("Image"), mode, nodeCenterInPosition, worldPos);
	// bind to set pin data
	setBoundInputFieldNode(nodeID);
	uiNodeList[nodeID]->setInput(0, &filePath);
	if (outSize != nullptr)
	{
		const sf::RenderTexture* tempPointer = (const sf::RenderTexture*) uiNodeList[nodeID]->getDataPointer(0);
		*outSize = tempPointer->getSize();
	}
	return nodeID;
}

int uiNodeSystem::pushFontNodeFromFile(const std::string& filePath, PushMode mode, bool nodeCenterInPosition, sf::Vector2f worldPos)
{
	int nodeID = uiNodeSystem::pushNewNode(nodeProvider::getNodeDataByName("Font"), mode, nodeCenterInPosition, worldPos);
	// bind to set pin data
	setBoundInputFieldNode(nodeID);
	uiNodeList[nodeID]->setInput(0, &filePath);
	return nodeID;
}

void uiNodeSystem::onPollEvent(const sf::Event& e)
{
	renderWindow->setView(theView);
	mouseWorldPos = renderWindow->mapPixelToCoords(*mouseScreenPosPointer);

	switch (e.type)
	{
		case sf::Event::Resized:
		{
			updateView();
			break;
		}
		case sf::Event::MouseButtonPressed:
		{
			if (e.mouseButton.button == sf::Mouse::Button::Left)
			{
				if (uiInputField::onMouseDown(mouseWorldPos))
					return;

				// collision from top to bottom
				for (int i = uiNodeList.size() - 1; i > -1; i--)
				{
					if (uiNodeList[i] == nullptr)
						continue;

					// index stores pin or inputfield index
					// subindex stores subinputfield index
					int index, subIndex;
					uiNode::MousePos mp = uiNodeList[i]->mouseOver(mouseWorldPos, index, subIndex);

					if (mp == uiNode::MousePos::Outside)
						continue;

					switch (mp)
					{
					case uiNode::MousePos::TopBar: // drag node
					{
						draggingNodeIndex = i;
						lastMouseScreenPos = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
						break;
					}
					case uiNode::MousePos::Pin: // mouse is over a pin
					{
						if (uiNodeList[i]->canConnectToPin(index))
						{
							// global state
							creatingConnection = true;
							connectionTempNode = i;
							connectionTempPin = index;

							connectionStartedOnRightSideNode = index < uiNodeList[i]->getInputPinCount();

							// temporary line
							uiConnections::createTemporary(
								uiNodeList[i]->getPinPosition(index),
								mouseWorldPos,
								uiNodeList[i]->getPinColor(index),
								connectionTempNode,
								connectionTempPin,
								connectionStartedOnRightSideNode
							);
						}
						break;
					}
					case uiNode::MousePos::InputField: // mouse is over an input field
					{
						boundInputFieldNode = i;
						uiNodeList[i]->bindInputField(index, subIndex, subIndex == 2 ? uiInputField::PickPosition : uiInputField::Default);
						return;
					}
					}
					break; // don't allow to click through nodes
				}
			}
			else if (e.mouseButton.button == sf::Mouse::Button::Right)
			{
				int i = uiNodeList.size() - 1;
				// collision from top to bottom
				for (; i > -1; i--)
				{
					if (uiNodeList[i] == nullptr)
						continue;

					// index stores pin or inputfield index
					// subindex stores subinputfield index
					int index, subIndex;
					uiNode::MousePos mp = uiNodeList[i]->mouseOver(mouseWorldPos, index, subIndex);

					if (mp == uiNode::MousePos::Outside)
						continue;

					switch (mp)
					{
					case uiNode::MousePos::InputField: // mouse is over an input field
					{
						boundInputFieldNode = i;
						uiNodeList[i]->bindInputField(index, subIndex, uiInputField::InteractionMode::Typing);
						return;
					}
					case uiNode::MousePos::TopBar:
					{
						// node selection
						if (i == selectedNodeIndex)
						{
							uiNodeList[i]->paintAsUnselected();
							selectedNodeIndex = -1;
							return;
						}

						if (selectedNodeIndex > -1)
							uiNodeList[selectedNodeIndex]->paintAsUnselected();

						uiNodeList[i]->paintAsSelected();
						selectedNodeIndex = i;

						if (onNodeSelectedCallback != nullptr)
							onNodeSelectedCallback(selectedNodeIndex);
						return;
					}
					}
					break;
				}
				removingConnections = true;
				lastMouseScreenPos = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
			}
			else if (e.mouseButton.button == sf::Mouse::Button::Middle)
			{
				panning = true;
				lastMouseScreenPos = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
			}
			break;
		}
		case sf::Event::MouseButtonReleased:
		{
			if (e.mouseButton.button == sf::Mouse::Button::Middle)
			{
				panning = false;
			}
			else if (e.mouseButton.button == sf::Mouse::Button::Left)
			{
				// collision from top to bottom
				if (creatingConnection)
				{
					int i = uiNodeList.size() - 1;

					for (; i > -1; i--)
					{
						if (uiNodeList[i] == nullptr)
							continue;

						int index, subIndex;
						uiNode::MousePos mouseOverValue = uiNodeList[i]->mouseOver(mouseWorldPos, index, subIndex);
						if (mouseOverValue == uiNode::MousePos::Pin)
						{
							if (!uiNodeList[i]->canConnectToPin(index))
							{
								uiConnections::hideTemporary();
								break;
							}

							int leftSideNode = connectionStartedOnRightSideNode ? i : connectionTempNode;
							int rightSideNode = connectionStartedOnRightSideNode ? connectionTempNode : i;
							int leftSidePin = connectionStartedOnRightSideNode ? index : connectionTempPin;
							int rightSidePin = connectionStartedOnRightSideNode ? connectionTempPin : index;

							// can't be both output or input
							bool canConnect =
								(leftSidePin < uiNodeList[leftSideNode]->getInputPinCount()) !=
								(rightSidePin < uiNodeList[rightSideNode]->getInputPinCount());
							canConnect &= nodeSystem::isConnectionValid(uiNodeIdToLogicalNodeId[leftSideNode], uiNodeIdToLogicalNodeId[rightSideNode], leftSidePin, rightSidePin);

							if (!canConnect) // failed to connect
							{
								uiConnections::hideTemporary();
								break;
							}
							
							// update interface lines
							// node indices are flipped if necessary such that node a is on the left side
							int uiConnectionId = uiConnections::connect(
								uiNodeList[i]->getPinPosition(index),
								connectionStartedOnRightSideNode ? leftSideNode : rightSideNode,
								connectionStartedOnRightSideNode ? leftSidePin : rightSidePin);

							// right side node first
							uiNodeList[rightSideNode]->attachConnectionPoint(uiConnectionId, rightSidePin);
							uiNodeList[leftSideNode]->attachConnectionPoint(uiConnectionId, leftSidePin);

							int logicalConnectionId = nodeSystem::connect(uiNodeIdToLogicalNodeId[leftSideNode], uiNodeIdToLogicalNodeId[rightSideNode], leftSidePin, rightSidePin);
							uiConnectionIdToLogicalConnectionId[uiConnectionId] = logicalConnectionId;
#ifdef TEST
	std::cout << "connect:\n\tleft side node: " << leftSideNode << "\n\tright side node: " << rightSideNode << "\n\tleft side pin: " << leftSidePin << "\n\tright side pin: " << rightSidePin << "\n\tconnection index: " << uiConnectionId << std::endl;
#endif

							break;
						}
					}

					// i is equal to -1 when the for loop ends and we know the mouse was not over any pin cuz we break
					if (i < 0) // mouse was not over any pin
						uiConnections::hideTemporary();
				}

				draggingNodeIndex = -1;
				creatingConnection = false;
				uiInputField::onMouseUp();
			}
			else if (e.mouseButton.button == sf::Mouse::Button::Right)
			{
				removingConnections = false;
			}
			break;
		}
		case sf::Event::MouseMoved:
		{
			currentMouseScreenPos = sf::Vector2f(e.mouseMove.x, e.mouseMove.y);
			sf::Vector2f displacement = -lastMouseScreenPos + currentMouseScreenPos;
			if (panning)
			{
				viewPosition -= displacement * currentZoom;
				updateView();
			}
			else if (draggingNodeIndex > -1) // dragging a node
			{
				uiNodeList[draggingNodeIndex]->displace(displacement * currentZoom); // pass node index so it can move the lines
			}
			else if (creatingConnection)
			{
				uiConnections::displaceTemporary(displacement * currentZoom);
			}
			else if (removingConnections)
			{
				int connectionToRemove = uiConnections::onTryingToRemove(mouseWorldPos);
				if (connectionToRemove > -1) // mouse was over a line and we have to detach it from nodeA and nodeB
				{
					deleteLine(connectionToRemove);

					int nA, nB, pA, pB;
					uiConnections::getNodesForLine(connectionToRemove, nA, nB);
					uiConnections::getPinsForLine(connectionToRemove, pA, pB);
					nodeSystem::disconnect(uiConnectionIdToLogicalConnectionId[connectionToRemove]);
#ifdef TEST
	std::cout << "disconnect:\n\tleft side node: " << nA << "\n\tright side node: " << nB << "\n\tleft side pin: " << pA << "\n\tright side pin: " << pB << "\n\tconnection index: " << lineToRemove << std::endl;
#endif
				}
			}
		
			lastMouseScreenPos = currentMouseScreenPos;
			uiInputField::onMouseMoved(displacement);
			break;
		}
		case sf::Event::MouseWheelScrolled:
		{
			zoomInt -= e.mouseWheelScroll.delta;

			//clamp from min to max zoom
			if (zoomInt < MAX_ZOOM) zoomInt = MAX_ZOOM; else if (zoomInt > MIN_ZOOM) zoomInt = MIN_ZOOM;
			updateView();

			uiConnections::updateShaderUniform(currentZoom);
			break;
		}
		case sf::Event::KeyPressed:
		{
			if (uiInputField::typingInteractionOngoing())
				return;
			switch (e.key.code)
			{
				case sf::Keyboard::Key::Backspace:
				case sf::Keyboard::Key::Delete:
				{
					// can't delete if node is not selected or any node being moved
					if (selectedNodeIndex < 0 || draggingNodeIndex > -1)
						return;

					// a copy is needed
					std::vector<int> lines = uiNodeList[selectedNodeIndex]->getConnectedLines();
					for (int l : lines)
						deleteLine(l);

					onNodeDeletedCallback(selectedNodeIndex);

					delete uiNodeList[selectedNodeIndex];
					uiNodeList[selectedNodeIndex] = nullptr;

					nodeSystem::deleteNode(uiNodeIdToLogicalNodeId[selectedNodeIndex]);
#ifdef TEST
					std::cout << "deleteNode:\n\tselected node index: " << selectedNodeIndex << "\n\tnode lines: ";
					for (int l : lines)
						std::cout << l << ", ";
					std::cout << std::endl;
#endif

					selectedNodeIndex = -1;
					break;
				}
				case sf::Keyboard::Key::F:
				{
					sf::Vector2f boundingBoxMax = { -INFINITY , -INFINITY };
					sf::Vector2f boundingBoxMin = {  INFINITY ,  INFINITY };
					int numberOfNodes = 0;
					for (const uiNode* node : uiNodeList)
					{
						if (node == nullptr)
							continue;
						sf::Vector2f nodeCenter = node->getCenterPosition();
						boundingBoxMax.x = boundingBoxMax.x < nodeCenter.x ? nodeCenter.x : boundingBoxMax.x;
						boundingBoxMax.y = boundingBoxMax.y < nodeCenter.y ? nodeCenter.y : boundingBoxMax.y;
						boundingBoxMin.x = boundingBoxMin.x > nodeCenter.x ? nodeCenter.x : boundingBoxMin.x;
						boundingBoxMin.y = boundingBoxMin.y > nodeCenter.y ? nodeCenter.y : boundingBoxMin.y;
						numberOfNodes++;
					}
					viewPosition = (boundingBoxMax + boundingBoxMin) / 2.0f;
					updateView();
					break;
				}
			}
			break;
		}
		case sf::Event::TextEntered:
		{
			if (uiInputField::keyboardInput(e.text.unicode)) // input field handling keyboard input
				break;

			if (e.text.unicode == 3) // ctrl c
				copyNode();
			else if (e.text.unicode == 22) // ctrl v
			{
				if (!pasteNode())
				{
					sf::Image image;
					if (!utils::imageFromClipboard(image))
					{
						std::cout << "[UI] Failed to paste node or image from clipboard\n";
						break;
					}
					int nodeID = uiNodeSystem::pushNewNode(nodeProvider::getNodeDataByName("Image"));
					// bind to set pin data
					setBoundInputFieldNode(nodeID);
					uiNodeList[nodeID]->setInput(0, &image, 1); // flag to let it know it's not a file path
				}
			}

			break;
		}
		case sf::Event::FilesDropped:
		{
			for (const std::string& file : e.filesDropped)
			{
				int type = utils::typeFromExtension(file);
				switch (type)
				{
				case NS_TYPE_IMAGE:
					std::cout << "[UI] Image file dropped: " << file << std::endl;
					pushImageNodeFromFile(file, PushMode::AtCursorPosition, true, mouseWorldPos);
					break;
				case NS_TYPE_FONT:
					std::cout << "[UI] Font file dropped: " << file << std::endl;
					pushFontNodeFromFile(file, PushMode::AtCursorPosition, true, mouseWorldPos);
					break;
				default:
					std::cout << "[UI] Unknown file dropped: " << file << std::endl;
					break;
				}
			}
			break;
		}
	}
}

void uiNodeSystem::draw()
{
	renderWindow->setView(theView);

	uiConnections::draw(*renderWindow);
	for (uiNode* n : uiNodeList)
	{
		if (n != nullptr)
			n->draw(*renderWindow);
	}
	nodeSelectionBox.draw(*renderWindow, mouseWorldPos);
}

void uiNodeSystem::setOnNodeSelectedCallback(void (*functionPointer)(int))
{
	onNodeSelectedCallback = functionPointer;
}
void uiNodeSystem::setOnNodeDeletedCallback(void (*functionPointer)(int))
{
	onNodeDeletedCallback = functionPointer;
}

void uiNodeSystem::setOnNodeChangedCallback(void(*functionPointer)(int))
{
	onNodeChangedCallback = functionPointer;
}

std::vector<uiNode*>& uiNodeSystem::getNodeList()
{
	return uiNodeList;
}

int uiNodeSystem::getSelectedNode()
{
	return selectedNodeIndex;
}

void uiNodeSystem::setSelectedNode(int nodeIndex)
{
	if (selectedNodeIndex > -1)
		uiNodeList[selectedNodeIndex]->paintAsUnselected();

	selectedNodeIndex = nodeIndex;
	if (selectedNodeIndex > -1)
	{
		uiNodeList[selectedNodeIndex]->paintAsSelected();
		if (onNodeSelectedCallback != nullptr)
			onNodeSelectedCallback(selectedNodeIndex);
	}
}

void uiNodeSystem::getView(int& zoom, sf::Vector2f& position)
{
	zoom = zoomInt;
	position = viewPosition;
}

void uiNodeSystem::setView(int zoom, const sf::Vector2f& position)
{
	zoomInt = zoom;
	viewPosition = position;
	updateView();
}

void uiNodeSystem::setBoundInputFieldNode(int node)
{
	boundInputFieldNode = node;
}

// called before loading a file
void uiNodeSystem::clearEverything()
{
	nodeSystem::clearEverything();
	uiNodeList.clear();
	uiConnections::clearEverything();
	uiConnectionIdToLogicalConnectionId.clear();
	uiNodeIdToLogicalNodeId.clear();
}

void uiNodeSystem::createConnection(int leftNode, int rightNode, int leftPin, int rightPin)
{
	sf::Vector2f leftPinCoords, rightPinCoords;
	sf::Color connectionColor;

	leftPinCoords = uiNodeList[leftNode]->getPinPosition(leftPin);
	rightPinCoords = uiNodeList[rightNode]->getPinPosition(rightPin);
	connectionColor = uiNodeList[leftNode]->getPinColor(leftPin);

	int uiConnectionId = uiConnections::connect(leftPinCoords, rightPinCoords, leftNode, rightNode, leftPin, rightPin, connectionColor);

	// right side node first
	uiNodeList[rightNode]->attachConnectionPoint(uiConnectionId, rightPin);
	uiNodeList[leftNode]->attachConnectionPoint(uiConnectionId, leftPin);

	int logicalConnectionId = nodeSystem::connect(uiNodeIdToLogicalNodeId[leftNode], uiNodeIdToLogicalNodeId[rightNode], leftPin, rightPin);
	uiConnectionIdToLogicalConnectionId[uiConnectionId] = logicalConnectionId;
}

void uiNodeSystem::clearNodeSelection()
{
	selectedNodeIndex = -1;
}

bool uiNodeSystem::isEmpty()
{
	for (uiNode* n : uiNodeList)
	{
		if (n != nullptr)
			return false;
	}
	return true;
}

void uiNodeSystem::copyNode()
{
	if (selectedNodeIndex < 0 || uiNodeList[selectedNodeIndex] == nullptr)
	{
		std::cout << "[UI] Failed to copy node data\n";
		return;
	}
	std::cout << "[UI] Copying node data\n";
	const std::string& nodeName = uiNodeList[selectedNodeIndex]->getName();
	const nodeData* nodeDataToUse = nodeProvider::getNodeDataByName(nodeName);
	std::string toCopy = "copy node\n" + nodeName + '\n';
	for (int i = 0; i < nodeDataToUse->inputPinCount; i++)
	{
		switch (nodeDataToUse->pinTypes[i])
		{
		case NS_TYPE_INT:
			toCopy += std::to_string(((int*)(uiNodeList[selectedNodeIndex]->getInput(i)))[0]) + "\n";
			break;
		case NS_TYPE_VECTOR2I:
			toCopy += utils::vector2iToString(((sf::Vector2i*)(uiNodeList[selectedNodeIndex]->getInput(i)))[0]) + "\n";
			break;
		case NS_TYPE_FLOAT:
			toCopy += std::to_string(((float*)(uiNodeList[selectedNodeIndex]->getInput(i)))[0]) + "\n";
			break;
		case NS_TYPE_COLOR:
			toCopy += utils::hexStringFromColor(((sf::Color*)(uiNodeList[selectedNodeIndex]->getInput(i)))[0]) + "\n";
			break;
		case NS_TYPE_STRING:
			toCopy += ((std::string*)(uiNodeList[selectedNodeIndex]->getInput(i)))[0] + "\n";
			break;
		case NS_TYPE_IMAGE:
		{
			int flags;
			const void* dataPointer = uiNodeList[selectedNodeIndex]->getInput(i, &flags);
			if (dataPointer == nullptr)
			{
				toCopy += '\n';
				break;
			}
			toCopy += std::to_string(flags) + ' ';
			if (flags == 1)
			{
				std::string base64Image;
				utils::base64StringFromImage(((const sf::RenderTexture*)dataPointer)->getTexture().copyToImage(), base64Image);
				toCopy += base64Image + '\n';
			}
			else
				toCopy += std::string((char*)dataPointer) + '\n';
			break;
		}
		case NS_TYPE_FONT:
		{
			const void* dataPointer = uiNodeList[selectedNodeIndex]->getInput(i);
			if (dataPointer == nullptr)
			{
				toCopy += '\n';
				break;
			}
			toCopy += std::string((char*)dataPointer) + '\n';
			break;
		}
		}
	}
	utils::textToClipboard(toCopy);
}

bool uiNodeSystem::pasteNode()
{
	std::string clipText;
	utils::textFromClipboard(clipText);
	if (!utils::stringStartsWith(clipText, "copy node\n"))
		return false;
	int q = 10, p = 10;
	for (; clipText[p] != '\n'; p++);
	std::string nodeName = clipText.substr(q, p - q);
	const nodeData* nodeDataPointer = nodeProvider::getNodeDataByName(nodeName);
	if (nodeDataPointer == nullptr)
	{
		std::cout << "[UI] Failed to paste node from clipboard, node with name '" + nodeName + "' doesn't exist\n";
		return false;
	}
	int nodeID = uiNodeSystem::pushNewNode(nodeDataPointer);
	int currentPin = 0;
	while (p < clipText.length() - 1)
	{
		q = p + 1;
		for (p = q; clipText[p] != '\n'; p++);

		switch (nodeDataPointer->pinTypes[currentPin])
		{
		case NS_TYPE_INT:
		{
			int value = atoi(clipText.substr(q, p - q).c_str());
			onProjectFileParseNodeInput(nodeID, currentPin, &value);
			break;
		}
		case NS_TYPE_VECTOR2I:
		{
			sf::Vector2i value;
			utils::vector2iFromString(clipText.substr(q, p - q), value);
			onProjectFileParseNodeInput(nodeID, currentPin, &value);
			break;
		}
		case NS_TYPE_FLOAT:
		{
			float value = atof(clipText.substr(q, p - q).c_str());
			onProjectFileParseNodeInput(nodeID, currentPin, &value);
			break;
		}
		case NS_TYPE_COLOR:
		{
			sf::Color value;
			utils::colorFromHexString(clipText.substr(q, p - q), value);
			onProjectFileParseNodeInput(nodeID, currentPin, &value);
			break;
		}
		case NS_TYPE_STRING:
		{
			std::string value = clipText.substr(q, p - q);
			onProjectFileParseNodeInput(nodeID, currentPin, &value);
			break;
		}
		case NS_TYPE_FONT:
		{
			if (q == p)
				break;
			std::string path = clipText.substr(q, p - q);
			onProjectFileParseNodeInput(nodeID, currentPin, &path);
			break;
		}
		case NS_TYPE_IMAGE:
		{
			if (q == p)
				break;
			std::string value = clipText.substr(q + 2, p - q - 2);
			if (clipText[q] == '1')
			{
				sf::Image copiedImage;
				utils::imageFromBase64String(value, copiedImage);
				onProjectFileParseNodeInput(nodeID, currentPin, &copiedImage, 1);
			}
			else
				onProjectFileParseNodeInput(nodeID, currentPin, &value);
			break;
		}
		}
		currentPin++;
	}
	return true;
}

int uiNodeSystem::getLogicalNodeId(int uiNodeId)
{
	return uiNodeIdToLogicalNodeId[uiNodeId];
}

// ---------------
// project loading
// ---------------

void uiNodeSystem::onProjectFileStartParsing()
{
	clearNodeSelection(); // unselect if there is a node selected
	clearEverything(); // remove all nodes and connections
}

void uiNodeSystem::onProjectFileParseNode(const std::string& nodeName, float coordinatesX, float coordinatesY)
{
	const nodeData* dataForNewNode = nodeProvider::getNodeDataByName(nodeName);
	if (dataForNewNode == nullptr)
	{
		std::cout << "[UI] Node not found for name: " + nodeName + "\n";
		return;
	}
	pushNewNode(
		dataForNewNode,
		PushMode::AtPosition,
		false,
		{ coordinatesX , coordinatesY }
	);
}

void uiNodeSystem::onProjectFileParseNodeInput(int nodeIndex, int pinIndex, void* data, int flags)
{
	std::vector<uiNode*>& nodes = getNodeList();
	int targetNodeIndex = nodeIndex < 0 ? (nodes.size() + nodeIndex) : nodeIndex;
	setBoundInputFieldNode(targetNodeIndex);
	uiNode* targetNode = nodes[targetNodeIndex];
	targetNode->setInput(pinIndex, data, flags);
}

void uiNodeSystem::onProjectFileParseConnection(int nodeAIndex, int pinAIndex, int nodeBIndex, int pinBIndex)
{
	createConnection(nodeAIndex, nodeBIndex, pinAIndex, pinBIndex);
}

void uiNodeSystem::onProjectFileParseNodeEditorState(int selectedNode, int nodeEditorZoom,
			float nodeEditorViewPositionX, float nodeEditorViewPositionY)
{
	uiNodeSystem::setSelectedNode(selectedNode);
	uiNodeSystem::setView(nodeEditorZoom, { nodeEditorViewPositionX, nodeEditorViewPositionY });
}