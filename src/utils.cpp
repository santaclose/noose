#include "utils.h"

#include <cmath>

#define min_f(a, b, c)  (std::fminf(a, std::fminf(b, c)))
#define max_f(a, b, c)  (std::fmaxf(a, std::fmaxf(b, c)))

char utils::lower(char in)
{
    if (in <= 'Z' && in >= 'A')
        return in - ('Z' - 'z');
    return in;
}
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

bool utils::colorFromHexString(const std::string& hexString, sf::Color& outColor)
{
    uint8_t res_arr[4];

    if (hexString.length() == 8 || hexString.length() == 6)
    {
        for (int i = 0; i < 3; i++)
        {
            char a = lower(hexString[i * 2 + 0]) > '9' ? lower(hexString[i * 2 + 0]) - 'a' + 10 : lower(hexString[i * 2 + 0]) - '0';
            char b = lower(hexString[i * 2 + 1]) > '9' ? lower(hexString[i * 2 + 1]) - 'a' + 10 : lower(hexString[i * 2 + 1]) - '0';
            res_arr[i] = a * 16 + b;
        }
        if (hexString.length() == 8)
        {
            char a = lower(hexString[3 * 2 + 0]) > '9' ? lower(hexString[3 * 2 + 0]) - 'a' + 10 : lower(hexString[3 * 2 + 0]) - '0';
            char b = lower(hexString[3 * 2 + 1]) > '9' ? lower(hexString[3 * 2 + 1]) - 'a' + 10 : lower(hexString[3 * 2 + 1]) - '0';
            res_arr[3] = a * 16 + b;
        }
        else
            res_arr[3] = 255;
        outColor = sf::Color(res_arr[0], res_arr[1], res_arr[2], res_arr[3]);
        return true;
    }
    else if (hexString.length() == 4 || hexString.length() == 3)
    {
        for (int i = 0; i < 3; i++)
        {
            char a = lower(hexString[i]) > '9' ? lower(hexString[i]) - 'a' + 10 : lower(hexString[i]) - '0';
            res_arr[i] = a * 16 + a;
        }
        if (hexString.length() == 4)
        {
            char a = lower(hexString[3]) > '9' ? lower(hexString[3]) - 'a' + 10 : lower(hexString[3]) - '0';
            res_arr[3] = a * 16 + a;
        }
        else
            res_arr[3] = 255;
        outColor = sf::Color(res_arr[0], res_arr[1], res_arr[2], res_arr[3]);
        return true;
    }

    return false;
}

void utils::drawQuads(sf::RenderTarget& renderTarget, const sf::Vertex* vertices, size_t vertexCount, const sf::RenderStates& renderStates)
{
    for (int i = 0; i < vertexCount; i += 4)
    {
        renderTarget.draw(&(vertices[i]), 4, sf::PrimitiveType::TriangleFan, renderStates);
    }
}
