#include "nodeSystem.h"

#include "node_system/uiNode.h"
#include "node_system/logicalNode.h"
#include "node_system/uiConnections.h"
#include "node_system/logicalConnections.h"

#include "dataController.h"

#include <iostream>
#include <vector>
#include <math.h>

#define MAX_ZOOM 5
#define MIN_ZOOM 30

namespace nodeSystem
{
	struct node {
		uiNode* g;
		logicalNode* l;

		node(const void* nodeData, sf::Vector2f& initialPosition, void(onValueChangedFunc)())
		{
			l = new logicalNode(nodeData);
			g = new uiNode(nodeData, initialPosition, l->getDataPointers(), onValueChangedFunc);
		}
		~node()
		{
			delete g;
			delete l;
		}
	};

	// public
	float currentZoom = 1.0f;
	//sf::Font font;
	sf::RenderWindow* renderWindow;

	// private
	std::vector<node*> nodeList;
	int selectedNodeIndex = -1;
	int draggingNodeIndex = -1;

	bool panning = false;

	bool creatingConnection = false;
	int connectionTempNode;
	int connectionTempPin;

	bool removingConnections = false;

	sf::Vector2f lastMouseScreenPos;
	sf::Vector2f currentMouseScreenPos;

	sf::View theView; // view with pan and zoom transformations
	sf::Vector2f viewPosition;
	int zoomInt = 10;

	void (*onNodeSelectedCallback)(logicalNode&) = nullptr;
	void (*onNodeDeletedCallback)(logicalNode&) = nullptr;

	logicalNode* boundInputFieldNode = nullptr;


	void onInputFieldValueChanged()
	{
		//std::cout << "value changed in node\n";
		boundInputFieldNode->activate();
	}

	void deleteLine(int lineToDelete)
	{
		int nA = logicalConnections::getNodeA(lineToDelete);
		int nB = logicalConnections::getNodeB(lineToDelete);

		nodeList[nA]->l->disconnect(lineToDelete);
		nodeList[nB]->l->disconnect(lineToDelete);

		nodeList[nA]->g->setLineIndexAsDisconnected(lineToDelete);
		nodeList[nB]->g->setLineIndexAsDisconnected(lineToDelete);

		uiConnections::hide(lineToDelete);
		logicalConnections::deleteConnection(lineToDelete);
	}
}

inline void updateView()
{
	nodeSystem::theView = sf::View(nodeSystem::viewPosition, (sf::Vector2f)nodeSystem::renderWindow->getSize());
	nodeSystem::theView.zoom(nodeSystem::currentZoom);
}

void nodeSystem::initialize(sf::RenderWindow& theRenderWindow)
{
	uiConnections::initialize();
	renderWindow = &theRenderWindow;
	//font.loadFromFile("res/fonts/Lato/Lato-Regular.ttf");
	updateView();
}

void nodeSystem::terminate()
{
	for (node* n : nodeList)
	{
		delete n;
	}
}

void nodeSystem::pushNewNode(const void* nodeData, sf::Vector2i& initialScreenPosition)
{
	renderWindow->setView(theView);
	sf::Vector2f worldPos = renderWindow->mapPixelToCoords(initialScreenPosition);
	nodeList.push_back(new node(nodeData, worldPos, onInputFieldValueChanged));
}

