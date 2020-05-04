#include <iostream>
#include <gtest/gtest.h>
#include "../noose/nodeProvider/nodeProvider.h"
#include "../noose/logic/nodeSystem.h"
#include "../noose/logic/node.h"
#include "../noose/logic/connectionSystem.h"

bool stringsAreEqual(const char* a, const char* b)
{
	int i = 0;
	for (; a[i] != '\0'; i++)
	{
		if (b[i] != a[i])
		{
			return false;
		}
	}
	return a[i] == b[i];
}

TEST(SimpleFunctions, NodeCreationAndDeletion)
{
	std::vector<node*>* nodeList = (std::vector<node*>*) nodeSystem::getNodeList();
	ASSERT_TRUE(nodeList->size() == 0);
	nodeSystem::onNodeCreated(0, &(nodeProvider::nodeDataList[0]));
	ASSERT_TRUE(nodeList->size() == 1);
	ASSERT_TRUE((*nodeList)[0]->m_inputPinCount == 1);
	ASSERT_TRUE((*nodeList)[0]->m_outputPinCount == 2);
	ASSERT_TRUE((*nodeList)[0]->m_pinTypes[0] == 3);
	ASSERT_TRUE((*nodeList)[0]->m_pinTypes[1] == 3);
	ASSERT_TRUE((*nodeList)[0]->m_pinTypes[2] == 2);
	ASSERT_TRUE((*nodeList)[0]->m_pinDataPointers.size() == 3);
	ASSERT_TRUE((*nodeList)[0]->m_pinDataPointers[0] != nullptr);
	ASSERT_TRUE((*nodeList)[0]->m_pinDataPointers[1] != nullptr);
	ASSERT_TRUE((*nodeList)[0]->m_pinDataPointers[2] != nullptr);
	ASSERT_TRUE((*nodeList)[0]->m_receivedDataPointers.size() == 1);
	ASSERT_TRUE((*nodeList)[0]->m_receivedDataPointers[0] == nullptr);
	ASSERT_TRUE((*nodeList)[0]->m_propagationMatrix.size() == 0);
	ASSERT_TRUE((*nodeList)[0]->m_leftSideNodes.size() == 0);
	ASSERT_TRUE((*nodeList)[0]->m_nodeFunctionalityPointer != nullptr);

	std::vector<int> c;
	nodeSystem::onNodeDeleted(0, c);
	ASSERT_TRUE(nodeList->size() == 1);
	ASSERT_TRUE((*nodeList)[0] == nullptr);
}

TEST(SimpleFunctions, ReusingVectorSlotsAndDefaultDataCreated)
{
	std::vector<node*>* nodeList = (std::vector<node*>*) nodeSystem::getNodeList();
	nodeSystem::onNodeCreated(0, &(nodeProvider::nodeDataList[2]));
	ASSERT_TRUE((*nodeList)[0]->m_pinDataPointers.size() == 3);
	ASSERT_TRUE(*((int*)((*nodeList)[0]->m_pinDataPointers[0])) == 20);
	std::vector<int> c;
	nodeSystem::onNodeDeleted(0, c);
}

TEST(SimpleFunctions, ConnectingNodesAndTestingGetDataPointerFunction)
{
	std::vector<node*>* nodeList = (std::vector<node*>*) nodeSystem::getNodeList();
	nodeSystem::onNodeCreated(0, &(nodeProvider::nodeDataList[2]));
	nodeSystem::onNodeCreated(1, &(nodeProvider::nodeDataList[0]));
	nodeSystem::onNodesConnected(0, 1, 2, 0, 0);
	void* receivedDataPointer = (*nodeList)[1]->m_receivedDataPointers[0];
	ASSERT_TRUE(receivedDataPointer != nullptr);
	ASSERT_TRUE(receivedDataPointer == (*nodeList)[0]->m_pinDataPointers[2]);
	ASSERT_TRUE(connectionSystem::connections.size() == 1);
	ASSERT_TRUE(connectionSystem::connections[0].deleted == false);
	ASSERT_TRUE(connectionSystem::connections[0].nodeA == (*nodeList)[0]);
	ASSERT_TRUE(connectionSystem::connections[0].nodeB == (*nodeList)[1]);
	ASSERT_TRUE(connectionSystem::connections[0].pinA == 2);
	ASSERT_TRUE(connectionSystem::connections[0].pinB == 0);
	ASSERT_TRUE(connectionSystem::connections[0].nodeIndexA == 0);
	ASSERT_TRUE(connectionSystem::connections[0].nodeIndexB == 1);
	ASSERT_TRUE(receivedDataPointer == (*nodeList)[1]->getDataPointer(0, true));
	ASSERT_TRUE((*nodeList)[1]->m_pinDataPointers[0] == (*nodeList)[1]->getDataPointer(0, false));

	ASSERT_TRUE((*nodeList)[0]->m_propagationMatrix.size() == 1);
	ASSERT_TRUE((*nodeList)[0]->m_propagationMatrix[0].size() == 1);
	ASSERT_TRUE((*nodeList)[0]->m_propagationMatrix[0][0] == (*nodeList)[1]);
	ASSERT_TRUE((*nodeList)[0]->m_leftSideNodes.size() == 0);
	ASSERT_TRUE((*nodeList)[1]->m_propagationMatrix.size() == 0);
	ASSERT_TRUE((*nodeList)[1]->m_leftSideNodes.size() == 1);
	ASSERT_TRUE((*nodeList)[1]->m_leftSideNodes[0] == (*nodeList)[0]);

	std::vector<int> c = {0};
	nodeSystem::onNodeDeleted(0, c);
	c.clear();
	nodeSystem::onNodeDeleted(1, c);
}

