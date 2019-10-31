#pragma once
#ifndef ENUM_TRAITS
#define ENUM_TRAITS

#include <vector>
#include <string>
#include <array>
#include <type_traits>
#include <algorithm>
#include <cassert>
#include <utility>
#include <functional>


template<class Comp = std::less<>>
struct CompareFirstAdapter {
	explicit CompareFirstAdapter() = default;

	template<class... Args>
	explicit CompareFirstAdapter(Args&&... args) : comp_{ std::forward<Args>(args)... } {}

	template<class First>
	bool operator()(const First& left, const First& right) const { return comp_(left, right); }

	template<class First, class Second>
	bool operator()(const std::pair<First, Second>& left, const std::pair<First, Second>& right) const { return this->operator()(left.first, right.first); }

	template<class First, class Second>
	bool operator()(const std::pair<First, Second>& left, const First& right) const { return this->operator()(left.first, right); }

	template<class First, class Second>
	bool operator()(const First& left, const std::pair<First, Second>& right) const { return this->operator()(left, right.first); }

private:
	Comp comp_;
};


namespace enumstrings_impl {

	using EnumStringType = std::string;

	template<typename EnumType, typename = std::enable_if_t<std::is_enum_v<EnumType>>>
	struct EnumTypeTag {};

}


template<typename EnumType, typename = std::enable_if_t<std::is_enum_v<EnumType>>>
EnumType StrToEnum(const enumstrings_impl::EnumStringType& str, bool* success) { return StrToEnum(str, success, enumstrings_impl::EnumTypeTag<EnumType>()); }


template<typename EnumType, typename = std::enable_if_t<std::is_enum_v<EnumType>>>
struct EnumView;


template<typename EnumType, typename = std::enable_if_t<std::is_enum_v<EnumType>>>
std::vector<std::string> MakeEnumStringList()
{
	return std::vector<std::string>(EnumView<EnumType>::begin(), EnumView<EnumType>::end());
}


namespace enumstrings_impl {

	template<typename ArrayStringsType>
	std::vector<std::pair<EnumStringType, std::size_t>> MakeEnumStringsToIndexes(const ArrayStringsType& arrStrings)
	{
		std::vector<std::pair<EnumStringType, std::size_t>> result;
		result.reserve(arrStrings.size());
		for (std::size_t index = 0; index < arrStrings.size(); ++index) {
			result.emplace_back(arrStrings.at(index), index);
		}
		std::sort(std::begin(result), std::end(result), CompareFirstAdapter<>());
		return result;
	}

	template<typename EnumType, typename = std::enable_if_t<std::is_enum_v<EnumType>>>
	EnumType EnumMemberByString(
		const std::vector<std::pair<enumstrings_impl::EnumStringType, std::size_t>>& stringsToEnumMembers,
		const enumstrings_impl::EnumStringType& memberStr,
		bool* success)
	{
		assert(success != nullptr);
		assert(std::is_sorted(std::cbegin(stringsToEnumMembers), std::cend(stringsToEnumMembers)));

		const auto it = std::lower_bound(std::cbegin(stringsToEnumMembers), std::cend(stringsToEnumMembers), memberStr, CompareFirstAdapter<>());
		if (it == std::cend(stringsToEnumMembers) || it->first != memberStr) {
			*success = false;
			return EnumType();
		}
		\
			*success = true;
		return static_cast<EnumType>(it->second);
	}
}


#define DECLARE_ENUM_STRINGS_CONVERSION(EnumName, MaxEnumID) \
    static_assert(std::is_enum_v<EnumName>, "EnumName must be enum type!"); \
    static_assert(std::is_integral_v<decltype(MaxEnumID)> || std::is_enum_v<decltype(MaxEnumID)>, "MaxEnumID must be of integral type or enum type"); \
    static_assert (static_cast<std::int64_t>(MaxEnumID) > 0, "MaxEnumID must be greater than zero!"); \
                                              \
    enumstrings_impl::EnumStringType EnumToStr(EnumName value); \
    EnumName StrToEnum(const enumstrings_impl::EnumStringType& str, bool* success, enumstrings_impl::EnumTypeTag<EnumName>); \
    template<> struct EnumView<EnumName> { \
        static std::array<enumstrings_impl::EnumStringType, static_cast<std::size_t>(MaxEnumID)>::const_iterator begin(); \
        static std::array<enumstrings_impl::EnumStringType, static_cast<std::size_t>(MaxEnumID)>::const_iterator end(); \
    };

  

#define DEFINE_ENUM_STRINGS_CONVERSION(EnumName, MaxEnumID, ...) \
    static_assert(std::is_enum_v<EnumName>, "EnumName must be enum type!"); \
    static_assert(std::is_integral_v<decltype(MaxEnumID)> || std::is_enum_v<decltype(MaxEnumID)>, "MaxEnumID must be of integral type or enum type"); \
    static_assert (static_cast<std::int64_t>(MaxEnumID) > 0, "MaxEnumID must be greater than zero!"); \
                                                      \
namespace EnumName##__EnumStringsConversion__impl__ { \
    static const std::array<enumstrings_impl::EnumStringType, static_cast<std::size_t>(MaxEnumID)> enumStrings = { \
        __VA_ARGS__ \
    }; \
} \
    \
    enumstrings_impl::EnumStringType EnumToStr(EnumName value) \
    { \
        const auto index = static_cast<int>(value); \
        assert(index >= 0); \
        assert(index < static_cast<int>(MaxEnumID)); \
                                                 \
        return EnumName##__EnumStringsConversion__impl__::enumStrings.at(static_cast<std::size_t>(index)); \
    } \
      \
    EnumName StrToEnum(const enumstrings_impl::EnumStringType& str, bool* success, enumstrings_impl::EnumTypeTag<EnumName>) { \
        assert(success != nullptr); \
                                    \
        static const std::vector<std::pair<enumstrings_impl::EnumStringType, std::size_t>> stringsToIndexes = \
            enumstrings_impl::MakeEnumStringsToIndexes(EnumName##__EnumStringsConversion__impl__::enumStrings); \
                                                                     \
        return enumstrings_impl::EnumMemberByString<EnumName>(stringsToIndexes, str, success); \
    } \
      \
      \
    std::array<enumstrings_impl::EnumStringType, static_cast<std::size_t>(MaxEnumID)>::const_iterator EnumView<EnumName>::begin() { \
        return std::cbegin(EnumName##__EnumStringsConversion__impl__::enumStrings); \
    } \
    std::array<enumstrings_impl::EnumStringType, static_cast<std::size_t>(MaxEnumID)>::const_iterator EnumView<EnumName>::end() { \
        return std::cend(EnumName##__EnumStringsConversion__impl__::enumStrings); \
    }


#endif // !ENUM_TRAITS
