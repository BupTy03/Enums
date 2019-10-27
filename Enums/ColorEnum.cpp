#include "ColorEnum.hpp"


DEFINE_ENUM_STRINGS_CONVERSION(Color, Color::MAX_ENUM_ID,
	std::string("Red"),
	std::string("Green"),
	std::string("Blue"))

