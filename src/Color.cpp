#include "Color.h"

/*static*/ const Color Color::Transparent = Color(0x00, 0x00, 0x00, 0x00);
/*static*/ const Color Color::Black       = Color(0x00, 0x00, 0x00, 0xff);
/*static*/ const Color Color::White       = Color(0xff, 0xff, 0xff, 0xff);
/*static*/ const Color Color::Gray        = Color(0x7f, 0x7f, 0x7f, 0xff);
/*static*/ const Color Color::Red         = Color(0xff, 0x00, 0x00, 0xff);
/*static*/ const Color Color::Green       = Color(0x00, 0xff, 0x00, 0xff);
/*static*/ const Color Color::Blue        = Color(0x00, 0x00, 0xff, 0xff);
/*static*/ const Color Color::Yellow      = Color(0xff, 0xff, 0x00, 0xff);
/*static*/ const Color Color::Cyan        = Color(0x00, 0xff, 0xff, 0xff);
/*static*/ const Color Color::Magenta     = Color(0xff, 0x00, 0xff, 0xff);

Color::Color() : Color(0, 0, 0, 0) {}
Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}
