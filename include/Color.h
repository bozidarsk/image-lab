#pragma once

#include <compare>
#include <cstdint>

struct Color
{
	uint8_t r, g, b, a;

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
	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff);
};
