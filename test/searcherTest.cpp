#include <iostream>
#include <gtest/gtest.h>
#include "../noose/nodeProvider/nodeProvider.h"
#include "../noose/searcher.h"
#include "../noose/nodeData.h"

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

TEST(SearcherTests, LowercaseAndUppercaseGiveTheSameResult)
{
	searcher::search("int", 3, 20);
	std::vector<std::string*> lowercaseResults = searcher::searchResults;
	searcher::search("INT", 3, 20);
	bool resultsAreTheSame = true;
	for (int i = 0; i < lowercaseResults.size(); i++)
		resultsAreTheSame &= lowercaseResults[i] == searcher::searchResults[i];
	ASSERT_TRUE(resultsAreTheSame);
}

TEST(SearcherTests, AllTheResultsContainTheSearchWordWithoutMatchingCase)
{
	searcher::search("vector", 5, 20);
	bool allResultsContainTheSearchWord = true;
	for (std::string* result : searcher::searchResults)
	{
		std::string resultCopy = *result;
		for (int i = 0; i < resultCopy.length(); i++)
		{
			if (resultCopy[i] >= 'A')
				resultCopy[i] = tolower(resultCopy[i]);
		}
		std::cout << "result copy " << resultCopy << std::endl;
		allResultsContainTheSearchWord &= resultCopy.find("vector") != std::string::npos;
	}
	ASSERT_TRUE(allResultsContainTheSearchWord);
}

TEST(SearcherTests, SearchReturnsTheExpectedResults)
{
	searcher::search("i", 1, 20);
	ASSERT_TRUE(stringsAreEqual(searcher::searchResults[0]->c_str(), "Int"));
	ASSERT_TRUE(stringsAreEqual(searcher::searchResults[1]->c_str(), "Flip"));
	ASSERT_TRUE(stringsAreEqual(searcher::searchResults[2]->c_str(), "Solid"));
	ASSERT_TRUE(stringsAreEqual(searcher::searchResults[3]->c_str(), "Invert"));
	ASSERT_TRUE(stringsAreEqual(searcher::searchResults[4]->c_str(), "Add ints"));
	ASSERT_TRUE(stringsAreEqual(searcher::searchResults[5]->c_str(), "Int vector"));
	ASSERT_TRUE(stringsAreEqual(searcher::searchResults[6]->c_str(), "Divide ints"));
	ASSERT_TRUE(stringsAreEqual(searcher::searchResults[7]->c_str(), "Subtract ints"));
	ASSERT_TRUE(stringsAreEqual(searcher::searchResults[8]->c_str(), "Multiply ints"));
	ASSERT_TRUE(stringsAreEqual(searcher::searchResults[9]->c_str(), "Divide floats"));
	ASSERT_TRUE(stringsAreEqual(searcher::searchResults[10]->c_str(), "Image from file"));
	ASSERT_TRUE(stringsAreEqual(searcher::searchResults[11]->c_str(), "Linear Gradient"));
	ASSERT_TRUE(stringsAreEqual(searcher::searchResults[12]->c_str(), "Add int vectors"));
	ASSERT_TRUE(stringsAreEqual(searcher::searchResults[13]->c_str(), "Multiply floats"));
	ASSERT_TRUE(stringsAreEqual(searcher::searchResults[14]->c_str(), "Combine channels"));
	ASSERT_TRUE(stringsAreEqual(searcher::searchResults[15]->c_str(), "Gamma Correction"));
	ASSERT_TRUE(stringsAreEqual(searcher::searchResults[16]->c_str(), "Separate int vector"));
	ASSERT_TRUE(stringsAreEqual(searcher::searchResults[17]->c_str(), "Color from RGBA ints"));
	ASSERT_TRUE(stringsAreEqual(searcher::searchResults[18]->c_str(), "RGBA ints from Color"));
	ASSERT_TRUE(stringsAreEqual(searcher::searchResults[19]->c_str(), "Int vector from ints"));
}

TEST(SearcherTests, GetDataFunctionTest)
{
	ASSERT_TRUE(stringsAreEqual(searcher::getDataFor(0)->nodeName.c_str(), "Int"));
	ASSERT_TRUE(stringsAreEqual(searcher::getDataFor(1)->nodeName.c_str(), "Flip"));
	ASSERT_TRUE(stringsAreEqual(searcher::getDataFor(2)->nodeName.c_str(), "Solid"));
	ASSERT_TRUE(stringsAreEqual(searcher::getDataFor(3)->nodeName.c_str(), "Invert"));
	ASSERT_TRUE(stringsAreEqual(searcher::getDataFor(4)->nodeName.c_str(), "Add ints"));
	ASSERT_TRUE(stringsAreEqual(searcher::getDataFor(5)->nodeName.c_str(), "Int vector"));
	ASSERT_TRUE(stringsAreEqual(searcher::getDataFor(6)->nodeName.c_str(), "Divide ints"));
	ASSERT_TRUE(stringsAreEqual(searcher::getDataFor(7)->nodeName.c_str(), "Subtract ints"));
	ASSERT_TRUE(stringsAreEqual(searcher::getDataFor(8)->nodeName.c_str(), "Multiply ints"));
	ASSERT_TRUE(stringsAreEqual(searcher::getDataFor(9)->nodeName.c_str(), "Divide floats"));
	ASSERT_TRUE(stringsAreEqual(searcher::getDataFor(10)->nodeName.c_str(), "Image from file"));
	ASSERT_TRUE(stringsAreEqual(searcher::getDataFor(11)->nodeName.c_str(), "Linear Gradient"));
	ASSERT_TRUE(stringsAreEqual(searcher::getDataFor(12)->nodeName.c_str(), "Add int vectors"));
	ASSERT_TRUE(stringsAreEqual(searcher::getDataFor(13)->nodeName.c_str(), "Multiply floats"));
	ASSERT_TRUE(stringsAreEqual(searcher::getDataFor(14)->nodeName.c_str(), "Combine channels"));
	ASSERT_TRUE(stringsAreEqual(searcher::getDataFor(15)->nodeName.c_str(), "Gamma Correction"));
	ASSERT_TRUE(stringsAreEqual(searcher::getDataFor(16)->nodeName.c_str(), "Separate int vector"));
	ASSERT_TRUE(stringsAreEqual(searcher::getDataFor(17)->nodeName.c_str(), "Color from RGBA ints"));
	ASSERT_TRUE(stringsAreEqual(searcher::getDataFor(18)->nodeName.c_str(), "RGBA ints from Color"));
	ASSERT_TRUE(stringsAreEqual(searcher::getDataFor(19)->nodeName.c_str(), "Int vector from ints"));
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	nodeProvider::initialize();
	return RUN_ALL_TESTS();
}