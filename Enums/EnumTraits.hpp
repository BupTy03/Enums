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


template<class Comp = std::less<void>>
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


template<typename EnumType, typename = std::enable_if_t<std::is_enum_v<EnumType>>>
struct EnumTypeTag {};

using EnumStringType = std::string;

template<typename EnumType, typename = std::enable_if_t<std::is_enum_v<EnumType>>>
EnumType StrToEnum(const EnumStringType& str, bool* success) { return StrToEnum(str, success, EnumTypeTag<EnumType>()); }

template<typename EnumType, typename = std::enable_if_t<std::is_enum_v<EnumType>>>
struct EnumView;


#define DECLARE_ENUM_STRINGS_CONVERSION(EnumName, MaxEnumID) \
	EnumStringType EnumToStr(EnumName value); \
	EnumName StrToEnum(const EnumStringType& str, bool* success, EnumTypeTag<EnumName>); \
	template<> struct EnumView<EnumName> { \
		static std::array<EnumStringType, static_cast<std::size_t>(MaxEnumID)>::const_iterator begin(); \
		static std::array<EnumStringType, static_cast<std::size_t>(MaxEnumID)>::const_iterator end(); \
	};


#define DEFINE_ENUM_STRINGS_CONVERSION(EnumName, MaxEnumID, ...) \
																 \
namespace EnumName##__EnumStringsConversion__impl__ { \
	static const std::array<EnumStringType, static_cast<std::size_t>(MaxEnumID)> enumStrings = { \
		__VA_ARGS__ \
	}; \
	   \
	static std::vector<std::pair<EnumStringType, std::size_t>> initEnumStringsToIndexes() { \
		std::vector<std::pair<EnumStringType, std::size_t>> result; \
		result.reserve(enumStrings.size()); \
		for (std::size_t index = 0; index < enumStrings.size(); ++index) { \
			result.emplace_back(enumStrings.at(index), index); \
		} \
		std::sort(std::begin(result), std::end(result), CompareFirstAdapter<>()); \
		return result; \
	} \
} \
	\
	EnumStringType EnumToStr(EnumName value) \
	{ \
		const auto index = static_cast<int>(value); \
		assert(index >= 0); \
		assert(index < static_cast<int>(MaxEnumID)); \
												 \
		return EnumName##__EnumStringsConversion__impl__::enumStrings.at(static_cast<std::size_t>(index)); \
	} \
	  \
	EnumName StrToEnum(const EnumStringType& str, bool* success, EnumTypeTag<EnumName>) { \
		assert(success != nullptr); \
									\
		static const std::vector<std::pair<EnumStringType, std::size_t>> stringsToIndexes = \
			EnumName##__EnumStringsConversion__impl__::initEnumStringsToIndexes(); \
																	 \
		const auto it = std::lower_bound(std::cbegin(stringsToIndexes), std::cend(stringsToIndexes), str, CompareFirstAdapter<>()); \
		if (it == std::cend(stringsToIndexes) || it->first != str) { \
			*success = false; \
			return static_cast<EnumName>(0); \
		} \
		  \
		*success = true; \
		return static_cast<EnumName>(it->second); \
	} \
	  \
	  \
	std::array<EnumStringType, static_cast<std::size_t>(MaxEnumID)>::const_iterator EnumView<EnumName>::begin() { \
		return std::cbegin(EnumName##__EnumStringsConversion__impl__::enumStrings); \
	} \
	std::array<EnumStringType, static_cast<std::size_t>(MaxEnumID)>::const_iterator EnumView<EnumName>::end() { \
		return std::cend(EnumName##__EnumStringsConversion__impl__::enumStrings); \
	}


#endif // !ENUM_TRAITS