TEST(ExecutionAlgorithmTesting, CheckMatricesAfterConnectingDisconnectingAndConnectingInDifferentOrder)
{
	std::vector<node*>* nodeList = (std::vector<node*>*) nodeSystem::getNodeList();
	nodeSystem::onNodeCreated(0, &(nodeProvider::nodeDataList[0]));
	nodeSystem::onNodeCreated(1, &(nodeProvider::nodeDataList[0]));
	nodeSystem::onNodeCreated(2, &(nodeProvider::nodeDataList[0]));
	nodeSystem::onNodeCreated(3, &(nodeProvider::nodeDataList[0]));
	nodeSystem::onNodesConnected(2, 3, 1, 0, 0);
	nodeSystem::onNodesConnected(1, 2, 1, 0, 1);
	nodeSystem::onNodesConnected(0, 1, 1, 0, 2);
	nodeSystem::onNodesDisconnected(2, 3, 1, 0, 0);
	nodeSystem::onNodesDisconnected(1, 2, 1, 0, 1);
	nodeSystem::onNodesConnected(1, 3, 1, 0, 0);
	nodeSystem::onNodesConnected(3, 2, 1, 0, 1);
	ASSERT_TRUE((*nodeList)[0]->m_propagationMatrix[0].size() == 1);
	ASSERT_TRUE((*nodeList)[0]->m_propagationMatrix[0][0] == (*nodeList)[1]);
	ASSERT_TRUE((*nodeList)[0]->m_propagationMatrix[1].size() == 1);
	ASSERT_TRUE((*nodeList)[0]->m_propagationMatrix[1][0] == (*nodeList)[3]);
	ASSERT_TRUE((*nodeList)[0]->m_propagationMatrix[2].size() == 1);
	ASSERT_TRUE((*nodeList)[0]->m_propagationMatrix[2][0] == (*nodeList)[2]);
	ASSERT_TRUE((*nodeList)[1]->m_propagationMatrix[0].size() == 1);
	ASSERT_TRUE((*nodeList)[1]->m_propagationMatrix[0][0] == (*nodeList)[3]);
	ASSERT_TRUE((*nodeList)[1]->m_propagationMatrix[1].size() == 1);
	ASSERT_TRUE((*nodeList)[1]->m_propagationMatrix[1][0] == (*nodeList)[2]);
	for (std::vector<node*>& list : (*nodeList)[2]->m_propagationMatrix)
		ASSERT_TRUE(list.size() == 0);
	ASSERT_TRUE((*nodeList)[3]->m_propagationMatrix[0].size() == 1);
	ASSERT_TRUE((*nodeList)[3]->m_propagationMatrix[0][0] == (*nodeList)[2]);

	std::vector<int> c = {2};
	nodeSystem::onNodeDeleted(0, c);
	c[0] = 0;
	nodeSystem::onNodeDeleted(1, c);
	c[0] = 1;
	nodeSystem::onNodeDeleted(2, c);
	c.clear();
	nodeSystem::onNodeDeleted(3, c);
	for (node* n : *nodeList)
		ASSERT_TRUE(n == nullptr); // everything clean
}

