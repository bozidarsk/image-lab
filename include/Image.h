#pragma once

#include <span>
#include <vector>
#include <initializer_list>
#include <cassert>

#include "Color.h"

class Image
{
private:
	int width, height;
	std::vector<Color> pixels;

protected:
	Image& operator = (const Image&) = default;
	Image(const Image&) = default;
	Image& operator = (Image&&) = default;
	Image(Image&&) = default;

public:
	int GetWidth() const;
	int GetHeight() const;

	std::span<Color> GetPixels();
	const std::span<const Color> GetPixels() const;

	Color& operator [] (int index);
	const Color& operator [] (int index) const;

	Color& operator [] (int x, int y);
	const Color& operator [] (int x, int y) const;

	Image(int width, int height);
	Image(int width, int height, const std::vector<Color>& pixels);
	Image(int width, int height, std::vector<Color>&& pixels);
	Image(int width, int height, std::initializer_list<Color> pixels);

	virtual ~Image() = default;
};
