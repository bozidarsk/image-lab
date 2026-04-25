#pragma once

#include <type_traits>

template<typename TEnum> requires std::is_enum<TEnum>::value || std::is_scoped_enum<TEnum>::value
using UnderlyingType = std::underlying_type<TEnum>::type;


template<typename TEnum> requires std::is_enum<TEnum>::value || std::is_scoped_enum<TEnum>::value
constexpr TEnum operator ~ (const TEnum value) { return (TEnum)(~((UnderlyingType<TEnum>)value)); }

template<typename TEnum> requires std::is_enum<TEnum>::value || std::is_scoped_enum<TEnum>::value
constexpr TEnum operator ! (const TEnum value) { return (TEnum)(!((UnderlyingType<TEnum>)value)); }

template<typename TEnum> requires std::is_enum<TEnum>::value || std::is_scoped_enum<TEnum>::value
constexpr TEnum operator & (const TEnum left, const TEnum right) { return (TEnum)(((UnderlyingType<TEnum>)left) & ((UnderlyingType<TEnum>)right)); }

template<typename TEnum> requires std::is_enum<TEnum>::value || std::is_scoped_enum<TEnum>::value
constexpr TEnum operator | (const TEnum left, const TEnum right) { return (TEnum)(((UnderlyingType<TEnum>)left) | ((UnderlyingType<TEnum>)right)); }

template<typename TEnum> requires std::is_enum<TEnum>::value || std::is_scoped_enum<TEnum>::value
constexpr TEnum operator ^ (const TEnum left, const TEnum right) { return (TEnum)(((UnderlyingType<TEnum>)left) ^ ((UnderlyingType<TEnum>)right)); }

template<typename TEnum> requires std::is_enum<TEnum>::value || std::is_scoped_enum<TEnum>::value
constexpr TEnum& operator &= (TEnum& left, const TEnum right) { *(UnderlyingType<TEnum>*)(&left) &= (UnderlyingType<TEnum>)right; return left; }

template<typename TEnum> requires std::is_enum<TEnum>::value || std::is_scoped_enum<TEnum>::value
constexpr TEnum& operator |= (TEnum& left, const TEnum right) { *(UnderlyingType<TEnum>*)(&left) |= (UnderlyingType<TEnum>)right; return left; }

template<typename TEnum> requires std::is_enum<TEnum>::value || std::is_scoped_enum<TEnum>::value
constexpr TEnum& operator ^= (TEnum& left, const TEnum right) { *(UnderlyingType<TEnum>*)(&left) ^= (UnderlyingType<TEnum>)right; return left; }

template<typename TEnum> requires std::is_enum<TEnum>::value || std::is_scoped_enum<TEnum>::value
constexpr bool operator == (const TEnum left, const TEnum right) { return (UnderlyingType<TEnum>)left == (UnderlyingType<TEnum>)right; }

template<typename TEnum> requires std::is_enum<TEnum>::value || std::is_scoped_enum<TEnum>::value
constexpr bool operator != (const TEnum left, const TEnum right) { return (UnderlyingType<TEnum>)left != (UnderlyingType<TEnum>)right; }


template<typename TEnum> requires std::is_enum<TEnum>::value || std::is_scoped_enum<TEnum>::value
constexpr bool operator == (const TEnum left, UnderlyingType<TEnum> right) { return (UnderlyingType<TEnum>)left == (UnderlyingType<TEnum>)right; }

template<typename TEnum> requires std::is_enum<TEnum>::value || std::is_scoped_enum<TEnum>::value
constexpr bool operator == (UnderlyingType<TEnum> left, const TEnum right) { return (UnderlyingType<TEnum>)left == (UnderlyingType<TEnum>)right; }

template<typename TEnum> requires std::is_enum<TEnum>::value || std::is_scoped_enum<TEnum>::value
constexpr bool operator != (const TEnum left, UnderlyingType<TEnum> right) { return (UnderlyingType<TEnum>)left != (UnderlyingType<TEnum>)right; }

template<typename TEnum> requires std::is_enum<TEnum>::value || std::is_scoped_enum<TEnum>::value
constexpr bool operator != (UnderlyingType<TEnum> left, const TEnum right) { return (UnderlyingType<TEnum>)left != (UnderlyingType<TEnum>)right; }
