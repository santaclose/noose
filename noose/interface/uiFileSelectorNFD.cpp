#include "uiFileSelector.h"

#include "../vendor/nfd.h"

char* uiFileSelector::SelectFile()
{
	nfdchar_t* outPath = nullptr;
	nfdchar_t filter[] = "bmp,png,tga,jpg,gif,psd,hdr,pic";
	nfdresult_t result = NFD_OpenDialog(filter, NULL, &outPath);

	if (result == NFD_OKAY)
	{
		puts("[NFD] Success");
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