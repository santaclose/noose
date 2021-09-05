#include "utils.h"

#include <cmath>

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