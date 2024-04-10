#include "utils.h"

#include <cmath>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <array>
#include <regex>
#include <fstream>
#include <sstream>
#include <thread>

#include <clip.h>
#include <base64.h>
#include <portable-file-dialogs.h>
#include <subprocess.hpp>

#include "pathUtils.h"
#include "types.h"

#define min_f(a, b, c)  (std::fminf(a, std::fminf(b, c)))
#define max_f(a, b, c)  (std::fmaxf(a, std::fmaxf(b, c)))

int utils::mod(int a, int b)
{
	int ret = a % b;
	if (ret < 0)
		ret += b;
	return ret;
}

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

std::string utils::hexStringFromColor(const sf::Color& color)
{
    uint32_t colorInt = color.toInteger();
    return intToHex(colorInt);
}

bool utils::vector2iFromString(const std::string& string, sf::Vector2i& outVector)
{
    int commaIndex = 0;
    for (; commaIndex < string.length() && string[commaIndex] != ','; commaIndex++);
    if (commaIndex == string.length())
        return false;
    outVector.x = atoi(string.substr(0, commaIndex).c_str());
    outVector.y = atoi(string.substr(commaIndex + 1).c_str());
    return true;
}

bool utils::imageFromBase64String(const std::string& base64String, sf::Image& out)
{
    std::string pngBytes = base64::decode(base64String);
    return out.loadFromMemory(pngBytes.data(), pngBytes.length());
}

bool utils::base64StringFromImage(const sf::Image& image, std::string& out)
{
    std::optional<std::vector<uint8_t>> pngBytes;
    // allocate max we would need for assert not to show up in debug mode
    // https://stackoverflow.com/questions/35310117/debug-assertion-failed-expression-acrt-first-block-header
    //pngBytes.reserve(image.getSize().x * image.getSize().y * 4);
    pngBytes = image.saveToMemory("png");
    if (!pngBytes.has_value())
        return false;
    out = base64::encode(pngBytes.value().data(), pngBytes.value().size());
    return true;
}

static sf::Shader loadImageShader;
static bool loadImageShaderLoaded = false;
bool utils::drawImageToRenderTexture(const sf::Texture& image, sf::RenderTexture& renderTexture)
{
    if (!loadImageShaderLoaded)
    {
        if (!loadImageShader.loadFromFile(pathUtils::getAssetsDirectory() + "shaders/loadImage.shader", sf::Shader::Type::Fragment))
            std::cout << "[Utils] Failed to load image loading shader\n";
        loadImageShaderLoaded = true;
    }
    loadImageShader.setUniform("tx", image);
    sf::Sprite spr(image);
    sf::Vector2u txSize = image.getSize();
    if (!renderTexture.create(txSize))
    {
        std::cout << "[Utils] Failed to create render texture\n";
        return false;
    }
    sf::RenderStates rs;
    rs.shader = &loadImageShader;
    rs.blendMode = sf::BlendNone;
    renderTexture.draw(spr, rs);
    return true;
}

bool utils::drawImageToRenderTexture(const sf::Image& image, sf::RenderTexture& renderTexture)
{
    sf::Texture tx;
    if (!tx.loadFromImage(image))
    {
        std::cout << "[Utils] Failed to create texture from image\n";
        return false;
    }
    return drawImageToRenderTexture(tx, renderTexture);
}

