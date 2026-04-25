#include "NetPBM.h"

/*static*/ std::expected<NetBPM, std::string> NetBPM::Load(const std::string& path) 
{
	std::ifstream file(path);

	if (!file.is_open())
		return std::unexpected(std::format("Cannot open file '{}'.", path));

	char magic[2];
	file.read(magic, sizeof(magic));

	if (magic[0] != 'P' || !(magic[1] >= '1' && magic[1] <= '6'))
		return std::unexpected("Invalid magic.");

	char type = magic[1];

	unsigned int width, height;
	std::vector<Color> pixels;

	if (type == '1' || type == '4') 
	{
		if (auto value = ReadNext<unsigned int>(type, file)) width = *value;
		else return std::unexpected(std::format("Error parsing file '{}' at {}.", path, (ssize_t)file.tellg()));

		if (auto value = ReadNext<unsigned int>(type, file)) height = *value;
		else return std::unexpected(std::format("Error parsing file '{}' at {}.", path, (ssize_t)file.tellg()));

		unsigned int count = width * height;
		pixels.reserve(count);

		for (unsigned int i = 0; i < count; i++) 
		{
			uint8_t x;

			if (auto value = ReadNext<char>(type, file)) x = (*value == '0') ? 0xff : 0x00;
			else return std::unexpected(std::format("Error parsing file '{}' at {}.", path, (ssize_t)file.tellg()));

			pixels.emplace_back(x, x, x);
		}
	}
	else if (type == '2' || type == '5') 
	{
		if (auto value = ReadNext<unsigned int>(type, file)) width = *value;
		else return std::unexpected(std::format("Error parsing file '{}' at {}.", path, (ssize_t)file.tellg()));

		if (auto value = ReadNext<unsigned int>(type, file)) height = *value;
		else return std::unexpected(std::format("Error parsing file '{}' at {}.", path, (ssize_t)file.tellg()));

		unsigned int count = width * height;
		pixels.reserve(count);

		uint16_t max;

		if (auto value = ReadNext<uint16_t>(type, file)) max = *value;
		else return std::unexpected(std::format("Error parsing file '{}' at {}.", path, (ssize_t)file.tellg()));

		for (unsigned int i = 0; i < count; i++)
		{
			uint8_t x;

			if (auto value = ReadNext<uint16_t>(type, file)) x = (uint8_t)(255.0 * ((float)(*value) / (float)max));
			else return std::unexpected(std::format("Error parsing file '{}' at {}.", path, (ssize_t)file.tellg()));

			pixels.emplace_back(x, x, x);
		}
	}
	else if (type == '3' || type == '6') 
	{
		if (auto value = ReadNext<unsigned int>(type, file)) width = *value;
		else return std::unexpected(std::format("Error parsing file '{}' at {}.", path, (ssize_t)file.tellg()));

		if (auto value = ReadNext<unsigned int>(type, file)) height = *value;
		else return std::unexpected(std::format("Error parsing file '{}' at {}.", path, (ssize_t)file.tellg()));

		unsigned int count = width * height;
		pixels.reserve(count);

		uint16_t max;

		if (auto value = ReadNext<uint16_t>(type, file)) max = *value;
		else return std::unexpected(std::format("Error parsing file '{}' at {}.", path, (ssize_t)file.tellg()));

		for (unsigned int i = 0; i < count; i++)
		{
			uint8_t r, g, b;

			if (auto value = ReadNext<uint16_t>(type, file)) r = (uint8_t)(255.0 * ((float)(*value) / (float)max));
			else return std::unexpected(std::format("Error parsing file '{}' at {}.", path, (ssize_t)file.tellg()));

			if (auto value = ReadNext<uint16_t>(type, file)) g = (uint8_t)(255.0 * ((float)(*value) / (float)max));
			else return std::unexpected(std::format("Error parsing file '{}' at {}.", path, (ssize_t)file.tellg()));

			if (auto value = ReadNext<uint16_t>(type, file)) b = (uint8_t)(255.0 * ((float)(*value) / (float)max));
			else return std::unexpected(std::format("Error parsing file '{}' at {}.", path, (ssize_t)file.tellg()));

			pixels.emplace_back(r, g, b);
		}
	}

	return NetBPM(width, height, pixels);
}

/*static*/ void NetBPM::Save(const std::string& path, const Image& image) 
{
	std::ofstream file(path);

	if (!file.is_open())
		return;

	char type = '1';

	for (const Color& pixel : image.GetPixels()) 
	{
		if (type == '1' && (pixel.r != pixel.g || pixel.g != pixel.b || (pixel.r != 0x00 && pixel.r != 0xff) || (pixel.g != 0x00 && pixel.g != 0xff) || (pixel.b != 0x00 && pixel.b != 0xff)))
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
			file << ((pixel.r == 0xff) ? '0' : '1') << ' ';
	}
	else if (type == '2') 
	{
		file << 0xff << '\n';

		for (const Color& pixel : image.GetPixels())
			file << (int)pixel.r << ' ';
	}
	else if (type == '3') 
	{
		file << 0xff << '\n';

		for (const Color& pixel : image.GetPixels())
			file << (int)pixel.r << ' ' << (int)pixel.g << ' ' << (int)pixel.b << ' ';
	}

	file << '\n';
	file.close();
}

NetBPM::NetBPM(unsigned int width, unsigned int height) : Image(width, height) {}
NetBPM::NetBPM(unsigned int width, unsigned int height, const std::vector<Color>& pixels) : Image(width, height, pixels) {}
NetBPM::NetBPM(unsigned int width, unsigned int height, std::vector<Color>&& pixels) : Image(width, height, std::move(pixels)) {}
NetBPM::NetBPM(unsigned int width, unsigned int height, std::initializer_list<Color> pixels) : Image(width, height, pixels) {}
