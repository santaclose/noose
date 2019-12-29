#include "graphOperations.h"

namespace go
{
	void updatePropagationMatrix(std::vector<std::vector<uiNode*>>& matrix, uiNode* node)
	{
		// add the right side node if not already added
		int i, s;
		nodePositionInMatrix(node, matrix, i, s);
		if (i < 0)
			appendNodeToMatrix(matrix, node, 0);

		// add the right node's matrix
		const std::vector<std::vector<uiNode*>>& matrixRS = node->getPropagationMatrix();
		int indexR = 1;
		for (const std::vector<uiNode*> l : matrixRS)
		{
			for (uiNode* n : l)
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
	void appendNodeToMatrix(std::vector<std::vector<uiNode*>>& matrix, uiNode* node, int index)
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
	void nodePositionInMatrix(const uiNode* n, std::vector<std::vector<uiNode*>>& m, int& index, int& subIndex)
	{
		index = 0;
		for (std::vector<uiNode*> l : m)
		{
			subIndex = 0;
			for (uiNode* node : l)
			{
				if (node == n)
					return;
				subIndex++;
			}
			index++;
		}
		index = subIndex = -1;
	}
	void removeNodeFromList(const uiNode* n, std::vector<uiNode*>& list)
	{
		int j = 0;
		for (uiNode* node : list)
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
	void matrixPropagation(std::vector<std::vector<uiNode*>>& leftSideMatrix, std::vector<std::vector<uiNode*>>& rightSideMatrix)
	{
		for (int i = 0; i < rightSideMatrix.size(); i++)
		{
			for (uiNode* n : rightSideMatrix[i])
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
}