TEST(ExecutionAlgorithmTesting, CheckMatricesAfterConnectingOneNodeToAnotherWithTwoPins)
{
	std::vector<node*>* nodeList = (std::vector<node*>*) nodeSystem::getNodeList();
	nodeSystem::onNodeCreated(0, &(nodeProvider::nodeDataList[6]));
	nodeSystem::onNodeCreated(1, &(nodeProvider::nodeDataList[6]));
	nodeSystem::onNodesConnected(0, 1, 3, 0, 0);
	nodeSystem::onNodesConnected(0, 1, 3, 1, 1);
	ASSERT_TRUE((*nodeList)[0]->m_propagationMatrix.size() == 1);
	ASSERT_TRUE((*nodeList)[0]->m_propagationMatrix[0].size() == 1);
	ASSERT_TRUE((*nodeList)[0]->m_propagationMatrix[0][0] == (*nodeList)[1]);
	ASSERT_TRUE((*nodeList)[1]->m_propagationMatrix.size() == 0);
	std::vector<int> c = {0, 1};
	nodeSystem::onNodeDeleted(0, c);
	c.clear();
	nodeSystem::onNodeDeleted(1, c);
	for (node* n : *nodeList)
		ASSERT_TRUE(n == nullptr); // everything clean
}

TEST(ExecutionAlgorithmTesting, CheckMatricesAfterConnectingDisconnectingRemovingNodes)
{
	std::vector<node*>* nodeList = (std::vector<node*>*) nodeSystem::getNodeList();
	nodeSystem::onNodeCreated(0, &(nodeProvider::nodeDataList[6]));
	nodeSystem::onNodeCreated(1, &(nodeProvider::nodeDataList[6]));
	nodeSystem::onNodeCreated(2, &(nodeProvider::nodeDataList[6]));
	nodeSystem::onNodeCreated(3, &(nodeProvider::nodeDataList[6]));
	nodeSystem::onNodesConnected(0, 1, 3, 0, 0);
	nodeSystem::onNodesConnected(0, 1, 3, 1, 1);
	nodeSystem::onNodesConnected(1, 2, 3, 0, 2);
	nodeSystem::onNodesConnected(1, 2, 3, 1, 3);
	nodeSystem::onNodesConnected(2, 3, 3, 0, 4);
	nodeSystem::onNodesConnected(2, 3, 3, 1, 5);
	std::vector<int> c = {2, 3, 4, 5};
	nodeSystem::onNodeDeleted(2, c);
	nodeSystem::onNodesConnected(3, 0, 3, 0, 2);
	nodeSystem::onNodesDisconnected(0, 1, 3, 1, 1);
	nodeSystem::onNodeCreated(2, &(nodeProvider::nodeDataList[6]));
	nodeSystem::onNodesConnected(2, 3, 3, 1, 1);
	nodeSystem::onNodesConnected(2, 3, 3, 0, 3);
	nodeSystem::onNodesConnected(0, 1, 3, 1, 4);

	ASSERT_TRUE((*nodeList)[2]->m_propagationMatrix[0].size() == 1);
	ASSERT_TRUE((*nodeList)[2]->m_propagationMatrix[0][0] == (*nodeList)[3]);
	ASSERT_TRUE((*nodeList)[2]->m_propagationMatrix[1].size() == 1);
	ASSERT_TRUE((*nodeList)[2]->m_propagationMatrix[1][0] == (*nodeList)[0]);
	ASSERT_TRUE((*nodeList)[2]->m_propagationMatrix[2].size() == 1);
	ASSERT_TRUE((*nodeList)[2]->m_propagationMatrix[2][0] == (*nodeList)[1]);

	ASSERT_TRUE((*nodeList)[3]->m_propagationMatrix[0].size() == 1);
	ASSERT_TRUE((*nodeList)[3]->m_propagationMatrix[0][0] == (*nodeList)[0]);
	ASSERT_TRUE((*nodeList)[3]->m_propagationMatrix[1].size() == 1);
	ASSERT_TRUE((*nodeList)[3]->m_propagationMatrix[1][0] == (*nodeList)[1]);

	ASSERT_TRUE((*nodeList)[0]->m_propagationMatrix[0].size() == 1);
	ASSERT_TRUE((*nodeList)[0]->m_propagationMatrix[0][0] == (*nodeList)[1]);
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	nodeProvider::initialize();
	nodeSystem::initialize();
	return RUN_ALL_TESTS();
}