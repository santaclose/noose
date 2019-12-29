#pragma once
#include "../uiNode.h"

namespace go
{
	void updatePropagationMatrix(std::vector<std::vector<uiNode*>>& matrix, uiNode* node);
	void appendNodeToMatrix(std::vector<std::vector<uiNode*>>& matrix, uiNode* node, int index);
	void nodePositionInMatrix(const uiNode* n, std::vector<std::vector<uiNode*>>& m, int& index, int& subIndex);
	void removeNodeFromList(const uiNode* n, std::vector<uiNode*>& list);
	void matrixPropagation(std::vector<std::vector<uiNode*>>& leftSideMatrix, std::vector<std::vector<uiNode*>>& rightSideMatrix);
};