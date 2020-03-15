#ifdef LINUX
#include <string.h>
#include <iostream>
#include "uiFileSelector.h"
#include "../utils.h"

char* uiFileSelector::selectFile(bool save)
{
	char* filePath = (char*) malloc(sizeof(char) * 1024);
	filePath[0] = '\0';
	FILE* f;
	if (save)
		f = popen("zenity --file-selection --save", "r");
	else
		f = popen("zenity --file-selection", "r");
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

	if (save)
	{
		if (utils::fileHasExtension(filePath))
			return filePath;
		else
		{
			// add 4 chars for the extension and 1 for \0
			int prevLength = strlen(filePath);
			char* fixedPath = (char*) malloc(sizeof(char) * (prevLength + 4 + 1));
			memcpy(fixedPath, filePath, prevLength);
			memcpy(&(fixedPath[prevLength]), ".png\0", 5);
			free(filePath);
			return fixedPath;
		}
	}

	return filePath;
}
#endif