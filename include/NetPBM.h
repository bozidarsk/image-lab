#pragma once

#include <string>
#include <fstream>
#include <expected>
#include <optional>

#include "Image.h"

class NetPBM : public Image
{
private:
	template<typename T>
	static std::optional<T> ReadNext(char type, std::ifstream& file) 
	{
		if (file.eof())
			return std::nullopt;

		T value;

		if ((type & 0b100) == 0) 
		{
			file >> std::ws;

			while (file.peek() == '#') 
			{
				for (char x = file.get(); x != '\r' && x != '\n' && x != EOF; x = file.get());
				file >> std::ws;
			}

			if (file.eof())
				return std::nullopt;

			file >> value;
		}
		else 
		{
			file.read((char*)(&value), sizeof(value));
		}

		return value;
	}

public:
	static std::expected<NetPBM, std::string> Load(const std::string& path);
	static void Save(const std::string& path, const Image& image);

	NetPBM(unsigned int width, unsigned int height);
	NetPBM(unsigned int width, unsigned int height, const std::vector<Color>& pixels);
	NetPBM(unsigned int width, unsigned int height, std::vector<Color>&& pixels);
	NetPBM(unsigned int width, unsigned int height, std::initializer_list<Color> pixels);
};
