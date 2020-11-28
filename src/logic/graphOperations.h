#pragma once
#include "node.h"

namespace go
{
	void updatePropagationMatrix(std::vector<std::vector<node*>>& matrix, node* theNode);
	void matrixPropagation(std::vector<std::vector<node*>>& leftSideMatrix, const std::vector<std::vector<node*>>& rightSideMatrix);
	void appendNodeToMatrix(std::vector<std::vector<node*>>& matrix, node* theNode, int index);
	void nodePositionInMatrix(const node* n, const std::vector<std::vector<node*>>& m, int& index, int& subIndex);
	void removeNodeFromList(const node* n, std::vector<node*>& list);
}