#include "uiNodeSystem.h"
#include "uiNode.h"
#include "uiNodeConnections.h"

#include <iostream>
#include <vector>
#include <math.h>

#define MAX_ZOOM 5
#define MIN_ZOOM 30

namespace uiNodeSystem {

	// public
	float currentZoom = 1.0f;
	sf::Font font;
	sf::RenderWindow* renderWindow;

	// private
	std::vector<uiNode*> nodeList;
	int selectedNodeIndex = -1;
	int draggingNodeIndex = -1;

	bool panning = false;

	//uiNodeConnections nodeConnections;
	bool creatingConnection = false;
	bool removingConnections = false;

	sf::Vector2f lastMouseScreenPos;
	sf::Vector2f currentMouseScreenPos;

	sf::View theView; // view with pan and zoom transformations
	sf::Vector2f viewPosition;
	int zoomInt = 10;
}

void printAllSystem()
{
	std::cout << "-----------------------\nnodes:\n";
	for (uiNode* node : uiNodeSystem::nodeList)
	{
		node->print();
	}
	std::cout << "connections:\n";
	uiNodeConnections::printAllConnections();
}

inline void updateView()
{
	uiNodeSystem::theView = sf::View(uiNodeSystem::viewPosition, (sf::Vector2f)uiNodeSystem::renderWindow->getSize());
	uiNodeSystem::theView.zoom(uiNodeSystem::currentZoom);
}

void uiNodeSystem::initialize(sf::RenderWindow& theRenderWindow)
{
	uiNodeConnections::init();
	//std::cout << "node list size: " << nodeList.size() << std::endl;
	renderWindow = &theRenderWindow;
	font.loadFromFile("res/fonts/Lato/Lato-Regular.ttf");
	updateView();
}

void uiNodeSystem::terminate()
{
	for (uiNode* n : nodeList)
		delete n;
	nodeList.clear();
}

void uiNodeSystem::pushNewNode(const std::string& name, int numberOfInputPins, int numberOfOutputPins, const uiNodeSystem::Types* pinTypes, const std::string* pinNames, sf::Vector2i* initialScreenPosition)
{
	renderWindow->setView(theView);
	sf::Vector2f worldPos = renderWindow->mapPixelToCoords(*initialScreenPosition);
	nodeList.push_back(new uiNode(name, numberOfInputPins, numberOfOutputPins, pinTypes, pinNames, &worldPos));
}

