#pragma once

#include <span>
#include <string>
#include <vector>
#include <initializer_list>
#include <cstdint>
#include <cassert>

#include "Color.h"
#include "Material.h"

class Image 
{
private:
	unsigned int width, height;
	std::vector<Color> pixels;

public:
	unsigned int GetWidth() const;
	unsigned int GetHeight() const;

	std::span<Color> GetPixels();
	const std::span<const Color> GetPixels() const;

	Color& operator [] (int index);
	const Color& operator [] (int index) const;

	Color& operator [] (int x, int y);
	const Color& operator [] (int x, int y) const;

	void ApplyMaterial(const Material& material);
	void ApplyMaterial(const Material& material, Image& destination) const;

	static Image ApplyMaterials(const Image& source, std::initializer_list<const Material> materials);

	Image(unsigned int width, unsigned int height);
	Image(unsigned int width, unsigned int height, const std::vector<Color>& pixels);
	Image(unsigned int width, unsigned int height, std::vector<Color>&& pixels);
	Image(unsigned int width, unsigned int height, std::initializer_list<Color> pixels);
};
