#pragma once

#include <string>
#include <vector>

namespace pathUtils
{
	void setProgramDirectory(const std::string& executablePath);
	const std::string& getProgramDirectory();
	const std::string& getAssetsDirectory();

	std::string getRelativePath(const std::string& fileRelativeTo, const std::string& targetPath);
	std::string getFolderPath(const std::string& filePath);
	void getCustomNodeFiles(std::vector<std::string>& fileList);

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
		for (;; i--)
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
}