#pragma once
namespace nodeSystem
{
	void initialize();

	void onNodeCreated(int n, const void* data);
	void onNodeDeleted(int n);
	void onNodeChanged(int n);

	// nA is always the node that has the output pin
	void onNodesConnected(int nA, int nB, int pA, int pB, int c);
	void onNodesDisconnected(int nA, int nB, int pA, int pB, int c);

	bool isConnectionValid(int nA, int nB, int pinA, int pinB);

	void** getDataPointersForNode(int n);
};