#include <iostream>
#include <sstream>
#include "../noose/nodeProvider/nodeProvider.h"

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
	//-------------//

	//-- TESTING --//

	nodeProvider::status(ss);

	std::cout << "--------- fails ----------\n";
	
	// print report
	// std::cout << ss.str();

	if (!equalsAt(&(ss.str()[21]), "Image from file") ||
		!equalsAt(&(ss.str()[48]), "3, 3, 2") ||
		!equalsAt(&(ss.str()[69]), "in, out, size") ||
		!equalsAt(&(ss.str()[103]), "0, 0, 0") ||
		 equalsAt(&(ss.str()[131]), "\t") ||
		!equalsAt(&(ss.str()[148]), "1") ||
		!equalsAt(&(ss.str()[168]), "2") ||
		!equalsAt(&(ss.str()[191]), "x")) // first node loading
		std::cout << "failed\n";

	if (!equalsAt(&(ss.str()[525]), "x")) // default data loading
		std::cout << "failed\n";

	if (!equalsAt(&(ss.str()[1430]), "\t")) // selection box options loading
		std::cout << "failed\n";

	if (!equalsAt(&(ss.str()[7615]), "Divide floats") ||
		!equalsAt(&(ss.str()[7640]), "1, 1, 1") ||
		!equalsAt(&(ss.str()[7661]), "a, b, out") ||
		!equalsAt(&(ss.str()[7691]), "0, 0, 0") ||
		 equalsAt(&(ss.str()[7719]), "\t") ||
		!equalsAt(&(ss.str()[7736]), "2") ||
		!equalsAt(&(ss.str()[7756]), "1") ||
		!equalsAt(&(ss.str()[7779]), "x")) // first node loading
		std::cout << "failed\n";
}