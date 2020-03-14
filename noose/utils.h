#pragma once
namespace utils
{
	/*inline char charToLower(char c)
	{
		if (c <= 'Z' && c >= 'A')
			return c - ('Z' - 'z');
		return c;
	}*/
	inline char* getFileNameFromPath(char* path)
	{
		int i = 0;
		for (; path[i] != '\0'; i++);
		for (; path[i - 1] != '\\' && path[i - 1] != '/'; i--);
		return &(path[i]);
	}
	inline bool fileHasExtension(char* path)
	{
		int i = 0;
		for (; path[i] != '\0'; i++);
		for (;;i--)
		{
			if (i == -1)
				return false;
			if (path[i] == '\\' || path[i] == '/')
				return false;
			if (path[i] == '.')
				return true;
		}
	}
};

