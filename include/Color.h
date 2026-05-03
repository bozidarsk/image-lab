#pragma once

#include <compare>
#include <cstdint>

struct Color
{
	float r, g, b, a;

	static const Color Transparent;
	static const Color Black;
	static const Color White;
	static const Color Gray;
	static const Color Red;
	static const Color Green;
	static const Color Blue;
	static const Color Yellow;
	static const Color Cyan;
	static const Color Magenta;

	auto operator <=> (const Color& other) const = default;

	Color();
	Color(float r, float g, float b, float a = 1.0);
	Color(uint32_t argb);
};
