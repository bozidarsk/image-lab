#pragma once

#include <string>
#include <compare>
#include <expected>
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

	static std::expected<Color, std::string> Parse(const std::string& str);
	static bool TryParse(const std::string& str, Color* result);

	void Clamp();
	float Luminance() const;

	Color& operator *= (float x);
	Color& operator /= (float x);
	Color& operator += (const Color& x);
	Color& operator -= (const Color& x);
	Color operator + (const Color& x) const;
	Color operator - (const Color& x) const;

	explicit operator uint32_t () const;

	auto operator <=> (const Color& other) const = default;

	Color();
	Color(const Color& x, float a);
	Color(float r, float g, float b, float a = 1.0f);
	explicit Color(uint32_t argb);
};

Color operator * (const Color& color, float x);
Color operator * (float x, const Color& color);
Color operator / (const Color& color, float x);
Color operator / (float x, const Color& color);
