#include <iostream>
#include <gtest/gtest.h>
#include "../noose/nodeProvider/nodeProvider.h"

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

TEST(NodeProviderTest, FirstNodeLoading)
{
	ASSERT_TRUE(stringsAreEqual(nodeProvider::nodeDataList[0].nodeName.c_str(), "Image from file"));
	ASSERT_TRUE(
		nodeProvider::nodeDataList[0].pinTypes.size() == 3 &&
		nodeProvider::nodeDataList[0].pinNames.size() == 3 &&
		nodeProvider::nodeDataList[0].pinDefaultData.size() == 3 &&
		nodeProvider::nodeDataList[0].pinEnumOptions.size() == 3 &&
		nodeProvider::nodeDataList[0].pinEnumOptions[0].size() == 0 &&
		nodeProvider::nodeDataList[0].pinEnumOptions[1].size() == 0 &&
		nodeProvider::nodeDataList[0].pinEnumOptions[2].size() == 0
		);
	ASSERT_TRUE(
		nodeProvider::nodeDataList[0].pinTypes[0] == 3 &&
		nodeProvider::nodeDataList[0].pinTypes[1] == 3 &&
		nodeProvider::nodeDataList[0].pinTypes[2] == 2
		);
	ASSERT_TRUE(
		stringsAreEqual(nodeProvider::nodeDataList[0].pinNames[0].c_str(), "in") &&
		stringsAreEqual(nodeProvider::nodeDataList[0].pinNames[1].c_str(), "out") &&
		stringsAreEqual(nodeProvider::nodeDataList[0].pinNames[2].c_str(), "size")
		);
	ASSERT_TRUE(
		nodeProvider::nodeDataList[0].pinDefaultData[0] == nullptr &&
		nodeProvider::nodeDataList[0].pinDefaultData[1] == nullptr &&
		nodeProvider::nodeDataList[0].pinDefaultData[2] == nullptr
		);
	ASSERT_TRUE(nodeProvider::nodeDataList[0].inputPinCount == 1);
	ASSERT_TRUE(nodeProvider::nodeDataList[0].outputPinCount == 2);
	ASSERT_TRUE(nodeProvider::nodeDataList[0].nodeFunctionality != nullptr);
}

TEST(NodeProviderTest, LastNodeDataLoading)
{
	ASSERT_TRUE(stringsAreEqual(nodeProvider::nodeDataList[36].nodeName.c_str(), "Divide floats"));
	ASSERT_TRUE(
		nodeProvider::nodeDataList[36].pinTypes.size() == 3 &&
		nodeProvider::nodeDataList[36].pinNames.size() == 3 &&
		nodeProvider::nodeDataList[36].pinDefaultData.size() == 3 &&
		nodeProvider::nodeDataList[36].pinEnumOptions.size() == 3 &&
		nodeProvider::nodeDataList[36].pinEnumOptions[0].size() == 0 &&
		nodeProvider::nodeDataList[36].pinEnumOptions[1].size() == 0 &&
		nodeProvider::nodeDataList[36].pinEnumOptions[2].size() == 0
		);
	ASSERT_TRUE(
		nodeProvider::nodeDataList[36].pinTypes[0] == 1 &&
		nodeProvider::nodeDataList[36].pinTypes[1] == 1 &&
		nodeProvider::nodeDataList[36].pinTypes[2] == 1
		);
	ASSERT_TRUE(
		stringsAreEqual(nodeProvider::nodeDataList[36].pinNames[0].c_str(), "a") &&
		stringsAreEqual(nodeProvider::nodeDataList[36].pinNames[1].c_str(), "b") &&
		stringsAreEqual(nodeProvider::nodeDataList[36].pinNames[2].c_str(), "out")
		);
	ASSERT_TRUE(
		nodeProvider::nodeDataList[36].pinDefaultData[0] == nullptr &&
		nodeProvider::nodeDataList[36].pinDefaultData[1] == nullptr &&
		nodeProvider::nodeDataList[36].pinDefaultData[2] == nullptr
		);
	ASSERT_TRUE(nodeProvider::nodeDataList[36].inputPinCount == 2);
	ASSERT_TRUE(nodeProvider::nodeDataList[36].outputPinCount == 1);
	ASSERT_TRUE(nodeProvider::nodeDataList[36].nodeFunctionality != nullptr);
}

TEST(NodeProviderTest, DefaultDataLoading)
{
	ASSERT_TRUE(
		*((int*)(nodeProvider::nodeDataList[19].pinDefaultData[0])) == 255 &&
		*((int*)(nodeProvider::nodeDataList[19].pinDefaultData[1])) == 0 &&
		*((int*)(nodeProvider::nodeDataList[19].pinDefaultData[2])) == 255 &&
		*((int*)(nodeProvider::nodeDataList[19].pinDefaultData[3])) == 255 &&
		nodeProvider::nodeDataList[19].pinDefaultData[4] == nullptr
		);
}

