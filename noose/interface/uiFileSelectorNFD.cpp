#ifdef WINDOWS
#include <string.h>
#include "uiFileSelector.h"
#include "../vendor/nfd.h"
#include "../utils.h"

char* uiFileSelector::selectFile(bool save)
{
	nfdchar_t* outPath = nullptr;
	//nfdchar_t filter[] = "bmp,png,tga,jpg,gif,psd,hdr,pic";

	nfdresult_t result;
	if (save)
		result = NFD_SaveDialog("png", NULL, &outPath);
	else
		result = NFD_OpenDialog("bmp,png,tga,jpg,gif,psd,hdr,pic", NULL, &outPath);

	if (result == NFD_OKAY)
	{
		puts("[NFD] Success");
		if (save)
		{
			if (utils::fileHasExtension(outPath))
				return outPath;
			else
			{
				// add 4 chars for the extension and 1 for \0
				int prevLength = strlen(outPath);
				char* fixedPath = (char*) malloc(sizeof(char) * (prevLength + 4 + 1)); //new char[prevLength + 4 + 1];
				memcpy(fixedPath, outPath, prevLength);
				memcpy(&(fixedPath[prevLength]), ".png\0", 5);
				free(outPath);
				return fixedPath;
			}
		}
		else
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
#endif