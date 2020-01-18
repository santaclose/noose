#pragma once
#include "../node_system/logicalNode.h"

class go
{
public:
	static void updatePropagationMatrix(std::vector<std::vector<logicalNode*>>& matrix, logicalNode* node);
	static void appendNodeToMatrix(std::vector<std::vector<logicalNode*>>& matrix, logicalNode* node, int index);
	static void nodePositionInMatrix(const logicalNode* n, std::vector<std::vector<logicalNode*>>& m, int& index, int& subIndex);
	static void removeNodeFromList(const logicalNode* n, std::vector<logicalNode*>& list);
	static void matrixPropagation(std::vector<std::vector<logicalNode*>>& leftSideMatrix, std::vector<std::vector<logicalNode*>>& rightSideMatrix);
};