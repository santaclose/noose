#pragma once
#include <iomanip>
#include <string>
#include <SFML/Graphics.hpp>

namespace utils
{
	int mod(int a, int b);
	char lower(char in);
	void rgb2hsv(const sf::Color& rgbColor, float& h, float& s, float& v);
	bool colorFromHexString(const std::string& hexString, sf::Color& outColor);
	std::string hexStringFromColor(const sf::Color& color);
	inline std::string vector2iToString(const sf::Vector2i& vector) { return std::to_string(vector.x) + ',' + std::to_string(vector.y); }
	bool vector2iFromString(const std::string& string, sf::Vector2i& outVector);

	bool imageFromBase64String(const std::string& base64String, sf::Image& out);
	bool base64StringFromImage(const sf::Image& image, std::string& out);

	bool drawImageToRenderTexture(const sf::Texture& image, sf::RenderTexture& renderTexture);
	bool drawImageToRenderTexture(const sf::Image& image, sf::RenderTexture& renderTexture);
	bool drawImageToRenderTexture(const std::string& imageFilePath, sf::RenderTexture& renderTexture);

	bool imageFromClipboard(sf::Image& outImage);
	bool imageToClipboard(const sf::Image& image);
	bool textFromClipboard(std::string& outString);
	bool textToClipboard(const std::string& string);

	template <typename T>
	std::string intToHex(T i)
	{
		std::stringstream stream;
		stream
			<< std::setfill ('0') << std::setw(sizeof(T)*2) 
			<< std::hex << i;
		return stream.str();
	}
	inline bool stringStartsWith(const std::string& string, const char* startsWith)
	{
		int i = 0;
		for (; i < string.length() && startsWith[i] != '\0' && startsWith[i] == string[i]; i++);
		return startsWith[i] == '\0';
	}

	// need drawQuads function since SFML removed Quad draw mode
	void drawQuads(sf::RenderTarget& renderTarget, const sf::Vertex* vertices, size_t vertexCount, const sf::RenderStates& renderStates = sf::RenderStates::Default);

	enum class osChoice { Cancel = -1, Ok, Yes, No, Abort, Retry, Ignore };
	void osShowNotification(const std::string& text);
	std::string osSaveFileDialog(const std::string& title, const std::string& extensionDropDownTitle, const std::string& extension);
	std::vector<std::string> osOpenFileDialog(const std::string& title, const std::string& extensionDropDownTitle, const std::string& extensions);
	osChoice osYesNoMessageBox(const std::string& title, const std::string& message);

	int pcall(const std::vector<std::string>& cmd);
	std::string pcheck_output(const std::vector<std::string>& cmd);

	void installUpdate(const std::string& updateUrl);
	void checkForUpdates();
	void checkForUpdatesAsync();

	int typeFromExtension(const std::string& filePath);

	inline bool endsWith(const std::string& str, const std::string& suffix)
	{
		return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
	}

	inline bool startsWith(const std::string& str, const std::string& prefix)
	{
		return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
	}
}