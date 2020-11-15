#pragma once

#include <string>
#include <iostream>
#include <utils.h>

namespace uiHelp
{
	inline void displayHelp()
	{
		std::cout << "[UI] Opening help HTML\n";
		std::string command;
		std::string localPath;
#ifdef NOOSE_PLATFORM_LINUX
		command = "xdg-open ";
		localPath = "assets/help/help.html";
		//system("xdg-open " + utils::getProgramDirectory() + "help/help.html");
#endif
#ifdef NOOSE_PLATFORM_MACOS
		command = "open ";
		localPath = "assets/help/help.html";
		//system("open " + utils::getProgramDirectory() + "help/help.html");
#endif
#ifdef NOOSE_PLATFORM_WINDOWS
		command = "start \"\" ";
		localPath = "assets\\help\\help.html";
		//system("start " + utils::getProgramDirectory() + "help\\help.html");
#endif
		command += "\"" + utils::getProgramDirectory() + localPath + "\"";
		system(command.c_str());
	}
}