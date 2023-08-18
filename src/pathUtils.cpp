#include "pathUtils.h"

#include <filesystem>
#include <iostream>

#ifdef NOOSE_PLATFORM_WINDOWS
#define PATH_SEP '\\'
#else
#define PATH_SEP '/'
#endif

std::string programDirectory;
std::string assetsDirectory;

void pathUtils::setProgramDirectory(const std::string& executableFilePath)
{
    programDirectory = getFolderPath(executableFilePath);
    assetsDirectory = programDirectory + "assets" + PATH_SEP;
    if (!std::filesystem::exists(assetsDirectory))
    {
        // asset directory is different when launching from visual studio
        std::string parent = getFolderPath(executableFilePath.substr(0, executableFilePath.length() - 1));
        parent = getFolderPath(parent.substr(0, parent.length() - 1));
        parent = getFolderPath(parent.substr(0, parent.length() - 1));
        parent = getFolderPath(parent.substr(0, parent.length() - 1));

        assetsDirectory = parent + "assets" + PATH_SEP;
    }
    if (!std::filesystem::exists(assetsDirectory))
        std::cout << "[Path utils] Warning, could not locate assets folder\n";
    else
        std::cout << "[Path utils] Assets directory set to: " << assetsDirectory << std::endl;
}

const std::string& pathUtils::getProgramDirectory()
{
    return programDirectory;
}

const std::string& pathUtils::getAssetsDirectory()
{
    return assetsDirectory;
}

std::string pathUtils::getRelativePath(const std::string& fileRelativeTo, const std::string& targetPath)
{
    std::string out = "";

    std::vector<std::string> pathA, pathB;

    int i = fileRelativeTo.length() - 1, iEnd = fileRelativeTo.length();
    int j = targetPath.length() - 1, jEnd = targetPath.length();
    while (i > -1)
    {
        if (fileRelativeTo[i] == '/' || fileRelativeTo[i] == '\\')
        {
            pathA.insert(pathA.begin(), fileRelativeTo.substr(i + 1, iEnd - i - 1));
            iEnd = i;
        }
        i--;
    }
    while (j > -1)
    {
        if (targetPath[j] == '/' || targetPath[j] == '\\')
        {
            pathB.insert(pathB.begin(), targetPath.substr(j + 1, jEnd - j - 1));
            jEnd = j;
        }
        j--;
    }

    i = 0; j = 0;
    while (pathA[i].compare(pathB[j]) == 0) { i++; j++; }

    while (i < pathA.size() - 1)
    {
        out += "../";
        i++;
    }
    while (j < pathB.size())
    {
        out += pathB[j];
        if (j != pathB.size() - 1)
            out += '/';
        j++;
    }
    return out;
}

std::string pathUtils::getFolderPath(const std::string& filePath)
{
    int i = filePath.length() - 1;
    for (; i > -1 && filePath[i] != '/' && filePath[i] != '\\'; i--) {};
    return filePath.substr(0, i + 1);
}