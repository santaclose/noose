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
		std::string command;
#ifdef NOOSE_PLATFORM_LINUX
		command = "xdg-open ";
#endif
#ifdef NOOSE_PLATFORM_MACOS
		command = "open ";
#endif
#ifdef NOOSE_PLATFORM_WINDOWS
		command = "explorer ";
#endif
		command += "\"" + helpUrl + "\"";
		system(command.c_str());
	}
}