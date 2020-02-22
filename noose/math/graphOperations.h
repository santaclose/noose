#pragma once
#include "../logic/node.h"

class go
{
public:
	static void updatePropagationMatrix(std::vector<std::vector<node*>>& matrix, node* theNode);
	static void appendNodeToMatrix(std::vector<std::vector<node*>>& matrix, node* theNode, int index);
	static void nodePositionInMatrix(const node* n, std::vector<std::vector<node*>>& m, int& index, int& subIndex);
	static void removeNodeFromList(const node* n, std::vector<node*>& list);
	static void matrixPropagation(std::vector<std::vector<node*>>& leftSideMatrix, std::vector<std::vector<node*>>& rightSideMatrix);
};