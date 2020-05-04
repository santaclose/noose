#include <iostream>
#include <vector>
#include <gtest/gtest.h>
#include "../noose/logic/node.h"
#include "../noose/logic/graphOperations.h"
#include "../noose/nodeProvider/nodeProvider.h"

TEST(NodeInsertionInMatrix, InsertingAtIndex0)
{
	std::vector<std::vector<node*>> matrix;
	go::appendNodeToMatrix(matrix, (node*) 200, 0);
	ASSERT_TRUE(matrix.size() == 1);
	ASSERT_TRUE(matrix[0].size() == 1);
	ASSERT_TRUE(matrix[0][0] == (node*) 200);
}

TEST(NodeInsertionInMatrix, InsertingAtIndex2)
{
	std::vector<std::vector<node*>> matrix;
	go::appendNodeToMatrix(matrix, (node*) 100, 2);
	ASSERT_TRUE(matrix.size() == 3);
	ASSERT_TRUE(matrix[0].size() == 0);
	ASSERT_TRUE(matrix[1].size() == 0);
	ASSERT_TRUE(matrix[2].size() == 1);
	ASSERT_TRUE(matrix[2][0] == (node*) 100);
}

TEST(NodeInsertionInMatrix, TwoInsertionsInTheSameIndex)
{
	std::vector<std::vector<node*>> matrix;
	go::appendNodeToMatrix(matrix, (node*) 100, 2);
	go::appendNodeToMatrix(matrix, (node*) 101, 2);
	ASSERT_TRUE(matrix.size() == 3);
	ASSERT_TRUE(matrix[0].size() == 0);
	ASSERT_TRUE(matrix[1].size() == 0);
	ASSERT_TRUE(matrix[2].size() == 2);
	ASSERT_TRUE(matrix[2][0] == (node*) 100);
	ASSERT_TRUE(matrix[2][1] == (node*) 101);
}

TEST(NodeFindingInMatrix, FindingFromIndex0To2)
{
	std::vector<std::vector<node*>> matrix;
	go::appendNodeToMatrix(matrix, (node*) 100, 0);
	go::appendNodeToMatrix(matrix, (node*) 101, 2);
	go::appendNodeToMatrix(matrix, (node*) 102, 2);
	go::appendNodeToMatrix(matrix, (node*) 103, 2);
	go::appendNodeToMatrix(matrix, (node*) 104, 1);
	int index, subindex;
	go::nodePositionInMatrix((node*) 100, matrix, index, subindex);
	ASSERT_TRUE(index == 0 && subindex == 0);
	go::nodePositionInMatrix((node*) 104, matrix, index, subindex);
	ASSERT_TRUE(index == 1 && subindex == 0);
	go::nodePositionInMatrix((node*) 101, matrix, index, subindex);
	ASSERT_TRUE(index == 2 && subindex == 0);
	go::nodePositionInMatrix((node*) 102, matrix, index, subindex);
	ASSERT_TRUE(index == 2 && subindex == 1);
	go::nodePositionInMatrix((node*) 103, matrix, index, subindex);
	ASSERT_TRUE(index == 2 && subindex == 2);
}

TEST(RemovingNodesFromList, RemovingFromListContaingOneElement)
{
	std::vector<std::vector<node*>> matrix;
	go::appendNodeToMatrix(matrix, (node*) 100, 0);
	go::removeNodeFromList((node*) 100, matrix[0]);
	ASSERT_TRUE(matrix.size() == 1);
	ASSERT_TRUE(matrix[0].size() == 0);
}

TEST(RemovingNodesFromList, RemovingFromListContaingTwoElements)
{
	std::vector<std::vector<node*>> matrix;
	go::appendNodeToMatrix(matrix, (node*) 100, 0);
	go::removeNodeFromList((node*) 100, matrix[0]);
	ASSERT_TRUE(matrix.size() == 1);
	ASSERT_TRUE(matrix[0].size() == 0);
}

TEST(RemovingNodesFromList, RemovingFirstElementFromListContaingTwoElements)
{
	std::vector<std::vector<node*>> matrix;
	go::appendNodeToMatrix(matrix, (node*) 100, 0);
	go::appendNodeToMatrix(matrix, (node*) 101, 0);
	go::removeNodeFromList((node*) 100, matrix[0]);
	ASSERT_TRUE(matrix[0].size() == 1);
	ASSERT_TRUE(matrix[0][0] == (node*) 101);
}

