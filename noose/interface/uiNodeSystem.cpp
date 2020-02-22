#include "uiNodeSystem.h"

#include "uiNode.h"
#include "uiConnections.h"
#include "../logic/nodeSystem.h"

#include <iostream>
#include <vector>
#include <math.h>

#define MAX_ZOOM 5
#define MIN_ZOOM 30

float currentZoom = 1.0f;
sf::RenderWindow* renderWindow;

std::vector<uiNode*> nodeList;
int selectedNodeIndex = -1;
int draggingNodeIndex = -1;

bool panning = false;

bool creatingConnection = false;
int connectionTempNode;
int connectionTempPin;

bool removingConnections = false;

sf::Vector2f lastMouseScreenPos;
sf::Vector2f currentMouseScreenPos;

sf::View theView; // view with panning and zoom transformations
sf::Vector2f viewPosition;
int zoomInt = 10;

void (*onNodeSelectedCallback)(int) = nullptr;
void (*onNodeDeletedCallback)(int) = nullptr;

int boundInputFieldNode = -1;

//sf::CircleShape debugCircle;

void onInputFieldValueChanged()
{
	nodeSystem::onNodeChanged(boundInputFieldNode);
}

void deleteLine(int lineToDelete)
{
	int nA, nB;
	uiConnections::getNodesForLine(lineToDelete, nA, nB);

	nodeList[nA]->setLineIndexAsDisconnected(lineToDelete);
	nodeList[nB]->setLineIndexAsDisconnected(lineToDelete);

	uiConnections::hide(lineToDelete);
}

inline void updateView()
{
	theView = sf::View(viewPosition, (sf::Vector2f)renderWindow->getSize());
	theView.zoom(currentZoom);
}

void uiNodeSystem::initialize(sf::RenderWindow& theRenderWindow)
{
	/*debugCircle.setRadius(2.0);
	debugCircle.setFillColor(sf::Color(255, 0, 0, 255));*/

	uiConnections::initialize();
	renderWindow = &theRenderWindow;
	updateView();
}

void uiNodeSystem::terminate()
{
	for (uiNode* n : nodeList)
	{
		delete n;
	}
}

int findSlotForNode()
{
	int i = 0;
	for (; i < nodeList.size(); i++)
	{
		if (nodeList[i] == nullptr)
			return i;
	}
	nodeList.push_back(nullptr);
	return i;
}

void uiNodeSystem::pushNewNode(const void* nodeData, sf::Vector2i& initialScreenPosition)
{
	renderWindow->setView(theView);
	sf::Vector2f worldPos = renderWindow->mapPixelToCoords(initialScreenPosition);

	int newNodeID = findSlotForNode();
	nodeSystem::onNodeCreated(newNodeID, nodeData);

	nodeList[newNodeID] = new uiNode(nodeData, worldPos, nodeSystem::getDataPointersForNode(newNodeID), onInputFieldValueChanged);
}

