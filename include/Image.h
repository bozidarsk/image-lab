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

public:
	int GetWidth() const;
	int GetHeight() const;

	void Save(const char* path) const;
	void Save(const std::string& path) const;

	std::span<Color> GetPixels();
	const std::span<const Color> GetPixels() const;

	Color& operator [] (int index);
	const Color& operator [] (int index) const;

	Color& operator [] (int x, int y);
	const Color& operator [] (int x, int y) const;

	explicit Image(const char* path);
	explicit Image(const std::string& path);
	Image(int width, int height);
	Image(int width, int height, const std::vector<Color>& pixels);
	Image(int width, int height, std::vector<Color>&& pixels);
	Image(int width, int height, std::initializer_list<Color> pixels);
};
