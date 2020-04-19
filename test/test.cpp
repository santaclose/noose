#include <iostream>
#include "../noose/logic/nodeSystem.h"
#include "../noose/nodeProvider/nodeProvider.h"

#define LOGIC_PRINT_SATUS std::cout.clear();std::cout << "//--------------TEST--------------//\n";nodeSystem::printSystemStatus();std::cout << "//--------------------------------//\n";std::cout.setstate(std::ios_base::failbit);

int main()
{

	//---------------//
	nodeProvider::initialize();
	nodeSystem::initialize();
	//---------------//

	//-- NODE PROVIDER TEST --//


	//-- LOGICAL PART TEST --//

	std::vector<int> c;
	LOGIC_PRINT_SATUS
	nodeSystem::onNodeCreated(0, &(nodeProvider::nodeDataList[0]));
	LOGIC_PRINT_SATUS
	nodeSystem::onNodeDeleted(0, c);
	LOGIC_PRINT_SATUS
	nodeSystem::onNodeCreated(0, &(nodeProvider::nodeDataList[0]));
	LOGIC_PRINT_SATUS
	nodeSystem::onNodeCreated(1, &(nodeProvider::nodeDataList[4]));
	LOGIC_PRINT_SATUS
	nodeSystem::onNodeDeleted(0, c);
	LOGIC_PRINT_SATUS
	nodeSystem::onNodeCreated(0, &(nodeProvider::nodeDataList[1]));
	LOGIC_PRINT_SATUS
	nodeSystem::onNodeCreated(0, &(nodeProvider::nodeDataList[2]));
	LOGIC_PRINT_SATUS
	nodeSystem::onNodesConnected(0, 1, 2, 0, 0);
	LOGIC_PRINT_SATUS
	nodeSystem::onNodesDisconnected(0, 1, 2, 0, 0);
	LOGIC_PRINT_SATUS
	nodeSystem::onNodesConnected(0, 1, 2, 0, 0);
	LOGIC_PRINT_SATUS
	c.push_back(0);
	nodeSystem::onNodeDeleted(0, c);
	LOGIC_PRINT_SATUS
}
