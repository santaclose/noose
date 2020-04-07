#ifdef WINDOWS
#include <string.h>
#include <iostream>
#include "uiFileSelector.h"
#include "../vendor/nfd.h"
#include "../utils.h"

char* uiFileSelector::openFileDialog()
{
	nfdchar_t* outPath = nullptr;
	nfdresult_t result = NFD_OpenDialog("bmp,png,tga,jpg,gif,psd,hdr,pic", NULL, &outPath);

	if (result == NFD_OKAY)
	{
		return outPath;
	}
	else if (result == NFD_CANCEL)
	{
		puts("[NFD] User pressed cancel.");
		return nullptr;
	}
	else
	{
		printf("[NFD] Error: %s\n", NFD_GetError());
		return nullptr;
	}
}

char* uiFileSelector::saveFileDialog(const std::string& fileExtension)
{
	nfdchar_t* outPath = nullptr;
	nfdresult_t result = NFD_SaveDialog(fileExtension.c_str(), NULL, &outPath);

	if (result == NFD_OKAY)
	{
		puts("[NFD] Success");
		if (utils::fileHasExtension(outPath, fileExtension.c_str()))
		{
			return outPath;
		}
		else
		{
			// add 4 chars for the extension and 1 for \0
			int prevLength = strlen(outPath);
			char* fixedPath = (char*)malloc(sizeof(char) * (prevLength + fileExtension.length() + 2)); // include dot and \0
			if (fixedPath == NULL)
			{
				std::cout << "[NFD] No memory available\n";
				return nullptr;
			}
			memcpy(fixedPath, outPath, prevLength);
			memcpy(&(fixedPath[prevLength]), ('.' + fileExtension).c_str(), fileExtension.length() + 2); // include dot and \0
			free(outPath);
			return fixedPath;
		}
	}
	else if (result == NFD_CANCEL)
	{
		puts("[NFD] User pressed cancel.");
		return nullptr;
	}
	else
	{
		printf("[NFD] Error: %s\n", NFD_GetError());
		return nullptr;
	}
}
#endif