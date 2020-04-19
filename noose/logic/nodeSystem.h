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

#ifdef TEST
	std::ostream& status(std::ostream& stream);
#endif
};