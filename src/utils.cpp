#include "utils.h"

std::string programDirectory;

void utils::setProgramDirectory(const std::string& executableFilePath)
{
    programDirectory = getFolderPath(executableFilePath);
}

const std::string& utils::getProgramDirectory()
{
    return programDirectory;
}

std::string utils::getRelativePath(const std::string& fileRelativeTo, const std::string& targetPath)
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

std::string utils::getFolderPath(const std::string& filePath)
{
    int i = filePath.length() - 1;
    for (; i > -1 && filePath[i] != '/' && filePath[i] != '\\';
        i--) {};
    return filePath.substr(0, i+1);
}