void uiNodeSystem::onPollEvent(const sf::Event& e, sf::Vector2i& mousePos)
{
	renderWindow->setView(theView);
	sf::Vector2f mouseWorldPos = renderWindow->mapPixelToCoords(mousePos);

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
				// collision from top to bottom
				for (int i = nodeList.size() - 1; i > -1; i--)
				{
					if (nodeList[i] == nullptr)
						continue;

					// index stores pin or inputfield index
					// subindex stores subinputfield index
					int index, subIndex;
					uiNode::MousePos mp = nodeList[i]->mouseOver(mouseWorldPos, index, subIndex);

					if (mp == uiNode::MousePos::Outside)
						continue;

					switch (mp)
					{
					case uiNode::MousePos::TopBar: // drag node
					{
						//std::cout << "mouse over topbar\n";
						draggingNodeIndex = i;
						lastMouseScreenPos = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
						break;
					}
					case uiNode::MousePos::Pin: // mouse is over a pin
					{
						//std::cout << "mouse over pin\n";

						//if (nodeSystem::canConnectToPin(i, index))
						if (nodeList[i]->canConnectToPin(index))
						{
							// global state
							creatingConnection = true;
							connectionTempNode = i;
							connectionTempPin = index;

							// temporary line
							uiConnections::createTemporary(
								nodeList[i]->getPinPosition(index),
								mouseWorldPos,
								nodeList[i]->getPinColor(index),
								index < nodeList[i]->getInputPinCount(),
								connectionTempNode,
								connectionTempPin
							);
							//uiNodeConnections::push(pinPosition, mouseWorldPos, pinColor, nodeList[i], index, direction);
						}
						break;
					}
					case uiNode::MousePos::InputField: // mouse is over an input field
					{
						//std::cout << "mouse over inputField\n";
						boundInputFieldNode = i;
						nodeList[i]->bindInputField(index, subIndex);
						break;
					}
					}
					//std::cout << "clicked inside node\n";
					break; // don't allow to click through nodes
				}
			}
			else if (e.mouseButton.button == sf::Mouse::Right)
			{
				bool mouseWasOverTopBar = false;

				// collision from top to bottom
				for (int i = nodeList.size() - 1; i > -1; i--)
				{
					if (nodeList[i] == nullptr)
						continue;

					//std::cout << i << "\n";
					if (nodeList[i]->mouseOverTopBar(mouseWorldPos))
					{
						// node selection
						if (i == selectedNodeIndex)
						{
							nodeList[i]->paintAsUnselected();
							selectedNodeIndex = -1;
							break;
						}

						if (selectedNodeIndex > -1)
							nodeList[selectedNodeIndex]->paintAsUnselected();

						nodeList[i]->paintAsSelected();
						selectedNodeIndex = i;
						mouseWasOverTopBar = true;
						if (onNodeSelectedCallback != nullptr)
							onNodeSelectedCallback(i);
						break;
					}
				}
				// mouse was not over any top bar
				if (!mouseWasOverTopBar)
				{
					removingConnections = true;
					lastMouseScreenPos = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
				}
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
					int i = nodeList.size() - 1;

					for (; i > -1; i--)
					{
						if (nodeList[i] == nullptr)
							continue;

						int index, subIndex;
						if (nodeList[i]->mouseOver(mouseWorldPos, index, subIndex) == uiNode::MousePos::Pin)
						{
							if (!nodeList[i]->canConnectToPin(index))
							{
								uiConnections::hideTemporary();
								break;
							}

							int nodeIndexA = connectionTempNode;
							int nodeIndexB = i;
							int pinA = connectionTempPin;
							int pinB = index;
														
							// can't be both output or input
							bool canConnect = (pinA < nodeList[nodeIndexA]->getInputPinCount()) != (pinB < nodeList[nodeIndexB]->getInputPinCount());
							canConnect &= nodeSystem::isConnectionValid(nodeIndexA, nodeIndexB, pinA, pinB);

							if (!canConnect) // failed to connect
							{
								uiConnections::hideTemporary();
								break;
							}
							
							// update interface lines

							// node indices are flipped if necessary such that node a is on the left side
							int connectionIndex = uiConnections::connect(nodeList[i]->getPinPosition(index), nodeIndexB, pinB);

							// update indices in case they were flipped
							uiConnections::getNodesForLine(connectionIndex, nodeIndexA, nodeIndexB);
							uiConnections::getPinsForLine(connectionIndex, pinA, pinB);

							nodeList[nodeIndexB]->attachConnectionPoint(connectionIndex, pinB);
							nodeList[nodeIndexA]->attachConnectionPoint(connectionIndex, pinA);

							nodeSystem::onNodesConnected(nodeIndexA, nodeIndexB, pinA, pinB, connectionIndex);

							break;
						}
					}

					// i is equal to -1 when the for loop ends and we know the mouse was not over any pin cuz we break
					if (i < 0) // mouse was not over any pin
						uiConnections::hideTemporary();
				}

				draggingNodeIndex = -1;
				creatingConnection = false;
				uiInputField::unbind();
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
				nodeList[draggingNodeIndex]->displace(displacement * currentZoom); // pass node index so it can move the lines
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
					//recalculatePropagationMatrices();
				}
			}
		
			uiInputField::onMouseMoved(displacement);
			lastMouseScreenPos = currentMouseScreenPos;
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
				// TODO: check if user is using the search bar
				case sf::Keyboard::BackSpace:
				case sf::Keyboard::Delete:
				{
					// can't delete if node is not selected or any node being moved
					if (selectedNodeIndex < 0 || draggingNodeIndex > -1)
						return;

					int lineCount;
					int* linesToDelete = nodeList[selectedNodeIndex]->getConnectedLinesInfo(lineCount);

					for (int i = 0; i < lineCount; i++)
						deleteLine(linesToDelete[i]);

					onNodeDeletedCallback(selectedNodeIndex);

					delete nodeList[selectedNodeIndex];
					nodeList[selectedNodeIndex] = nullptr;

					nodeSystem::onNodeDeleted(selectedNodeIndex, linesToDelete, lineCount);

					delete[] linesToDelete; // free memory
					selectedNodeIndex = -1;
					break;
				}
			}
		}
	}
}

void uiNodeSystem::draw()
{
	renderWindow->setView(theView);

	for (uiNode* n : nodeList)
	{
		if (n != nullptr)
			n->draw(*renderWindow);
	}

	uiConnections::draw(*renderWindow);

	//renderWindow->draw(debugCircle);
}

void uiNodeSystem::setOnNodeSelectedCallback(void (*functionPointer)(int))
{
	onNodeSelectedCallback = functionPointer;
}
void uiNodeSystem::setOnNodeDeletedCallback(void (*functionPointer)(int))
{
	onNodeDeletedCallback = functionPointer;
}

void uiNodeSystem::deselectNode()
{
	if (selectedNodeIndex > -1)
	{
		nodeList[selectedNodeIndex]->paintAsUnselected();
		selectedNodeIndex = -1;
	}
}
