#pragma once

#include <string>
#include <iostream>
#include <utils.h>

namespace uiHelp
{
	const std::string helpUrl = "http://sntcls.xyz/noose/help";
	inline void displayHelp()
	{
		std::cout << "[UI] Opening help url\n";
#ifdef NOOSE_PLATFORM_LINUX
		utils::pcall({ "xdg-open", helpUrl });
#endif
#ifdef NOOSE_PLATFORM_MACOS
		utils::pcall({ "open", helpUrl });
#endif
#ifdef NOOSE_PLATFORM_WINDOWS
		utils::pcall({ "explorer", helpUrl });
#endif
	}
}