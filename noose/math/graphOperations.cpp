#include "graphOperations.h"
#include <iostream>

void go::updatePropagationMatrix(std::vector<std::vector<logicalNode*>>& matrix, logicalNode* node)
{
	// add the right side node if not already added
	int i, s;
	nodePositionInMatrix(node, matrix, i, s);
	if (i < 0)
		appendNodeToMatrix(matrix, node, 0);

	// add the right node's matrix
	const std::vector<std::vector<logicalNode*>>& matrixRS = node->getPropagationMatrix();
	int indexR = 1;
	for (const std::vector<logicalNode*> l : matrixRS)
	{
		for (logicalNode* n : l)
		{
			int index, subIndex;
			nodePositionInMatrix(n, matrix, index, subIndex);
			if (index < 0) // append the node
			{
				appendNodeToMatrix(matrix, n, indexR);
			}
			else // have to keep the one that is further if we have two repeated nodes
			{
				if (index < indexR) // we have to replace the node
				{
					matrix[index].erase(matrix[index].begin() + subIndex);
					appendNodeToMatrix(matrix, n, indexR);
				}
			}
		}
		indexR++;
	}
}
void go::appendNodeToMatrix(std::vector<std::vector<logicalNode*>>& matrix, logicalNode* node, int index)
{
	// get more space if needed
	/*std::cout << "index: " << index << std::endl;
	std::cout << "the size of the matrix is " << matrix.size() << std::endl;*/
	while (matrix.size() <= index)
	{
		matrix.emplace_back();
		//std::cout << "lal\n";
	}


	//std::cout << "the size of the matrix is " << matrix.size() << std::endl;
	// push the node
	matrix[index].push_back(node);
}
void go::nodePositionInMatrix(const logicalNode* n, std::vector<std::vector<logicalNode*>>& m, int& index, int& subIndex)
{
	index = 0;
	for (std::vector<logicalNode*> l : m)
	{
		subIndex = 0;
		for (logicalNode* node : l)
		{
			if (node == n)
				return;
			subIndex++;
		}
		index++;
	}
	index = subIndex = -1;
}
void go::removeNodeFromList(const logicalNode* n, std::vector<logicalNode*>& list)
{
	int j = 0;
	for (logicalNode* node : list)
	{
		if (node == n)
		{
			list.erase(list.begin() + j);
			std::cout << "node erased from left side list\n";
			return;
		}
		j++;
	}
}
void go::matrixPropagation(std::vector<std::vector<logicalNode*>>& leftSideMatrix, std::vector<std::vector<logicalNode*>>& rightSideMatrix)
{
	for (int i = 0; i < rightSideMatrix.size(); i++)
	{
		for (logicalNode* n : rightSideMatrix[i])
		{
			int subIndexA;
			int indexA;
			nodePositionInMatrix(n, leftSideMatrix, indexA, subIndexA);
			if (indexA < 0)
			{
				appendNodeToMatrix(leftSideMatrix, n, i + 1);
				continue;
			}

			std::cout << "indexA: " << indexA << "\nsubIndexA: " << subIndexA << "\ni + 1: " << i + 1 << std::endl;
			if (indexA < i + 1)
			{
				//std::cout << "asdf\n";
				leftSideMatrix[indexA].erase(leftSideMatrix[indexA].begin() + subIndexA);
				appendNodeToMatrix(leftSideMatrix, n, i + 1);
			}
		}
	}
}