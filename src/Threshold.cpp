#include "Threshold.h"

const std::string& Threshold::GetName() const { static std::string name = "Threshold"; return name; }

Threshold::Threshold(Color threshold) : threshold(threshold) {}

void Threshold::Apply(Image& image) const
{
	for (Color& pixel : image.GetPixels())
	{
		pixel = (pixel >= threshold) ? Color::White : Color::Black;
	}
}
