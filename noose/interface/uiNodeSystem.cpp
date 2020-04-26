#include "uiNodeSystem.h"

#include "uiNode.h"
#include "uiConnections.h"
#include "uiSelectionBox.h"
#include "../logic/nodeSystem.h"

#include <iostream>
#include <vector>
#include <math.h>

#define MAX_ZOOM 5
#define MIN_ZOOM 20

sf::RenderWindow* nsRenderWindow;
const sf::Vector2i* nsMouseScreenPosPointer;
sf::Vector2f nsMouseWorldPos;
uiSelectionBox nodeSystemSelectionBox;

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

void (*onNodeSelectedCallback)(int) = nullptr;
void (*onNodeDeletedCallback)(int) = nullptr;
void (*onNodeChangedCallback)(int) = nullptr;

int boundInputFieldNode = -1;

#ifdef TEST
std::ostream* testStream;
#endif

void onInputFieldValueChanged()
{
	nodeSystem::onNodeChanged(boundInputFieldNode);
#ifdef TEST
	(*testStream) << "onNodeChanged:\n\tbound input field node: " << boundInputFieldNode << std::endl;
#endif
	onNodeChangedCallback(boundInputFieldNode);
}

void deleteLine(int lineToDelete)
{
	int nA, nB;
	uiConnections::getNodesForLine(lineToDelete, nA, nB);

	uiNodeList[nA]->setLineIndexAsDisconnected(lineToDelete);
	uiNodeList[nB]->setLineIndexAsDisconnected(lineToDelete);

	//std::cout << "hiding line " << lineToDelete << std::endl;
	uiConnections::hide(lineToDelete);
}

inline void updateView()
{
	theView = sf::View(viewPosition, (sf::Vector2f)nsRenderWindow->getSize());
	theView.zoom(currentZoom);
}

void uiNodeSystem::initialize(sf::RenderWindow& theRenderWindow, const sf::Vector2i* mouseScreenPosPointer)
{
	uiConnections::initialize(currentZoom);
	nodeSystem::initialize();
	nsRenderWindow = &theRenderWindow;
	nsMouseScreenPosPointer = mouseScreenPosPointer;
	updateView();
	nodeSystemSelectionBox.initialize();
}

void uiNodeSystem::terminate()
{
	for (uiNode* n : uiNodeList)
	{
		if (n != nullptr)
			delete n;
	}
	nodeSystemSelectionBox.terminate();
	nodeSystem::terminate();
}

int findSlotForNode()
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

void uiNodeSystem::pushNewNode(const nodeData* nData, PushMode mode)
{
	nsRenderWindow->setView(theView);
	sf::Vector2f worldPos;
	switch (mode)
	{
	case Centered:
		worldPos = nsRenderWindow->mapPixelToCoords(sf::Vector2i(nsRenderWindow->getSize().x / 2, nsRenderWindow->getSize().y / 2));
		break;
	case OnCursorPosition:
		worldPos = nsRenderWindow->mapPixelToCoords(*nsMouseScreenPosPointer);
		break;
	}

	int newNodeID = findSlotForNode();
	nodeSystem::onNodeCreated(newNodeID, nData);
#ifdef TEST
	(*testStream) << "onNodeCreated:\n\tnode id: " << newNodeID << "\n\tnode name: " << nData->nodeName << std::endl;
#endif

	uiNodeList[newNodeID] = new uiNode(nData, worldPos, nodeSystem::getDataPointersForNode(newNodeID), onInputFieldValueChanged, &nodeSystemSelectionBox);

	if (selectedNodeIndex == -1)
	{
		selectedNodeIndex = newNodeID;
		uiNodeList[newNodeID]->paintAsSelected();
		if (onNodeSelectedCallback != nullptr)
			onNodeSelectedCallback(newNodeID);

	}
}

