#include <algorithm>

#include "Color.h"

/*static*/ const Color Color::Transparent = Color(0x00000000);
/*static*/ const Color Color::Black       = Color(0xff000000);
/*static*/ const Color Color::White       = Color(0xffffffff);
/*static*/ const Color Color::Gray        = Color(0xff7f7f7f);
/*static*/ const Color Color::Red         = Color(0xff0000ff);
/*static*/ const Color Color::Green       = Color(0xff00ff00);
/*static*/ const Color Color::Blue        = Color(0xffff0000);
/*static*/ const Color Color::Yellow      = Color(0xff00ffff);
/*static*/ const Color Color::Cyan        = Color(0xffffff00);
/*static*/ const Color Color::Magenta     = Color(0xffff00ff);

void Color::Clamp()
{
	r = std::clamp(r, 0.0f, 1.0f);
	g = std::clamp(g, 0.0f, 1.0f);
	b = std::clamp(b, 0.0f, 1.0f);
	a = std::clamp(a, 0.0f, 1.0f);
}

float Color::Luminance() const { return 0.2125f*r + 0.7154f*g + 0.0721f*b; }

Color& Color::operator *= (float x) { r *= x; g *= x; b *= x; a *= x; return *this; }
Color& Color::operator /= (float x) { r /= x; g /= x; b /= x; a /= x; return *this; }
Color& Color::operator += (const Color& x) { r += x.r; g += x.g; b += x.g; a += x.a; return *this; }
Color& Color::operator -= (const Color& x) { r -= x.r; g -= x.g; b -= x.g; a -= x.a; return *this; }
Color Color::operator + (const Color& x) const { return Color(r + x.r, g + x.g, b + x.g, a + x.a); }
Color Color::operator - (const Color& x) const { return Color(r - x.r, g - x.g, b - x.g, a - x.a); }

Color operator * (const Color& color, float x) { return Color(color.r * x, color.g * x, color.b * x, color.a * x); }
Color operator * (float x, const Color& color) { return Color(color.r * x, color.g * x, color.b * x, color.a * x); }
Color operator / (const Color& color, float x) { return Color(color.r / x, color.g / x, color.b / x, color.a / x); }
Color operator / (float x, const Color& color) { return Color(color.r / x, color.g / x, color.b / x, color.a / x); }

Color::operator uint32_t () const { return (((uint32_t)(a * 255.0f) & 0xff) << 24) | (((uint32_t)(b * 255.0f) & 0xff) << 16) | (((uint32_t)(g * 255.0f) & 0xff) << 8) | (((uint32_t)(r * 255.0f) & 0xff) << 0); }

Color::Color() : Color(0.0f, 0.0f, 0.0f, 0.0f) {}
Color::Color(const Color& x, float a) : Color(x.r, x.g, x.b, a) {}
Color::Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
Color::Color(uint32_t abgr) : r(((abgr >> 0) & 0xff) / 255.0f), g(((abgr >> 8) & 0xff) / 255.0f), b(((abgr >> 16) & 0xff) / 255.0f), a(((abgr >> 24) & 0xff) / 255.0f) {}
