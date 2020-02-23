#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

namespace nodeSystem
{
	void initialize();
	void terminate();

	void onNodeCreated(int n, const void* data);
	void onNodeDeleted(int n, const std::vector<int>& connections);//int* ci, int cc);// connection indices and connection count
	void onNodeChanged(int n);

	// nA is always the node that has the output pin
	void onNodesConnected(int nA, int nB, int pA, int pB, int c);
	void onNodesDisconnected(int nA, int nB, int pA, int pB, int c);

	bool isConnectionValid(int nA, int nB, int pinA, int pinB);

	void** getDataPointersForNode(int n);
	sf::RenderTexture* getFirstOutputImageForNode(int n);
};