#pragma once
#include <sstream>
#include <iomanip>
#include <queue>
#include <SFML/Graphics.hpp>

namespace utils
{
	void setProgramDirectory(const std::string& executablePath);
	const std::string& getProgramDirectory();

	std::string getRelativePath(const std::string& fileRelativeTo, const std::string& targetPath);
	std::string getFolderPath(const std::string& filePath);

	void HSVtoRGB(int H, double S, double V, sf::Color& output);

	inline const char* getFileNameFromPath(const char* path)
	{
		int i = 0;
		for (; path[i] != '\0'; i++);
		for (; path[i - 1] != '\\' && path[i - 1] != '/'; i--);
		return &(path[i]);
	}
	inline bool fileHasExtension(const char* path, const char* ext)
	{
		int i = 0;
		for (; path[i] != '\0'; i++);
		for (;;i--)
		{
			if (i == -1 || path[i] == '\\' || path[i] == '/')
				return false;
			if (path[i] == '.')
			{
				i++;
				for (int j = 0; ext[j] != '\0'; j++)
				{
					if (ext[j] != path[i + j])
						return false;
				}
				return true;
			}
		}
	}

	template <typename T>
	std::string intToHex(T i)
	{
		std::stringstream stream;
		stream
			<< std::setfill ('0') << std::setw(sizeof(T)*2) 
			<< std::hex << i;
		return stream.str();
	}
}