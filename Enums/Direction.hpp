#pragma once
#ifndef DIRECTION_ENUM_HPP
#define DIRECTION_ENUM_HPP

#include "EnumTraits.hpp"


enum class Direction {
	UP = 0,
	DOWN,
	LEFT,
	RIGHT,
	MAX_ENUM_ID
};

DECLARE_ENUM_STRINGS_CONVERSION(Direction, Direction::MAX_ENUM_ID)


#endif // !DIRECTION_ENUM_HPP
