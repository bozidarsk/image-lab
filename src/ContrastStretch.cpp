#include "ContrastStretch.h"

const std::string& ContrastStretch::GetName() const { static std::string name = "ContrastStretch"; return name; }

void ContrastStretch::Apply(Image& image) const
{
	Color min(1, 1, 1, 1), max(0, 0, 0, 0);

	for (const Color& pixel : image.GetPixels())
	{
		if (pixel < min)
			min = pixel;

		if (pixel > max)
			max = pixel;
	}

	for (Color& pixel : image.GetPixels())
	{
		pixel.r = (pixel.r - min.r) / (max.r - min.r);
		pixel.g = (pixel.g - min.g) / (max.g - min.g);
		pixel.b = (pixel.b - min.b) / (max.b - min.b);
	}
}
