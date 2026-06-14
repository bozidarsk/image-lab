#include <filesystem>
#include <cstdint>
#include <cassert>
#include <memory>

#include "Image.h"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

int Image::GetWidth() const { return width; }
int Image::GetHeight() const { return height; }

void Image::Save(const std::string& path) const { Save(path.c_str()); }
void Image::Save(const char* path) const
{
	assert(path);

	int size = width * height;
	auto data = std::make_unique<uint32_t[]>(size);

	for (int i = 0; i < size; i++)
	{
		const Color& pixel = pixels[i];

		data[i] |= ((uint32_t)(pixel.r * 255.0f) & 0xff) << 0;
		data[i] |= ((uint32_t)(pixel.g * 255.0f) & 0xff) << 8;
		data[i] |= ((uint32_t)(pixel.b * 255.0f) & 0xff) << 16;
		data[i] |= ((uint32_t)(pixel.a * 255.0f) & 0xff) << 24;
	}

	stbi_write_png(path, width, height, 4, data.get(), 4 * width);
}

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

/* static */ std::expected<Image, std::string> Image::FromFile(const std::string& path)
{
	if (!std::filesystem::exists(path))
		return std::unexpected(std::format("File '{}' does not exist.", path));

	if (!std::filesystem::is_regular_file(path))
		return std::unexpected(std::format("File '{}' is not a file.", path));

	int width, height;

	const uint32_t* data = (const uint32_t*)stbi_load(path.c_str(), &width, &height, nullptr, 4);

	if (!data)
		return std::unexpected("Error parsing image.");

	Image image(width, height);

	int size = width * height;
	image.pixels.resize(size);

	for (int i = 0; i < size; i++)
	{
		Color& pixel = image.pixels[i];
		pixel.r = (float)((data[i] >> 0) & 0xff) / 255.0f;
		pixel.g = (float)((data[i] >> 8) & 0xff) / 255.0f;
		pixel.b = (float)((data[i] >> 16) & 0xff) / 255.0f;
		pixel.a = (float)((data[i] >> 24) & 0xff) / 255.0f;
	}

	stbi_image_free((void*)data);

	return image;
}

Image::Image(const std::string& path) : Image(path.c_str()) {}
Image::Image(const char* path)
{
	assert(path);
	assert(std::filesystem::exists(path));
	assert(std::filesystem::is_regular_file(path));

	const uint32_t* data = (const uint32_t*)stbi_load(path, &width, &height, nullptr, 4);
	assert(data);

	int size = width * height;
	pixels.resize(size);

	for (int i = 0; i < size; i++)
	{
		Color& pixel = pixels[i];
		pixel.r = (float)((data[i] >> 0) & 0xff) / 255.0f;
		pixel.g = (float)((data[i] >> 8) & 0xff) / 255.0f;
		pixel.b = (float)((data[i] >> 16) & 0xff) / 255.0f;
		pixel.a = (float)((data[i] >> 24) & 0xff) / 255.0f;
	}

	stbi_image_free((void*)data);
}

Image::Image(int width, int height) : width(width), height(height)
{
	assert(width > 0);
	assert(height > 0);

	pixels.resize(width * height);
}

Image::Image(int width, int height, const std::vector<Color>& pixels) : width(width), height(height), pixels(pixels)
{
	assert(width > 0);
	assert(height > 0);
	assert(width * height == this->pixels.size());
}

Image::Image(int width, int height, std::vector<Color>&& pixels) : width(width), height(height), pixels(std::move(pixels))
{
	assert(width > 0);
	assert(height > 0);
	assert(width * height == this->pixels.size());
}

Image::Image(int width, int height, std::initializer_list<Color> pixels) : width(width), height(height), pixels(pixels)
{
	assert(width > 0);
	assert(height > 0);
	assert(width * height == this->pixels.size());
}
