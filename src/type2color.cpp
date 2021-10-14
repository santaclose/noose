#include "type2color.h"
#include <unordered_map>
#include "types.h"

namespace type2color {

	std::unordered_map<int, int> mapping =
	{
		{NS_TYPE_INT, 0x4ca4fbff},
		{NS_TYPE_FLOAT, 0xee1133ff},
		{NS_TYPE_VECTOR2I, 0x56957aff},
		{NS_TYPE_COLOR, 0xaa7700ff},
		{NS_TYPE_IMAGE, 0x00bc44ff},
		{NS_TYPE_STRING, 0xbc007aff},
		{NS_TYPE_FONT, 0x7d7d7dff}
	};
}

const int type2color::get(int type)
{
	return mapping[type];
}