TEST(NodeProviderTest, EnumOptionsLoading)
{
	ASSERT_TRUE(
		nodeProvider::nodeDataList[6].pinEnumOptions.size() == 4 &&
		nodeProvider::nodeDataList[6].pinEnumOptions[0].size() == 0 &&
		nodeProvider::nodeDataList[6].pinEnumOptions[1].size() == 0 &&
		nodeProvider::nodeDataList[6].pinEnumOptions[2].size() == 10 &&
		nodeProvider::nodeDataList[6].pinEnumOptions[3].size() == 0
		);
	ASSERT_TRUE(
		stringsAreEqual(nodeProvider::nodeDataList[6].pinEnumOptions[2][0].c_str(), "Alpha blend") &&
		stringsAreEqual(nodeProvider::nodeDataList[6].pinEnumOptions[2][1].c_str(), "Darken") &&
		stringsAreEqual(nodeProvider::nodeDataList[6].pinEnumOptions[2][2].c_str(), "Lighten") &&
		stringsAreEqual(nodeProvider::nodeDataList[6].pinEnumOptions[2][3].c_str(), "Add") &&
		stringsAreEqual(nodeProvider::nodeDataList[6].pinEnumOptions[2][4].c_str(), "Multiply") &&
		stringsAreEqual(nodeProvider::nodeDataList[6].pinEnumOptions[2][5].c_str(), "Difference") &&
		stringsAreEqual(nodeProvider::nodeDataList[6].pinEnumOptions[2][6].c_str(), "Screen") &&
		stringsAreEqual(nodeProvider::nodeDataList[6].pinEnumOptions[2][7].c_str(), "Color burn") &&
		stringsAreEqual(nodeProvider::nodeDataList[6].pinEnumOptions[2][8].c_str(), "Linear burn") &&
		stringsAreEqual(nodeProvider::nodeDataList[6].pinEnumOptions[2][9].c_str(), "Color dodge")
		);
}

TEST(NodeProviderTest, LengthSorting)
{
	int lastLength = 0;
	for (int item : nodeProvider::sortedByLength)
	{
		ASSERT_TRUE(lastLength <= nodeProvider::nodeDataList[item].nodeName.length());
		lastLength = nodeProvider::nodeDataList[item].nodeName.length();
	}
}

TEST(NodeProviderTest, CategoryNamesLoading)
{
	ASSERT_TRUE(nodeProvider::categoryNames.size() == 6);
	ASSERT_TRUE(stringsAreEqual(nodeProvider::categoryNames[0].c_str(), "Insert image"));
	ASSERT_TRUE(stringsAreEqual(nodeProvider::categoryNames[1].c_str(), "Image processing"));
	ASSERT_TRUE(stringsAreEqual(nodeProvider::categoryNames[2].c_str(), "Color operations"));
	ASSERT_TRUE(stringsAreEqual(nodeProvider::categoryNames[3].c_str(), "Int vector operations"));
	ASSERT_TRUE(stringsAreEqual(nodeProvider::categoryNames[4].c_str(), "Integer operations"));
	ASSERT_TRUE(stringsAreEqual(nodeProvider::categoryNames[5].c_str(), "Float operations"));
}

TEST(NodeProviderTest, CategoryStartIndicesLoading)
{
	ASSERT_TRUE(nodeProvider::categoryStartIndex.size() == 6);
	ASSERT_TRUE(nodeProvider::categoryStartIndex[0] == 0);
	ASSERT_TRUE(nodeProvider::categoryStartIndex[1] == 4);
	ASSERT_TRUE(nodeProvider::categoryStartIndex[2] == 18);
	ASSERT_TRUE(nodeProvider::categoryStartIndex[3] == 21);
	ASSERT_TRUE(nodeProvider::categoryStartIndex[4] == 27);
	ASSERT_TRUE(nodeProvider::categoryStartIndex[5] == 32);
}

TEST(NodeProviderTest, NodeNamesByCategoryLoading)
{
	ASSERT_TRUE(nodeProvider::nodeNamesByCategory.size() == 6);
	ASSERT_TRUE(stringsAreEqual(nodeProvider::nodeNamesByCategory[0][0].c_str(), "Image from file"));
	ASSERT_TRUE(stringsAreEqual(nodeProvider::nodeNamesByCategory[0][3].c_str(), "Linear Gradient"));
	ASSERT_TRUE(stringsAreEqual(nodeProvider::nodeNamesByCategory[1][0].c_str(), "Separate channels"));
	ASSERT_TRUE(stringsAreEqual(nodeProvider::nodeNamesByCategory[2][0].c_str(), "Color"));
	ASSERT_TRUE(stringsAreEqual(nodeProvider::nodeNamesByCategory[3][0].c_str(), "Int vector"));
	ASSERT_TRUE(stringsAreEqual(nodeProvider::nodeNamesByCategory[4][0].c_str(), "Int"));
	ASSERT_TRUE(stringsAreEqual(nodeProvider::nodeNamesByCategory[5][0].c_str(), "Float"));
	ASSERT_TRUE(stringsAreEqual(nodeProvider::nodeNamesByCategory[5][4].c_str(), "Divide floats"));
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	nodeProvider::initialize();
	return RUN_ALL_TESTS();
}