void uiNodeSystem::onPollEvent(const sf::Event& e)
{
	nsRenderWindow->setView(theView);
	nsMouseWorldPos = nsRenderWindow->mapPixelToCoords(*nsMouseScreenPosPointer);

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
				if (uiInputField::onMouseDown(nsMouseWorldPos))
					return;

				// collision from top to bottom
				for (int i = uiNodeList.size() - 1; i > -1; i--)
				{
					if (uiNodeList[i] == nullptr)
						continue;

					// index stores pin or inputfield index
					// subindex stores subinputfield index
					int index, subIndex;
					uiNode::MousePos mp = uiNodeList[i]->mouseOver(nsMouseWorldPos, index, subIndex);

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
								nsMouseWorldPos,
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
						uiNodeList[i]->bindInputField(index, subIndex);
						return;
					}
					}
					break; // don't allow to click through nodes
				}
			}
			else if (e.mouseButton.button == sf::Mouse::Right)
			{
				// collision from top to bottom
				for (int i = uiNodeList.size() - 1; i > -1; i--)
				{
					if (uiNodeList[i] == nullptr)
						continue;

					// index stores pin or inputfield index
					// subindex stores subinputfield index
					int index, subIndex;
					uiNode::MousePos mp = uiNodeList[i]->mouseOver(nsMouseWorldPos, index, subIndex);

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
							onNodeSelectedCallback(i);
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
						if (uiNodeList[i]->mouseOver(nsMouseWorldPos, index, subIndex) == uiNode::MousePos::Pin)
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
	(*testStream) << "onNodesConnected:\n\tleft side node: " << leftSideNode << "\n\tright side node: " << rightSideNode << "\n\tleft side pin: " << leftSidePin << "\n\tright side pin: " << rightSidePin << "\n\tconnection index: " << connectionIndex << std::endl;
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
				int lineToRemove = uiConnections::onTryingToRemove(nsMouseWorldPos);
				if (lineToRemove > -1) // mouse was over a line and we have to detach it from nodeA and nodeB
				{
					deleteLine(lineToRemove);

					int nA, nB, pA, pB;
					uiConnections::getNodesForLine(lineToRemove, nA, nB);
					uiConnections::getPinsForLine(lineToRemove, pA, pB);
					nodeSystem::onNodesDisconnected(nA, nB, pA, pB, lineToRemove);
#ifdef TEST
	(*testStream) << "onNodesDisconnected:\n\tleft side node: " << nA << "\n\tright side node: " << nB << "\n\tleft side pin: " << pA << "\n\tright side pin: " << pB << "\n\tconnection index: " << lineToRemove << std::endl;
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
			currentZoom = zoomInt / 10.0f;
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
					(*testStream) << "onNodeDeleted:\n\tselected node index: " << selectedNodeIndex << "\n\tnode lines: ";
					for (int l : nodeLines)
						(*testStream) << l << ", ";
					(*testStream) << std::endl;
#endif

					selectedNodeIndex = -1;
					break;
				}
			}
		}
		case sf::Event::TextEntered:
		{
			uiInputField::keyboardInput(e.text.unicode);
		}
	}
}

void uiNodeSystem::draw()
{
	nsRenderWindow->setView(theView);

	for (uiNode* n : uiNodeList)
	{
		if (n != nullptr)
			n->draw(*nsRenderWindow);
	}

	uiConnections::draw(*nsRenderWindow);
	nodeSystemSelectionBox.draw(*nsRenderWindow, nsMouseWorldPos);
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

#ifdef TEST
void uiNodeSystem::initialize(sf::RenderWindow& theRenderWindow, const sf::Vector2i* mouseScreenPosPointer, std::ostream& stream)
{
	uiConnections::initialize(currentZoom);
	nodeSystem::initialize();
	nsRenderWindow = &theRenderWindow;
	nsMouseScreenPosPointer = mouseScreenPosPointer;
	updateView();
	nodeSystemSelectionBox.initialize();
	testStream = &stream;
}
#endif