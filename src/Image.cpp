#include <cassert>

#include "Image.h"

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
	auto& shader = material.GetShader();
	auto& uniforms = material.GetUniforms();
	auto shaderEntryPoint = shader.GetEntryPoint();

	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			pixels[y * width + x] = shaderEntryPoint(x, y, *this, uniforms);
}

void Image::ApplyMaterial(const Material& material, Image& destination) const 
{
	auto& shader = material.GetShader();
	auto& uniforms = material.GetUniforms();
	auto shaderEntryPoint = shader.GetEntryPoint();

	assert(width == destination.width);
	assert(height == destination.height);

	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			destination.pixels[y * width + x] = shaderEntryPoint(x, y, *this, uniforms);
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
