#pragma once

#include <cstdint>

struct Color 
{
	uint8_t r, g, b, a;

	auto operator <=> (const Color& other) const = default;

	Color();
	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff);
};
