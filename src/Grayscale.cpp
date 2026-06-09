#include "Grayscale.h"

const std::string& Grayscale::GetName() const { static std::string name = "Grayscale"; return name; }

void Grayscale::Apply(Image& image) const
{
	for (Color& pixel : image.GetPixels())
	{
		float luminance = pixel.Luminance();
		pixel.r = luminance;
		pixel.g = luminance;
		pixel.b = luminance;
	}
}
