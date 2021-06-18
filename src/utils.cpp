#include "utils.h"

#include <cmath>

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

#define min_f(a, b, c)  (std::fminf(a, std::fminf(b, c)))
#define max_f(a, b, c)  (std::fmaxf(a, std::fmaxf(b, c)))
void utils::rgb2hsv(const sf::Color& rgbColor, float& h, float& s, float& v)
{
    float r = rgbColor.r / 255.0f;
    float g = rgbColor.g / 255.0f;
    float b = rgbColor.b / 255.0f;

    //float h, s, v; // h:0-360.0, s:0.0-1.0, v:0.0-1.0

    float max = max_f(r, g, b);
    float min = min_f(r, g, b);

    v = max;

    if (max == 0.0f) {
        s = 0;
        h = 0;
    }
    else if (max - min == 0.0f) {
        s = 0;
        h = 0;
    }
    else {
        s = (max - min) / max;

        if (max == r) {
            h = 60 * ((g - b) / (max - min)) + 0;
        }
        else if (max == g) {
            h = 60 * ((b - r) / (max - min)) + 120;
        }
        else {
            h = 60 * ((r - g) / (max - min)) + 240;
        }
    }

    if (h < 0) h += 360.0f;
}