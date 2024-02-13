#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

namespace nodeSystem
{
	void initialize();
	void terminate();

	int createNode(const void* data);
	void deleteNode(int n);
	void onNodeChanged(int n, int p);

	// nA is always the node that has the output pin
	int connect(int nA, int nB, int pA, int pB, bool activateNodeB = true);
	void disconnect(int c);

	bool isConnectionValid(int nA, int nB, int pinA, int pinB);

	const std::vector<void*>& getDataPointersForNode(int n);
	const int* getPinTypesForNode(int n);
	int getOutputPinCountForNode(int n);

	void clearEverything();

#ifdef TEST
	void* getNodeList();
#endif

	const void* getData(int n, int p, int& type);

	// project loading specific
	void onProjectFileParseNode(const std::string& nodeName, float coordinatesX, float coordinatesY);
	void onProjectFileParseNodeInput(int nodeIndex, int pinIndex, void* data, int flags);
	void onProjectFileParseConnection(int nodeAIndex, int pinAIndex, int nodeBIndex, int pinBIndex);
	void onProjectFileFinishParsing();

	// custom node specific
	void onCustomNodeFileStartParsing();
	void onCustomNodeFileParseNode(const std::string& nodeName, float coordinatesX, float coordinatesY);
	void onCustomNodeFileParseNodeInput(int nodeIndex, int pinIndex, void* data, int flags);
	void onCustomNodeFileParseConnection(int nodeAIndex, int pinAIndex, int nodeBIndex, int pinBIndex);
	void onCustomNodeFileParseCustomNodeInput(int inPin, int subgraphNode, int subgraphPin);
	void onCustomNodeFileParseCustomNodeOutput(int outPin, int subgraphNode, int subgraphPin);
	void onCustomNodeFileFinishParsing();
};