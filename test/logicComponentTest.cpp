#include <iostream>
#include <sstream>
#include "../noose/logic/nodeSystem.h"
#include "../noose/nodeProvider/nodeProvider.h"

#define LOGIC_PRINT_STATUS nodeSystem::status(ss);ss<<"//--------------------------------//\n";

bool equalsAt(char* string, char* substring)
{
	int substringIndex = 0;
	for (int substringIndex = 0;
		 substring[substringIndex] != '\0';
		 substringIndex++)
		if (substring[substringIndex] != string[substringIndex])
			return false;
	return true;
}

int main()
{
	std::stringstream ss;
	//-------------//
	nodeProvider::initialize();
	nodeSystem::initialize();
	//-------------//

	//-- TESTING --//

	std::vector<int> c;
	LOGIC_PRINT_STATUS
	nodeSystem::onNodeCreated(0, &(nodeProvider::nodeDataList[0]));
	LOGIC_PRINT_STATUS
	nodeSystem::onNodeDeleted(0, c);
	LOGIC_PRINT_STATUS
	nodeSystem::onNodeCreated(0, &(nodeProvider::nodeDataList[0]));
	LOGIC_PRINT_STATUS
	nodeSystem::onNodeCreated(1, &(nodeProvider::nodeDataList[4]));
	LOGIC_PRINT_STATUS
	nodeSystem::onNodeDeleted(0, c);
	LOGIC_PRINT_STATUS
	nodeSystem::onNodeCreated(0, &(nodeProvider::nodeDataList[1]));
	LOGIC_PRINT_STATUS
	nodeSystem::onNodeCreated(0, &(nodeProvider::nodeDataList[2]));
	LOGIC_PRINT_STATUS
	nodeSystem::onNodesConnected(0, 1, 2, 0, 0);
	LOGIC_PRINT_STATUS
	nodeSystem::onNodesDisconnected(0, 1, 2, 0, 0);
	LOGIC_PRINT_STATUS
	nodeSystem::onNodesConnected(0, 1, 2, 0, 0);
	LOGIC_PRINT_STATUS
	c.push_back(0);
	nodeSystem::onNodeDeleted(0, c);
	LOGIC_PRINT_STATUS
	std::cout << "--------- fails ----------\n";
	
	// print report
	// std::cout << ss.str();

	if (!equalsAt(&(ss.str()[75]), "\t")) // created successfully
		std::cout << "failed\n";

	if (!equalsAt(&(ss.str()[368]), "nullptr")) // deleted successfully
		std::cout << "failed\n";

	if (!equalsAt(&(ss.str()[460]), "\t")) // created successfully again
		std::cout << "failed\n";

	if (!equalsAt(&(ss.str()[962]), "\t")) // another node created successfully
		std::cout << "failed\n";

	if (!equalsAt(&(ss.str()[1289]), "nullptr")) // node 0 deleted successfully
		std::cout << "failed\n";

	if (!equalsAt(&(ss.str()[1645]), "\t")) // node 0 recreated successfully, slot reused
		std::cout << "failed\n";

	if (!equalsAt(&(ss.str()[2237]), "0")) // node 0 overwritten successfully
		std::cout << "failed\n";

	if (!equalsAt(&(ss.str()[3293]), "-") ||
		!equalsAt(&(ss.str()[3305]), "0")) // nodes connected successfully
		std::cout << "failed\n";

	if (!equalsAt(&(ss.str()[3971]), "1")) // nodes disconnected successfully
		std::cout << "failed\n";

	if (!equalsAt(&(ss.str()[4681]), "0")) // nodes reconnected successfully, slot reused
		std::cout << "failed\n";

	if (!equalsAt(&(ss.str()[4828]), "nullptr") ||
		!equalsAt(&(ss.str()[5126]), "1") ) // node deleted while connected successfully
		std::cout << "failed\n";
}