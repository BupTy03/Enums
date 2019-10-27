#include "ColorEnum.hpp"
#include "Direction.hpp"

#include <iostream>
#include <algorithm>
#include <string>


int main()
{
	for (auto&& enumItem : EnumView<Color>()) {
		std::cout << enumItem << std::endl;
	}

	auto it = std::find(EnumView<Color>::begin(), EnumView<Color>::end(), "Green");
	std::size_t index = std::distance(EnumView<Color>::begin(), it);

	bool success = false;
	if (static_cast<Color>(index) == StrToEnum<Color>("Green", &success) && success) {
		std::cout << "Success!!!" << std::endl;
	}

	std::for_each(EnumView<Direction>::begin(), EnumView<Direction>::end(), 
		[](auto&& enumStr) { std::cout << enumStr << std::endl; });

	system("pause");
	return 0;
}