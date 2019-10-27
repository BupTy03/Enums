#include "Direction.hpp"


DEFINE_ENUM_STRINGS_CONVERSION(Direction, Direction::MAX_ENUM_ID,
	std::string("Up"),
	std::string("Down"),
	std::string("Left"),
	std::string("Right"))