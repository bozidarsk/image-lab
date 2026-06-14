#include <format>
#include <cstdint>

#include "NetPBM.h"

/*static*/ std::expected<NetPBM, std::string> NetPBM::Load(const std::string& path)
{
	std::ifstream file(path);

	if (!file.is_open())
		return std::unexpected(std::format("Cannot open file '{}'.", path));

	char magic[2];
	file.read(magic, sizeof(magic));

	if (magic[0] != 'P' || !(magic[1] >= '1' && magic[1] <= '6'))
		return std::unexpected("Invalid magic.");

	char type = magic[1];

	int width, height;
	std::vector<Color> pixels;

	if (type == '1' || type == '4')
	{
		if (TryReadNext<int>(type, file, &width));
		else return std::unexpected(std::format("Error parsing file '{}' at {}.", path, (int64_t)file.tellg()));

		if (TryReadNext<int>(type, file, &height));
		else return std::unexpected(std::format("Error parsing file '{}' at {}.", path, (int64_t)file.tellg()));

		int count = width * height;
		pixels.reserve(count);

		for (int i = 0; i < count; i++)
		{
			char x;

			if (TryReadNext<char>(type, file, &x)) x = (x == '0') ? 0xff : 0x00;
			else return std::unexpected(std::format("Error parsing file '{}' at {}.", path, (int64_t)file.tellg()));

			pixels.emplace_back(x / 255.0f, x / 255.0f, x / 255.0f);
		}
	}
	else if (type == '2' || type == '5')
	{
		if (TryReadNext<int>(type, file, &width));
		else return std::unexpected(std::format("Error parsing file '{}' at {}.", path, (int64_t)file.tellg()));

		if (TryReadNext<int>(type, file, &height));
		else return std::unexpected(std::format("Error parsing file '{}' at {}.", path, (int64_t)file.tellg()));

		int count = width * height;
		pixels.reserve(count);

		uint16_t max;

		if (TryReadNext<uint16_t>(type, file, &max));
		else return std::unexpected(std::format("Error parsing file '{}' at {}.", path, (int64_t)file.tellg()));

		for (int i = 0; i < count; i++)
		{
			uint16_t x;

			if (TryReadNext<uint16_t>(type, file, &x)) x = (uint16_t)(255.0f * ((float)x / (float)max));
			else return std::unexpected(std::format("Error parsing file '{}' at {}.", path, (int64_t)file.tellg()));

			pixels.emplace_back(x / 255.0f, x / 255.0f, x / 255.0f);
		}
	}
	else if (type == '3' || type == '6')
	{
		if (TryReadNext<int>(type, file, &width));
		else return std::unexpected(std::format("Error parsing file '{}' at {}.", path, (int64_t)file.tellg()));

		if (TryReadNext<int>(type, file, &height));
		else return std::unexpected(std::format("Error parsing file '{}' at {}.", path, (int64_t)file.tellg()));

		int count = width * height;
		pixels.reserve(count);

		uint16_t max;

		if (TryReadNext<uint16_t>(type, file, &max));
		else return std::unexpected(std::format("Error parsing file '{}' at {}.", path, (int64_t)file.tellg()));

		for (int i = 0; i < count; i++)
		{
			uint16_t r, g, b;

			if (TryReadNext<uint16_t>(type, file, &r)) r = (uint16_t)(255.0f * ((float)r / (float)max));
			else return std::unexpected(std::format("Error parsing file '{}' at {}.", path, (int64_t)file.tellg()));

			if (TryReadNext<uint16_t>(type, file, &g)) g = (uint16_t)(255.0f * ((float)g / (float)max));
			else return std::unexpected(std::format("Error parsing file '{}' at {}.", path, (int64_t)file.tellg()));

			if (TryReadNext<uint16_t>(type, file, &b)) b = (uint16_t)(255.0f * ((float)b / (float)max));
			else return std::unexpected(std::format("Error parsing file '{}' at {}.", path, (int64_t)file.tellg()));

			pixels.emplace_back(r / 255.0f, g / 255.0f, b / 255.0f);
		}
	}

	return NetPBM(width, height, pixels);
}

/*static*/ void NetPBM::Save(const std::string& path, const Image& image)
{
	std::ofstream file(path);

	if (!file.is_open())
		return;

	char type = '1';

	for (const Color& pixel : image.GetPixels())
	{
		if (type == '1' && (pixel.r != pixel.g || pixel.g != pixel.b || (pixel.r != 0.0f && pixel.r != 1.0f) || (pixel.g != 0.0f && pixel.g != 1.0f) || (pixel.b != 0.0f && pixel.b != 1.0f)))
			type = '2';

		if (type == '2' && (pixel.r != pixel.g || pixel.g != pixel.b))
		{
			type = '3';
			break;
		}
	}

	file << 'P' << type << '\n';
	file << image.GetWidth() << ' ' << image.GetHeight() << '\n';

	if (type == '1')
	{
		for (const Color& pixel : image.GetPixels())
			file << ((pixel.r == 1.0f) ? '0' : '1') << ' ';
	}
	else if (type == '2')
	{
		file << 0xff << '\n';

		for (const Color& pixel : image.GetPixels())
			file << (int)(pixel.r * 255.0f) << ' ';
	}
	else if (type == '3')
	{
		file << 0xff << '\n';

		for (const Color& pixel : image.GetPixels())
			file << (int)(pixel.r * 255.0f) << ' ' << (int)(pixel.g * 255.0f) << ' ' << (int)(pixel.b * 255.0f) << ' ';
	}

	file << '\n';
	file.close();
}

NetPBM::NetPBM(int width, int height) : Image(width, height) {}
NetPBM::NetPBM(int width, int height, const std::vector<Color>& pixels) : Image(width, height, pixels) {}
NetPBM::NetPBM(int width, int height, std::vector<Color>&& pixels) : Image(width, height, std::move(pixels)) {}
NetPBM::NetPBM(int width, int height, std::initializer_list<Color> pixels) : Image(width, height, pixels) {}
