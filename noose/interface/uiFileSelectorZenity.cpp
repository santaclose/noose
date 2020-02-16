#include "uiFileSelector.h"
#include <iostream>

char* uiFileSelector::SelectFile()
{
	char* filePath = new char[1024];
	filePath[0] = '\0';
	FILE* f = popen("zenity --file-selection", "r");
	fgets(filePath, 1024, f);

	if (filePath[0] == '\0')
	{
		std::cout << "[Zenity] Could not select file\n";
		delete[] filePath;
		return nullptr;
	}

	// remove endline
	int i; for (i = 0; filePath[i] != '\n'; i++);
	filePath[i] = '\0';
	return filePath;
}