#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

namespace nodeSystem
{
	void initialize();
	void terminate();

	void onNodeCreated(int n, const void* data);
	void onNodeDeleted(int n, const std::vector<int>& connections); // connection indices and connection count
	void onNodeChanged(int n);

	// nA is always the node that has the output pin
	void onNodesConnected(int nA, int nB, int pA, int pB, int c);
	void onNodesDisconnected(int nA, int nB, int pA, int pB, int c);

	bool isConnectionValid(int nA, int nB, int pinA, int pinB);

	const std::vector<void*>& getDataPointersForNode(int n);
	const int* getPinTypesForNode(int n);
	int getOutputPinCountForNode(int n);

	void clearEverything();

#ifdef TEST
	void* getNodeList();
#endif

	const void* getData(int n, int p, int& type);

	// project loading
	void onProjectFileLoadingStart();
	void onProjectFileLoadingAddNode(const std::string& nodeName, float coordinatesX, float coordinatesY);
	void onProjectFileLoadingSetNodeInput(int nodeIndex, int pinIndex, void* data, int flags);
	void onProjectFileLoadingAddConnection(int nodeAIndex, int pinAIndex, int nodeBIndex, int pinBIndex);
};