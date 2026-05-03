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

Color::Color() : Color(0, 0, 0, 0) {}
Color::Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
Color::Color(uint32_t abgr) : r(((abgr >> 0) & 0xff) / 255.0), g(((abgr >> 8) & 0xff) / 255.0), b(((abgr >> 16) & 0xff) / 255.0), a(((abgr >> 24) & 0xff) / 255.0) {}
