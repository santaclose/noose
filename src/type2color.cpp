#include "type2color.h"
#include "types.h"

const int type2color::get(int type)
{
	switch (type)
	{
	case NS_TYPE_INT:
		return 0x4ca4fbff;
	case NS_TYPE_FLOAT:
		return 0xee1133ff;
	case NS_TYPE_VECTOR2I:
		return 0x56957aff;
	case NS_TYPE_COLOR:
		return 0xaa7700ff;
	case NS_TYPE_IMAGE:
		return 0x00bc44ff;
	case NS_TYPE_STRING:
		return 0xbc007aff;
	case NS_TYPE_FONT:
		return 0x7d7d7dff;
	}
}