void uiNodeSystem::onPollEvent(const sf::Event& e, sf::Vector2i& mousePos)
{
	/*sf::Vector2i mousePos = sf::Mouse::getPosition(*renderWindow);
	sf::Vector2f worldPos = renderWindow->mapPixelToCoords(mousePos);*/
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
					if (!nodeList[i]->mouseOverNode(mouseWorldPos))
						continue;

					// drag node
					if (nodeList[i]->mouseOverTopBar(mouseWorldPos))
					{
						draggingNodeIndex = i;
						lastMouseScreenPos = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
						break;
					}

					// start new connection
					sf::Color* pinColor = nullptr;
					sf::Vector2f pinPosition;
					int direction;

					// get pin info
					int pin = nodeList[i]->mouseOverPin(mouseWorldPos, pinColor, pinPosition, direction);
					std::cout << "pin " << pin << std::endl;

					// mouse is over pin
					if (pin > -1)
					{
						//std::cout << "mouse is over pin " << pin << std::endl;

						// pin cannot be connected and be an input pin
						bool canCreateConnection = !(nodeList[i]->isInputAndAlreadyConnected(pin));
						std::cout << "canCreateConnection: " << canCreateConnection << std::endl;
						
						if (canCreateConnection)
						{
							creatingConnection = true;
							uiNodeConnections::push(pinPosition, mouseWorldPos, pinColor, nodeList[i], pin, direction);
						}
						break;
					}
				}
			}
			else if (e.mouseButton.button == sf::Mouse::Right)
			{
				// collision from top to bottom
				//std::cout << "finding node\n";
				bool mouseWasOverTopBar = false;
				for (int i = nodeList.size() - 1; i > -1; i--)
				{
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
				//onPanningZoomingView = renderWindow->getView();
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
						// see if connection can be established
						sf::Color* pinColor = nullptr;
						sf::Vector2f pinPosition;
						int direction; // ignored, we dont need it
						int pin = nodeList[i]->mouseOverPin(mouseWorldPos, pinColor, pinPosition, direction);
						std::cout << "pin " << pin << std::endl;
						if (pin > -1)
						{
							// pin cannot be connected and be an input pin
							std::cout << "is input and already connected: " << nodeList[i]->isInputAndAlreadyConnected(pin) << std::endl;
							if (nodeList[i]->isInputAndAlreadyConnected(pin))
							{
								uiNodeConnections::removeLast();
								break;
							}

							void* nodeA; // being retreived when establishing connection
							void* nodeB = nodeList[i];

							// check if color is the same and other stuff
							if (uiNodeConnections::tryToEstablish(pinPosition, pinColor, nodeB, pin, direction, nodeA))
							{
								int line = uiNodeConnections::getLastLineIndex();
								//std::cout << "attaching line " << line << " from pin " << uiNodeConnections::getLastLinePinA() << " of node " << nodeA << " to pin " << pin << " of node " << nodeB << std::endl;
								((uiNode*)nodeA)->attachConnectionPoint(line);
								((uiNode*)nodeB)->attachConnectionPoint(line);
							}
							break;
						}
					}
					// i is equal to -1 when the for loop ends and we know the mouse was not over any pin cuz we break
					if (i < 0) // mouse was not over any pin
						uiNodeConnections::removeLast();
				}

				draggingNodeIndex = -1;
				creatingConnection = false;
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
				//theView.move(-displacement * currentZoom);
				//renderWindow->setView(onPanningZoomingView);
			}
			else if (draggingNodeIndex > -1) // dragging a node
			{
				nodeList[draggingNodeIndex]->move(displacement * currentZoom); // pass node index so it can move the lines
			}
			else if (creatingConnection)
			{
				uiNodeConnections::moveLastPoint(displacement * currentZoom);
			}
			else if (removingConnections)
			{
				void* nodeA;
				void* nodeB;
				int lineToRemove = uiNodeConnections::onTryingToRemove(mouseWorldPos, nodeA, nodeB);
				if (lineToRemove > -1) // mouse was over a line and we have to detach it from nodeA and nodeB
				{
					((uiNode*)nodeA)->setLineIndexAsDisconnected(lineToRemove);
					((uiNode*)nodeB)->setLineIndexAsDisconnected(lineToRemove);
				}
			}
			lastMouseScreenPos = currentMouseScreenPos;
			break;
		}
		case sf::Event::MouseWheelScrolled:
		{
			//onPanningZoomingView = renderWindow->getView();
			//float zoomFactor = e.mouseWheelScroll.delta * -ZOOM_SENSITIVITY + 1.0f;

			zoomInt -= e.mouseWheelScroll.delta;
			//clamp from 1 to 20
			if (zoomInt < MAX_ZOOM) zoomInt = MAX_ZOOM; else if (zoomInt > MIN_ZOOM) zoomInt = MIN_ZOOM;
			currentZoom = zoomInt / 10.0f;
			updateView();

			//std::cout << "zoomInt: " << zoomInt << std::endl;
			//std::cout << "currentZoom: " << currentZoom << std::endl;

			//onPanningZoomingView.zoom(zoomFactor);
			//currentZoom *= zoomFactor;
			//renderWindow->setView(onPanningZoomingView);

			uiNodeConnections::updateShaderUniform(currentZoom);

			//const float* matrix = onPanningZoomingView.getTransform().getMatrix();

			/*std::cout << "----------------------------------\n";
			std::cout << matrix[0] << ", " << matrix[1] << ", " << matrix[2] << ", " << matrix[3] << std::endl;
			std::cout << matrix[4] << ", " << matrix[5] << ", " << matrix[6] << ", " << matrix[7] << std::endl;
			std::cout << matrix[8] << ", " << matrix[9] << ", " << matrix[10] << ", " << matrix[11] << std::endl;
			std::cout << matrix[12] << ", " << matrix[13] << ", " << matrix[14] << ", " << matrix[15] << std::endl;*/
			//std::cout << "current zoom: " << currentZoom << std::endl;

			break;
		}
		case sf::Event::KeyPressed:
		{
			switch (e.key.code)
			{
				case sf::Keyboard::BackSpace:
				case sf::Keyboard::Delete:
				{
					if (selectedNodeIndex < 0 || draggingNodeIndex > 0)
						return;

					//std::cout << "deleting node " << selectedNodeIndex << " " << nodeList[selectedNodeIndex] << std::endl;

					delete nodeList[selectedNodeIndex];
					nodeList.erase(nodeList.begin() + selectedNodeIndex);
					selectedNodeIndex = -1;
					break;
				}
				case sf::Keyboard::Q:
				{
					printAllSystem();
					break;
				}
			}
		}
	}
}
void uiNodeSystem::draw(sf::RenderWindow& window)
{
	window.setView(theView);

	for (uiNode* n : nodeList)
		n->draw(window);

	uiNodeConnections::draw(window);
}