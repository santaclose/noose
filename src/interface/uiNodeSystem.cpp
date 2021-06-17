#include "uiNodeSystem.h"

#include "uiConnections.h"
#include "uiSelectionBox.h"
#include "../logic/nodeSystem.h"
#include "../type2color.h"

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

	void onInputFieldValueChanged();
	void deleteLine(int lineToDelete);
	void updateView();
	int findSlotForNode();
}

void uiNodeSystem::onInputFieldValueChanged()
{
	nodeSystem::onNodeChanged(boundInputFieldNode);
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

int uiNodeSystem::findSlotForNode()
{
	int i = 0;
	while (true)
	{
		if (i == uiNodeList.size())
		{
			uiNodeList.resize(i + 1);
			// no need to set the slot to nullptr since were adding a node to it
			break;
		}
		if (uiNodeList[i] == nullptr)
			break;
		i++;
	}
	return i;
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

	int newNodeID = findSlotForNode();
	nodeSystem::onNodeCreated(newNodeID, nData);
#ifdef TEST
	std::cout << "onNodeCreated:\n\tnode id: " << newNodeID << "\n\tnode name: " << nData->nodeName << std::endl;
#endif

	uiNodeList[newNodeID] =
		new uiNode(
			nData,
			nodeCenterInPosition,
			worldPos,
			nodeSystem::getDataPointersForNode(newNodeID),
			onInputFieldValueChanged,
			&nodeSelectionBox);

	if (selectedNodeIndex == -1)
	{
		selectedNodeIndex = newNodeID;
		uiNodeList[newNodeID]->paintAsSelected();
		if (onNodeSelectedCallback != nullptr)
			onNodeSelectedCallback(newNodeID);
	}
	return newNodeID;
}

int uiNodeSystem::pushImageNodeFromFile(const std::string& filePath, PushMode mode, bool nodeCenterInPosition, sf::Vector2f worldPos)
{
	int nodeID = uiNodeSystem::pushNewNode(&nodeProvider::nodeDataList[0], mode, nodeCenterInPosition, worldPos);
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
			if (e.mouseButton.button == sf::Mouse::Left)
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
			else if (e.mouseButton.button == sf::Mouse::Right)
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
			else if (e.mouseButton.button == sf::Mouse::Middle)
			{
				panning = true;
				lastMouseScreenPos = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
			}
			break;
		}
		case sf::Event::MouseButtonReleased:
		{
			if (e.mouseButton.button == sf::Mouse::Middle)
			{
				panning = false;
			}
			else if (e.mouseButton.button == sf::Mouse::Left)
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
							canConnect &= nodeSystem::isConnectionValid(leftSideNode, rightSideNode, leftSidePin, rightSidePin);

							if (!canConnect) // failed to connect
							{
								uiConnections::hideTemporary();
								break;
							}
							
							// update interface lines
							// node indices are flipped if necessary such that node a is on the left side
							int connectionIndex = uiConnections::connect(
								uiNodeList[i]->getPinPosition(index),
								connectionStartedOnRightSideNode ? leftSideNode : rightSideNode,
								connectionStartedOnRightSideNode ? leftSidePin : rightSidePin);

							// right side node first
							uiNodeList[rightSideNode]->attachConnectionPoint(connectionIndex, rightSidePin);
							uiNodeList[leftSideNode]->attachConnectionPoint(connectionIndex, leftSidePin);

							nodeSystem::onNodesConnected(leftSideNode, rightSideNode, leftSidePin, rightSidePin, connectionIndex);
#ifdef TEST
	std::cout << "onNodesConnected:\n\tleft side node: " << leftSideNode << "\n\tright side node: " << rightSideNode << "\n\tleft side pin: " << leftSidePin << "\n\tright side pin: " << rightSidePin << "\n\tconnection index: " << connectionIndex << std::endl;
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
			else if (e.mouseButton.button == sf::Mouse::Right)
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
				int lineToRemove = uiConnections::onTryingToRemove(mouseWorldPos);
				if (lineToRemove > -1) // mouse was over a line and we have to detach it from nodeA and nodeB
				{
					deleteLine(lineToRemove);

					int nA, nB, pA, pB;
					uiConnections::getNodesForLine(lineToRemove, nA, nB);
					uiConnections::getPinsForLine(lineToRemove, pA, pB);
					nodeSystem::onNodesDisconnected(nA, nB, pA, pB, lineToRemove);
#ifdef TEST
	std::cout << "onNodesDisconnected:\n\tleft side node: " << nA << "\n\tright side node: " << nB << "\n\tleft side pin: " << pA << "\n\tright side pin: " << pB << "\n\tconnection index: " << lineToRemove << std::endl;
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
			switch (e.key.code)
			{
				case sf::Keyboard::BackSpace:
				case sf::Keyboard::Delete:
				{
					// can't delete if node is not selected or any node being moved
					if (selectedNodeIndex < 0 || draggingNodeIndex > -1 || uiInputField::isBound())
						return;

					// a copy is needed
					std::vector<int> nodeLines = uiNodeList[selectedNodeIndex]->getConnectedLines();

					for (int l : nodeLines)
						deleteLine(l);

					onNodeDeletedCallback(selectedNodeIndex);

					delete uiNodeList[selectedNodeIndex];
					uiNodeList[selectedNodeIndex] = nullptr;

					nodeSystem::onNodeDeleted(selectedNodeIndex, nodeLines);//linesToDelete, lineCount);
#ifdef TEST
					std::cout << "onNodeDeleted:\n\tselected node index: " << selectedNodeIndex << "\n\tnode lines: ";
					for (int l : nodeLines)
						std::cout << l << ", ";
					std::cout << std::endl;
#endif

					selectedNodeIndex = -1;
					break;
				}
				case sf::Keyboard::F:
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
			uiInputField::keyboardInput(e.text.unicode);
			break;
		}
		case sf::Event::FilesDropped:
		{
			std::cout << "Files dropped:\n";
			for (const auto& file : e.filesDropped)
			{
				std::cout << "    - " << file << std::endl;
				pushImageNodeFromFile(file, PushMode::AtCursorPosition, true, mouseWorldPos);
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
}

void uiNodeSystem::createConnection(int leftNode, int rightNode, int leftPin, int rightPin)
{
	sf::Vector2f leftPinCoords, rightPinCoords;
	sf::Color connectionColor;

	leftPinCoords = uiNodeList[leftNode]->getPinPosition(leftPin);
	rightPinCoords = uiNodeList[rightNode]->getPinPosition(rightPin);
	connectionColor = uiNodeList[leftNode]->getPinColor(leftPin);

	int connectionIndex = uiConnections::connect(leftPinCoords, rightPinCoords, leftNode, rightNode, leftPin, rightPin, connectionColor);

	// right side node first
	uiNodeList[rightNode]->attachConnectionPoint(connectionIndex, rightPin);
	uiNodeList[leftNode]->attachConnectionPoint(connectionIndex, leftPin);

	nodeSystem::onNodesConnected(leftNode, rightNode, leftPin, rightPin, connectionIndex);
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
