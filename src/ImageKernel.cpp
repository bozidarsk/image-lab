#include <cassert>
#include <cstring>

#include "ImageKernel.h"

const std::string& ImageKernel::GetName() const { static std::string name = "ImageKernel"; return name; }

void ImageKernel::Apply(Image& image) const
{
	for (unsigned int y = 0; y < image.GetHeight(); y++)
	{
		for (unsigned int x = 0; x < image.GetWidth(); x++)
		{
			if (x == 0 || y == 0 || x == image.GetWidth() - 1 || y == image.GetHeight() - 1)
			{
				image[x, y] = Color::Black;
				continue;
			}

			Color color(0, 0, 0, 0);

			color += matrix[0] * image[x - 1, y - 1];
			color += matrix[1] * image[x, y - 1];
			color += matrix[2] * image[x + 1, y - 1];

			color += matrix[3] * image[x - 1, y];
			color += matrix[4] * image[x, y];
			color += matrix[5] * image[x + 1, y];

			color += matrix[6] * image[x - 1, y + 1];
			color += matrix[7] * image[x, y + 1];
			color += matrix[8] * image[x + 1, y + 1];

			image[x, y] = color;
		}
	}
}

ImageKernel::ImageKernel(const float matrix[9])
{
	assert(matrix);
	memcpy(this->matrix, matrix, sizeof(this->matrix));
}
