#pragma once
#ifndef COLOR_ENUM_HPP
#define COLOR_ENUM_HPP

#include "EnumTraits.hpp"


enum class Color {
	RED = 0,
	GREEN,
	BLUE,
	MAX_ENUM_ID
};

DECLARE_ENUM_STRINGS_CONVERSION(Color, Color::MAX_ENUM_ID)


#endif // !COLOR_ENUM_HPP