bool utils::drawImageToRenderTexture(const std::string& imageFilePath, sf::RenderTexture& renderTexture)
{
    sf::Texture tx;
    if (!tx.loadFromFile(imageFilePath))
    {
        std::cout << "[Utils] Failed to open image file\n";
        return false;
    }
    return drawImageToRenderTexture(tx, renderTexture);
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

void utils::osShowNotification(const std::string& text)
{
    pfd::notify("", text);
}

std::string utils::osSaveFileDialog(const std::string& title, const std::string& extensionDropDownTitle, const std::string& extension)
{
    return pfd::save_file(title, "", { extensionDropDownTitle, extension }).result();
}

std::vector<std::string> utils::osOpenFileDialog(const std::string& title, const std::string& extensionDropDownTitle, const std::string& extensions)
{
    return pfd::open_file(title, "", { extensionDropDownTitle, extensions }).result();
}

utils::osChoice utils::osYesNoMessageBox(const std::string& title, const std::string& message)
{
    pfd::button choice = pfd::message(title, message, pfd::choice::yes_no, pfd::icon::warning).result();
    return (osChoice)choice;
}

int utils::pcall(const std::vector<std::string>& cmd)
{
    return subprocess::call(cmd);
}

std::string utils::pcheck_output(const std::vector<std::string>& cmd)
{
    subprocess::Buffer out = subprocess::check_output(cmd);
    std::string result;
    result.resize(out.length + 1);
    memcpy(result.data(), out.buf.data(), result.size());
    return result;
}

void utils::installUpdate(const std::string& updateUrl)
{
    std::cout << "[Utils] Installing update from " + updateUrl;
#ifdef NOOSE_PLATFORM_WINDOWS
    utils::pcall({ pathUtils::getAssetsDirectory() + "autoUpdate\\win32.bat", pathUtils::getProgramDirectory(), updateUrl });
#endif // NOOSE_PLATFORM_WINDOWS
}

void utils::checkForUpdates()
{
    std::string latestVersion;
    float latestVersionF = -1.0f;
    std::string installedVersion;
    float installedVersionF;

    // get current version
    std::ifstream versiondat(pathUtils::getProgramDirectory() + "version.dat");
    if (!versiondat.good())
    {
        std::cout << "[Utils] Version file not found, skipping update check\n";
        return;
    }
    std::stringstream buffer;
    buffer << versiondat.rdbuf();
    installedVersion = buffer.str();
    installedVersionF = std::stof(installedVersion);

    // get latest version
    std::string html = pcheck_output({ "curl", "https://github.com/santaclose/noose/releases" });
    std::regex re("release.*>(\\d+\\.\\d+)<");
    for (std::sregex_iterator it{ html.begin(), html.end(), re }, end{}; it != end; it++) {
        std::smatch result = *it;
        if (result.size() < 2)
        {
            std::cout << "[Utils] Failed to read latest revision\n";
            return;
        }
        latestVersion = result[1];
        latestVersionF = std::stof(latestVersion);
        break;
    }
    if (latestVersionF == -1.0f)
    {
        std::cout << "[Utils] Failed to fetch latest revision\n";
        return;
    }

    // show dialog
    if (installedVersionF < latestVersionF)
    {
        utils::osChoice choice = osYesNoMessageBox("Update available",
            "Currently installed version: " + installedVersion +
            "\nLatest version: " + latestVersion + "\nDo you want to update?");
        if (choice == utils::osChoice::Yes)
            std::thread* t = new std::thread(installUpdate, "https://github.com/santaclose/noose/releases/download/" + latestVersion + "/noose_windows_x64.zip");
    }
}

void utils::checkForUpdatesAsync()
{
    std::thread* t = new std::thread(checkForUpdates);
}

int utils::typeFromExtension(const std::string& filePath)
{
    if (endsWith(filePath, ".png") || endsWith(filePath, ".jpg") || endsWith(filePath, ".jpeg") || endsWith(filePath, ".bmp") || endsWith(filePath, ".tga") || endsWith(filePath, ".gif") || endsWith(filePath, ".psd") || endsWith(filePath, ".hdr") || endsWith(filePath, ".pic"))
        return NS_TYPE_IMAGE;
    if (endsWith(filePath, ".ttf") || endsWith(filePath, ".tte") || endsWith(filePath, ".otf") || endsWith(filePath, ".otc") || endsWith(filePath, ".ttc"))
        return NS_TYPE_FONT;
    return -1;
}