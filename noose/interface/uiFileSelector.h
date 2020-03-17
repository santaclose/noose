#pragma once

#include <string>

namespace uiFileSelector
{
	char* openFileDialog();
	char* saveFileDialog(const std::string& fileExtension);
}