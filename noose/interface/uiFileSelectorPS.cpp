#ifdef USE_POWERSHELL_FILE_SELECTION
#include <string.h>
#include <stdio.h>
#include <iostream>
#include "uiFileSelector.h"
#include "../utils.h"

char* uiFileSelector::openFileDialog()
{
	char* filePath = (char*)malloc(sizeof(char) * 1024);
	filePath[0] = '\0';
	FILE* f = _popen("\"interface\\uiFileSelectorWindowsPS.bat\"", "r");
	fgets(filePath, 1024, f);

	if (filePath[0] == '\0')
	{
		std::cout << "[Powershell handler] Could not select file\n";
		free(filePath);
		return nullptr;
	}

	// remove endline
	int i; for (i = 0; filePath[i] != '\n'; i++);
	filePath[i] = '\0';
	return filePath;
}
char* uiFileSelector::saveFileDialog(const std::string& fileExtension)
{
	char* filePath = (char*)malloc(sizeof(char) * 1024);
	filePath[0] = '\0';
	FILE* f = _popen("\"interface\\uiFileSelectorSaveWindowsPS.bat\"", "r");
	fgets(filePath, 1024, f);

	if (filePath[0] == '\0')
	{
		std::cout << "[Powershell handler] Could not select file\n";
		delete[] filePath;
		return nullptr;
	}

	// remove endline
	int i; for (i = 0; filePath[i] != '\n'; i++);
	filePath[i] = '\0';

	if (utils::fileHasExtension(filePath, fileExtension.c_str()))
	{
		return filePath;
	}
	else
	{
		// add 4 chars for the extension and 1 for \0
		int prevLength = strlen(filePath);
		char* fixedPath = (char*)malloc(sizeof(char) * (prevLength + fileExtension.length() + 2)); // include dot and \0
		if (fixedPath == NULL)
		{
			std::cout << "[Powershell handler] No memory available\n";
			return nullptr;
		}
		memcpy(fixedPath, filePath, prevLength);
		memcpy(&(fixedPath[prevLength]), ('.' + fileExtension).c_str(), fileExtension.length() + 2); // include dot and \0
		free(filePath);
		return fixedPath;
	}

	return filePath;
}
#endif