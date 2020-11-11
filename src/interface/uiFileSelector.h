#pragma once

#include <string>

namespace uiFileSelector
{
	char* openFileDialog(const std::string& filter = "bmp,png,tga,jpg,gif,psd,hdr,pic");
	char* saveFileDialog(const std::string& fileExtension);
}