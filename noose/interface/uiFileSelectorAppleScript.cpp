#ifdef MACOS
#include <string.h>
#include <iostream>
#include "uiFileSelector.h"
#include "../utils.h"

char* fixAppleScriptOutput(char* in)
{
	int startIndex = -1;
	int i = 0;
	for (; in[i] != '\0'; i++)
	{
		if (startIndex < 0 && in[i] == ':')
			startIndex = i;
		if (in[i] == ':')
			in[i] = '/';
		if (in[i] == '\n')
		{
			in[i] = '\0';
			break;
		}
	}
	char* out = (char*) malloc(sizeof(char) * (i - startIndex + 1));
	memcpy(out, &(in[startIndex]), i - startIndex + 1);
	free(in);
	return out;
}

char* uiFileSelector::openFileDialog()
{
	char* filePath = (char*) malloc(sizeof(char) * 1024);
	filePath[0] = '\0';
	FILE* f = popen("osascript interface/uiFileSelectorMacOS.scpt", "r");
	fgets(filePath, 1024, f);

	if (filePath[0] == '\0')
	{
		std::cout << "[AppleScript handler] Could not select file\n";
		free(filePath);
		return nullptr;
	}
	filePath = fixAppleScriptOutput(filePath);

	return filePath;
}
char* uiFileSelector::saveFileDialog(const std::string& fileExtension)
{
	char* filePath = (char*) malloc(sizeof(char) * 1024);
	filePath[0] = '\0';
	FILE* f = popen("osascript interface/uiFileSelectorSaveMacOS.scpt", "r");
	fgets(filePath, 1024, f);

	if (filePath[0] == '\0')
	{
		std::cout << "[AppleScript handler] Could not select file\n";
		delete[] filePath;
		return nullptr;
	}

	filePath = fixAppleScriptOutput(filePath);

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
			std::cout << "[AppleScript handler] No memory available\n";
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