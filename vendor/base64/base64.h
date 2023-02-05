#pragma once

#include <string>

namespace base64
{
	std::string encode(unsigned char const* bytes_to_encode, unsigned int in_len);
	std::string decode(std::string const& encoded_string);
}