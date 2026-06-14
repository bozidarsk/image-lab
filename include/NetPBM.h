#pragma once

#include <string>
#include <fstream>
#include <expected>

#include "Image.h"

class NetPBM : public Image
{
private:
	template<typename T>
	static bool TryReadNext(char type, std::ifstream& file, T* value)
	{
		if (file.eof())
			return false;

		if ((type & 0b100) == 0)
		{
			file >> std::ws;

			while (file.peek() == '#')
			{
				for (char x = file.get(); x != '\r' && x != '\n' && x != EOF; x = file.get());
				file >> std::ws;
			}

			if (file.eof())
				return false;

			file >> *value;
		}
		else
		{
			file.read((char*)value, sizeof(T));
		}

		return true;
	}

public:
	static std::expected<NetPBM, std::string> Load(const std::string& path);
	static void Save(const std::string& path, const Image& image);

	NetPBM(int width, int height);
	NetPBM(int width, int height, const std::vector<Color>& pixels);
	NetPBM(int width, int height, std::vector<Color>&& pixels);
	NetPBM(int width, int height, std::initializer_list<Color> pixels);
};
