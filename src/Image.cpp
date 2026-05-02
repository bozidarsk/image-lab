#include <cassert>

#include "Image.h"
#include "Properties.h"

unsigned int Image::GetWidth() const { return width; }
unsigned int Image::GetHeight() const { return height; }

std::span<Color> Image::GetPixels() { return pixels; }
const std::span<const Color> Image::GetPixels() const { return pixels; }

Color& Image::operator [] (int index)
{
	assert(index >= 0 && index < (width * height));

	return pixels[index];
}

const Color& Image::operator [] (int index) const
{
	assert(index >= 0 && index < (width * height));

	return pixels[index];
}

Color& Image::operator [] (int x, int y)
{
	assert(x >= 0 && x < width);
	assert(y >= 0 && y < height);

	return pixels[y * width + x];
}

const Color& Image::operator [] (int x, int y) const
{
	assert(x >= 0 && x < width);
	assert(y >= 0 && y < height);

	return pixels[y * width + x];
}

void Image::ApplyMaterial(const Material& material)
{
	ApplyMaterial(material, *this);
}

void Image::ApplyMaterial(const Material& material, Image& destination) const
{
	const Uniforms& uniforms = material.GetUniforms();
	Properties input, output;

	for (const Shader& shader : material.GetShaders())
	{
		input.Set<const Image*>("image", this);

		for (int y = 0; y < height; y++)
		{
			input.Set<int>("y", y);

			for (int x = 0; x < width; x++)
			{
				input.Set<Color>("color", pixels[y * width + x]);
				input.Set<int>("x", x);

				shader(input, output, uniforms);
				destination[y * width + x] = output.Has<Color>("color") ? output.Get<Color>("color") : Color(0, 0, 0);
			}
		}

		input = output;
	}
}

/*static*/ Image Image::ApplyMaterials(const Image& image, std::initializer_list<const Material> materials)
{
	Image source(0, 0);
	Image destination = image;

	for (const Material& material : materials)
	{
		source = destination;
		source.ApplyMaterial(material, destination);
	}

	return destination;
}

Image::Image(unsigned int width, unsigned int height) : width(width), height(height) { pixels.resize(width * height); }
Image::Image(unsigned int width, unsigned int height, const std::vector<Color>& pixels) : width(width), height(height), pixels(pixels) {}
Image::Image(unsigned int width, unsigned int height, std::vector<Color>&& pixels) : width(width), height(height), pixels(std::move(pixels)) {}
Image::Image(unsigned int width, unsigned int height, std::initializer_list<Color> pixels) : width(width), height(height), pixels(pixels) {}
