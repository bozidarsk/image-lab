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

Image::Image(unsigned int width, unsigned int height) : width(width), height(height) { pixels.resize(width * height); }
Image::Image(unsigned int width, unsigned int height, const std::vector<Color>& pixels) : width(width), height(height), pixels(pixels) {}
Image::Image(unsigned int width, unsigned int height, std::vector<Color>&& pixels) : width(width), height(height), pixels(std::move(pixels)) {}
Image::Image(unsigned int width, unsigned int height, std::initializer_list<Color> pixels) : width(width), height(height), pixels(pixels) {}