TEST(RemovingNodesFromList, RemovingLastElementFromListContaingTwoElements)
{
	std::vector<std::vector<node*>> matrix;
	go::appendNodeToMatrix(matrix, (node*) 100, 0);
	go::appendNodeToMatrix(matrix, (node*) 101, 0);
	go::removeNodeFromList((node*) 101, matrix[0]);
	ASSERT_TRUE(matrix[0].size() == 1);
	ASSERT_TRUE(matrix[0][0] == (node*) 100);
}

TEST(MatrixPropagation, SimplestCase)
{
	std::vector<std::vector<node*>> lsMatrix, rsMatrix;
	go::appendNodeToMatrix(lsMatrix, (node*) 'b', 0);
	go::appendNodeToMatrix(lsMatrix, (node*) 'c', 0);
	go::appendNodeToMatrix(rsMatrix, (node*) 'd', 0);
	go::matrixPropagation(lsMatrix, rsMatrix);
	ASSERT_TRUE(lsMatrix.size() == 2);
	ASSERT_TRUE(lsMatrix[0].size() == 2);
	ASSERT_TRUE(lsMatrix[1].size() == 1);
	ASSERT_TRUE(lsMatrix[0][0] == (node*) 'b');
	ASSERT_TRUE(lsMatrix[0][1] == (node*) 'c');
	ASSERT_TRUE(lsMatrix[1][0] == (node*) 'd');

}

TEST(MatrixPropagation, ReplaceCase)
{
	std::vector<std::vector<node*>> lsMatrix, rsMatrix;
	go::appendNodeToMatrix(lsMatrix, (node*) 'b', 0);
	go::appendNodeToMatrix(lsMatrix, (node*) 'c', 0);
	go::appendNodeToMatrix(rsMatrix, (node*) 'c', 0);
	go::matrixPropagation(lsMatrix, rsMatrix);
	ASSERT_TRUE(lsMatrix.size() == 2);
	ASSERT_TRUE(lsMatrix[0].size() == 1);
	ASSERT_TRUE(lsMatrix[1].size() == 1);
	ASSERT_TRUE(lsMatrix[0][0] == (node*) 'b');
	ASSERT_TRUE(lsMatrix[1][0] == (node*) 'c');

}

TEST(UpdateMatrixFromNode, HasToAppendTheNode)
{
	std::vector<std::vector<node*>> lsMatrix;
	node* rsNode = new node(&(nodeProvider::nodeDataList[0]));
	go::appendNodeToMatrix(rsNode->m_propagationMatrix, (node*) '0', 0);
	go::updatePropagationMatrix(lsMatrix, rsNode);
	ASSERT_TRUE(lsMatrix.size() == 2);
	ASSERT_TRUE(lsMatrix[0].size() == 1);
	ASSERT_TRUE(lsMatrix[1].size() == 1);
	ASSERT_TRUE(lsMatrix[0][0] == rsNode);
	ASSERT_TRUE(lsMatrix[1][0] == (node*) '0');
	delete rsNode;
}

TEST(UpdateMatrixFromNode, DoesntHaveToAppendTheNode)
{
	std::vector<std::vector<node*>> lsMatrix;
	node* rsNode = new node(&(nodeProvider::nodeDataList[0]));
	go::appendNodeToMatrix(rsNode->m_propagationMatrix, (node*) '0', 0);
	go::appendNodeToMatrix(lsMatrix, rsNode, 0);
	go::updatePropagationMatrix(lsMatrix, rsNode);
	ASSERT_TRUE(lsMatrix.size() == 2);
	ASSERT_TRUE(lsMatrix[0].size() == 1);
	ASSERT_TRUE(lsMatrix[1].size() == 1);
	ASSERT_TRUE(lsMatrix[0][0] == rsNode);
	ASSERT_TRUE(lsMatrix[1][0] == (node*) '0');
	delete rsNode;
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	nodeProvider::initialize();
	return RUN_ALL_TESTS();
}