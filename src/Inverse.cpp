#include "Inverse.h"

const std::string& Inverse::GetName() const { static std::string name = "Inverse"; return name; }

void Inverse::Apply(Image& image) const
{
	for (Color& pixel : image.GetPixels())
	{
		pixel.r = 1.0f - pixel.r;
		pixel.g = 1.0f - pixel.g;
		pixel.b = 1.0f - pixel.b;
	}
}
