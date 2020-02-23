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
};

