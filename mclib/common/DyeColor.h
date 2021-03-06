#ifndef MCLIB_COMMON_DYECOLOR_H_
#define MCLIB_COMMON_DYECOLOR_H_


#include <common/Vector.h>

enum DyeColor
{
	Black,
	Red,
	Green,
	Brown,
	Blue,
	Purple,
	Cyan,
	LightGray,
	Gray,
	Pink,
	Lime,
	Yellow,
	LightBlue,
	Magenta,
	Orange,
	White,
};

MCLIB_API glm::dvec3 GetColor(DyeColor dyeColor);
MCLIB_API std::string to_string(DyeColor dyeColor);
MCLIB_API std::wstring to_wstring(DyeColor dyeColor);

#endif
