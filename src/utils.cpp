#include "utils.h"

#include <cmath>
#include <clip.h>

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

bool utils::imageFromClipboard(sf::Image& outImage)
{
    clip::image clipboardImage;
    if (!clip::get_image(clipboardImage) || !clipboardImage.is_valid())
        return false;
    clip::image_spec spec = clipboardImage.spec();
    outImage.create({ spec.width, spec.height }, sf::Color(255u, 255u, 255u, 255u));
    for (unsigned long y = 0; y < spec.height; ++y)
    {
        char* p = clipboardImage.data() + spec.bytes_per_row * y;
        for (unsigned long x = 0; x < spec.width; ++x)
        {
            sf::Color pixelColor;
            pixelColor.a = 255;
            for (unsigned long byte = 0; byte < spec.bits_per_pixel / 8; ++byte, ++p)
            {
                uint8_t* pixelPointer = (uint8_t*)p;
                if (byte == spec.red_shift / 8)
                    pixelColor.r = *pixelPointer;
                else if (byte == spec.green_shift / 8)
                    pixelColor.g = *pixelPointer;
                else if (byte == spec.blue_shift / 8)
                    pixelColor.b = *pixelPointer;
                else if (byte == spec.alpha_shift / 8)
                    pixelColor.a = *pixelPointer;
            }
            outImage.setPixel(sf::Vector2u(x, y), pixelColor);
        }
    }
    return true;
}

bool utils::imageToClipboard(const sf::Image& image)
{
    clip::image_spec spec;
    spec.width = image.getSize().x;
    spec.height = image.getSize().y;
    spec.bits_per_pixel = 32;
    spec.bytes_per_row = 4 * image.getSize().x;
    spec.red_mask = 0xff;
    spec.green_mask = 0xff00;
    spec.blue_mask = 0xff0000;
    spec.alpha_mask = 0xff000000;
    spec.red_shift = 0;
    spec.green_shift = 8;
    spec.blue_shift = 16;
    spec.alpha_shift = 24;
    char* clipboardImageBuffer = (char*)malloc(sizeof(char) * image.getSize().x * image.getSize().y * 4); // RGBA
    for (unsigned long y = 0; y < spec.height; ++y)
    {
        char* p = clipboardImageBuffer + spec.bytes_per_row * y;
        for (unsigned long x = 0; x < spec.width; ++x)
        {
            sf::Color pixelColor = image.getPixel({ x, y });
            for (unsigned long byte = 0; byte < spec.bits_per_pixel / 8; ++byte, ++p)
            {
                char* pixelPointer = (char*)p;
                if (byte == spec.red_shift / 8)
                    *pixelPointer = pixelColor.r;
                else if (byte == spec.green_shift / 8)
                    *pixelPointer = pixelColor.g;
                else if (byte == spec.blue_shift / 8)
                    *pixelPointer = pixelColor.b;
                else if (byte == spec.alpha_shift / 8)
                    *pixelPointer = pixelColor.a;
            }
        }
    }

    clip::image clipboardImage = clip::image(clipboardImageBuffer, spec);
    bool returnValue = clip::set_image(clipboardImage);
    free(clipboardImageBuffer);
    return returnValue;
}

bool utils::textFromClipboard(std::string& outString)
{
    return clip::get_text(outString);
}

bool utils::textToClipboard(const std::string& string)
{
    return clip::set_text(string);
}

void utils::drawQuads(sf::RenderTarget& renderTarget, const sf::Vertex* vertices, size_t vertexCount, const sf::RenderStates& renderStates)
{
    for (int i = 0; i < vertexCount; i += 4)
    {
        renderTarget.draw(&(vertices[i]), 4, sf::PrimitiveType::TriangleFan, renderStates);
    }
}
