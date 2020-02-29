#include "graphOperations.h"
#include <iostream>

void go::updatePropagationMatrix(std::vector<std::vector<node*>>& matrix, node* theNode)
{
	// add the right side node if not already added
	int i, s;
	nodePositionInMatrix(theNode, matrix, i, s);
	if (i < 0)
		appendNodeToMatrix(matrix, theNode, 0);

	// add the right node's matrix
	const std::vector<std::vector<node*>>& matrixRS = theNode->getPropagationMatrix();
	matrixPropagation(matrix, matrixRS);
}

void go::matrixPropagation(std::vector<std::vector<node*>>& leftSideMatrix, const std::vector<std::vector<node*>>& rightSideMatrix)
{
	int indexR = 1;
	for (const std::vector<node*>& l : rightSideMatrix)
	{
		for (node* n : l)
		{
			int indexA, subIndexA;
			nodePositionInMatrix(n, leftSideMatrix, indexA, subIndexA);
			if (indexA < 0)
			{
				appendNodeToMatrix(leftSideMatrix, n, indexR);
			}
			else if (indexA < indexR)
			{
				// we have to replace the node
				leftSideMatrix[indexA].erase(leftSideMatrix[indexA].begin() + subIndexA);
				appendNodeToMatrix(leftSideMatrix, n, indexR);
			}
		}
		indexR++;
	}
}

void go::appendNodeToMatrix(std::vector<std::vector<node*>>& matrix, node* theNode, int index)
{
	// get more space if needed
	//std::cout << "index: " << index << std::endl;
	//std::cout << "the size of the matrix is " << matrix.size() << std::endl;
	if (index >= matrix.size())
		matrix.resize(index + 1);

	//std::cout << "the size of the matrix is " << matrix.size() << std::endl;
	// push the node
	matrix[index].push_back(theNode);
}

void go::nodePositionInMatrix(const node* n, const std::vector<std::vector<node*>>& m, int& index, int& subIndex)
{
	index = 0;
	for (std::vector<node*> l : m)
	{
		subIndex = 0;
		for (node* theNode : l)
		{
			if (theNode == n)
				return;
			subIndex++;
		}
		index++;
	}
	index = subIndex = -1;
}

void go::removeNodeFromList(const node* n, std::vector<node*>& list)
{
	int j = 0;
	for (node* theNode : list)
	{
		if (theNode == n)
		{
			list.erase(list.begin() + j);
			//std::cout << "node erased from left side list\n";
			return;
		}
		j++;
	}
}