void nodeSystem::onPollEvent(const sf::Event& e, sf::Vector2i& mousePos)
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
					// index stores pin or inputfield index
					// subindex stores subinputfield index
					int index, subIndex;
					uiNode::MousePos mp = nodeList[i]->g->mouseOver(mouseWorldPos, index, subIndex);

					if (mp == uiNode::MousePos::Outside)
						continue;

					switch (mp)
					{
					case uiNode::MousePos::TopBar: // drag node
					{
						std::cout << "mouse over topbar\n";
						draggingNodeIndex = i;
						lastMouseScreenPos = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
						break;
					}
					case uiNode::MousePos::Pin: // mouse is over a pin
					{
						std::cout << "mouse over pin\n";
						// pin cannot be connected and be an input pin
						bool canCreateConnection = (nodeList[i]->l->canConnectToPin(index));
						std::cout << "canCreateConnection to pin "<< index<<": " << canCreateConnection << std::endl;

						if (canCreateConnection)
						{
							// global state
							creatingConnection = true;
							connectionTempNode = i;
							connectionTempPin = index;

							// temporary line
							uiConnections::createTemporary(nodeList[i]->g->getPinPosition(index), mouseWorldPos, nodeList[i]->g->getPinColor(index), index < nodeList[i]->l->getInputPinCount());
							//uiNodeConnections::push(pinPosition, mouseWorldPos, pinColor, nodeList[i], index, direction);
						}
						break;
					}
					case uiNode::MousePos::InputField: // mouse is over an input field
					{
						std::cout << "mouse over inputField\n";
						boundInputFieldNode = nodeList[i]->l;
						nodeList[i]->g->bindInputField(index, subIndex);
						break;
					}
					}
					std::cout << "clicked inside node\n";
					break; // don't allow to click through nodes
				}
			}
			else if (e.mouseButton.button == sf::Mouse::Right)
			{
				bool mouseWasOverTopBar = false;

				// collision from top to bottom
				for (int i = nodeList.size() - 1; i > -1; i--)
				{
					//std::cout << i << "\n";
					if (nodeList[i]->g->mouseOverTopBar(mouseWorldPos))
					{
						// node selection
						if (i == selectedNodeIndex)
						{
							nodeList[i]->g->paintAsUnselected();
							selectedNodeIndex = -1;
							break;
						}

						if (selectedNodeIndex > -1)
							nodeList[selectedNodeIndex]->g->paintAsUnselected();

						nodeList[i]->g->paintAsSelected();
						selectedNodeIndex = i;
						mouseWasOverTopBar = true;
						if (onNodeSelectedCallback != nullptr)
							onNodeSelectedCallback(*(nodeList[i]->l));
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
						int index, subIndex;
						if (nodeList[i]->g->mouseOver(mouseWorldPos, index, subIndex) == uiNode::MousePos::Pin)
						{
							// can't connect two lines to an input pin
							if (!nodeList[i]->l->canConnectToPin(index))
							{
								uiConnections::hideTemporary();
								break;
							}

							int nodeIndexA = connectionTempNode;
							int nodeIndexB = i;
							int pinA = connectionTempPin;
							int pinB = index;

							// node indices are flipped if necessary such that node a is on the left side
							int connectionIndex = logicalConnections::tryToConnect(nodeList[connectionTempNode]->l, pinA, nodeList[i]->l, pinB, nodeIndexA, nodeIndexB);
							if (connectionIndex == -1) // failed to connect
							{
								uiConnections::hideTemporary();
								break;
							}
							
							//connect right side node before
							nodeList[nodeIndexB]->l->connect(connectionIndex);
							nodeList[nodeIndexA]->l->connect(connectionIndex);
							nodeList[nodeIndexA]->l->activate();

							// logically connected, update interface lines

							uiConnections::connect(nodeList[i]->g->getPinPosition(index), connectionIndex);
							nodeList[nodeIndexB]->g->attachConnectionPoint(connectionIndex, pinB);
							nodeList[nodeIndexA]->g->attachConnectionPoint(connectionIndex, pinA);

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
				nodeList[draggingNodeIndex]->g->displace(displacement * currentZoom); // pass node index so it can move the lines
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
					recalculatePropagationMatrices();
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
					int* linesToDelete = nodeList[selectedNodeIndex]->g->getConnectedLinesInfo(lineCount);

					for (int i = 0; i < lineCount; i++)
						deleteLine(linesToDelete[i]);
					delete[] linesToDelete; // free memory

					onNodeDeletedCallback(*(nodeList[selectedNodeIndex]->l));
					nodeList.erase(nodeList.begin() + selectedNodeIndex);

					recalculatePropagationMatrices();
					selectedNodeIndex = -1;
					break;
				}
				case sf::Keyboard::Q:
				{
					//printAllSystem();
					break;
				}
			}
		}
	}
}

void nodeSystem::draw()
{
	renderWindow->setView(theView);

	for (node* n : nodeList)
		n->g->draw(*renderWindow);

	uiConnections::draw(*renderWindow);
}

void nodeSystem::setOnNodeSelectedCallback(void* functionPointer)
{
	onNodeSelectedCallback = (void (*)(logicalNode&))functionPointer;
}
void nodeSystem::setOnNodeDeletedCallback(void* functionPointer)
{
	onNodeDeletedCallback = (void (*)(logicalNode&))functionPointer;
}

void nodeSystem::deselectNode()
{
	if (selectedNodeIndex > -1)
	{
		nodeList[selectedNodeIndex]->g->paintAsUnselected();
		selectedNodeIndex = -1;
	}
}

void nodeSystem::recalculatePropagationMatrices()
{
	for (node* n : nodeList)
		n->l->clearPropagationMatrix();

	int i = 0;
	for (logicalConnection& l : logicalConnections::connections)
	{
		if (!l.deleted) // not deleted
		{
			l.nodeA->rebuildMatrices(i);
			l.nodeB->rebuildMatrices(i);
		}
		i++;
	}
}
