#include <print>
#include <array>
#include <string>
#include <vector>
#include <optional>
#include <iostream>
#include <algorithm>
#include <initializer_list>
#include <cmath>
#include <cstring>

#include "Shell.h"
#include "Image.h"
#include "Filter.h"
#include "NetPBM.h"

struct LoadedFilter
{
	std::unique_ptr<Filter> filter;
	std::optional<std::string> alias;

	LoadedFilter(std::unique_ptr<Filter>&& filter, const std::optional<std::string>& alias) : filter(std::move(filter)), alias(alias) {}
};

struct LoadedImage
{
	Image image;
	std::string path;
	std::optional<std::string> alias;
	std::vector<LoadedFilter> filters;

	LoadedImage(Image&& image, const std::string& path, const std::optional<std::string>& alias) : image(std::move(image)), path(path), alias(alias) {}
};

static std::vector<LoadedImage> loadedImages;

static bool TryParse(const char* str, int* result)
{
	if (!str || !result)
		return false;

	bool isNegative = *str == '-';

	if (*str == '+' || *str == '-')
		str++;

	if (*str == 0)
		return false;

	for (*result = 0; *str; str++)
	{
		if (*str < '0' || *str > '9')
			return false;

		*result *= 10;
		*result += *str - '0';
	}

	if (isNegative)
		*result = -(*result);

	return true;
}

static bool TryParse(const char* str, float* result)
{
	if (!str || !result)
		return false;

	std::string integerString = "";
	std::string fractionString = "";
	std::string exponentString = "";

	while (*str && *str != '.' && *str != 'e')
	{
		integerString += *str;
		str++;
	}

	if (*str == '.')
		for (str++; *str && *str != 'e'; str++)
			fractionString += *str;

	if (*str == 'e')
		for (str++; *str; str++)
			exponentString += *str;

	int integer, fraction, exponent;

	if (!integerString.empty() && !TryParse(integerString.c_str(), &integer)) return false;
	if (!fractionString.empty() && !TryParse(fractionString.c_str(), &fraction)) return false;
	if (!exponentString.empty() && !TryParse(exponentString.c_str(), &exponent)) return false;

	if (integerString.empty() && fractionString.empty())
		return  false;;

	*result = !integerString.empty() ? (float)integer : 0.0f;

	if (!fractionString.empty())
		*result += (float)fraction / std::pow(10.0f, fractionString.length());

	if (!exponentString.empty())
		*result *= std::pow(10.0f, (float)exponent);

	return true;
}

static bool TryParse(const char* str, Color* result)
{
	if (!str || !str)
		return false;

	if (!str[0])
		return false;

	if (str[0] == '#') str++;
	if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) str += 2;

	uint32_t color = 0;
	int length = 0;

	for (int i = 0; str[i]; i++)
	{
		color <<= 4;
		length++;

		if (str[i] >= '0' && str[i] <= '9') color |= str[i] - '0';
		else if (str[i] >= 'a' && str[i] <= 'f') color |= (str[i] - 'a') + 0xa;
		else if (str[i] >= 'A' && str[i] <= 'F') color |= (str[i] - 'A') + 0xa;
		else return false;
	}

	if (length != 6 && length != 8)
		return false;

	if (length == 6)
		color |= 0xff000000;

	*result = Color(color);
	return true;
}

static int Help(const ProgramArguments& args)
{
	std::println("help - prints commands usage");
	std::println("pwd - prints the working directory");
	std::println("cd [dir] - changes the working directory");
	std::println("ls [dir] - list directory contents");
	std::println("load <path> [as <alias>] - loads an image file with an optional name/alias");
	std::println("add-filter <image> <filter> [as <alias>] [filter-specific-args ...] - adds a filter with an optional name/alias to be applied to the image");
	std::println("remove-filter <image> {{<filter>|<filter-index>}} - removes the specified filter from an image");
	std::println("show-filters <image> - shows all filters that will be applied to the image");
	std::println("show-all-filters - shows all loaded images with all of their filters");
	std::println("run <image> - apply all filters of the image");
	std::println("run-all - apply all filters of all loaded images");
	std::println("save <image> [file] - saves the current state of the image into a file");
	std::println("quit - exits the program (all unsaved work will be lost)");

	return 0;
}

static int Load(const ProgramArguments& args)
{
	// load <path> [as <alias>]

	if (args.size() != 1 && args.size() != 3)
	{
		std::println(stderr, "Invalid arguments.");
		return 1;
	}
	else if (args.size() == 1)
	{
		auto image = NetPBM::Load(args[0]);
		if (!image)
		{
			std::println(stderr, "Failed to load image.");
			std::println(stderr, "{}", image.error());

			return 2;
		}

		loadedImages.emplace_back(std::move(image.value()), args[0], std::nullopt);
	}
	else if (args.size() == 3)
	{
		if (args[1] != "as")
		{
			std::println(stderr, "Invalid arguments.");
			return 1;
		}

		auto image = NetPBM::Load(args[0]);
		if (!image)
		{
			std::println(stderr, "Failed to load image.");
			std::println(stderr, "{}", image.error());

			return 2;
		}

		loadedImages.emplace_back(std::move(image.value()), args[0], args[2]);
	}

	return 0;
}

static int AddFilter(const ProgramArguments& args)
{
	// add-filter <image> <filter> [as <alias>] [filter-specific-args ...]

	if (args.size() < 2)
	{
		std::println(stderr, "Invalid arguments.");
		return 1;
	}

	auto loadedImage = std::find_if(loadedImages.begin(), loadedImages.end(), [&args](const LoadedImage& x) { return x.alias == args[0] || x.path == args[0]; });
	if (loadedImage == loadedImages.end())
	{
		std::println(stderr, "Cannot find the loaded image '{}'.", args[0]);
		return 2;
	}

	std::optional<std::string> alias = std::nullopt;
	if (args.size() >= 3 && args[2] == "as")
	{
		if (args.size() < 4)
		{
			std::println(stderr, "Invalid arguments.");
			return 1;
		}

		alias = args[3];
	}

	std::vector<std::any> arguments;

	for (size_t i = alias ? 4 : 2; i < args.size(); i++)
	{
		int _int;
		float _float;
		Color _Color;

		if (TryParse(args[i].c_str(), &_int)) arguments.push_back(_int);
		else if (TryParse(args[i].c_str(), &_float)) arguments.push_back(_float);
		else if (TryParse(args[i].c_str(), &_Color)) arguments.push_back(_Color);
		else
		{
			std::println(stderr, "Cannot parse argument '{}'.", args[i]);
			return 2;
		}
	}

	auto filter = Filter::Parse(args[1], arguments);
	if (!filter)
	{
		std::println(stderr, "Failed to load filter.", args[1]);
		std::println(stderr, "{}", filter.error());
		return 2;
	}

	loadedImage->filters.emplace_back(std::move(filter.value()), alias);

	return 0;
}

static int RemoveFilter(const ProgramArguments& args)
{
	// remove-filter <image> {<filter>|<filter-index>}

	if (args.size() != 2)
	{
		std::println(stderr, "Invalid arguments.");
		return 1;
	}

	auto loadedImage = std::find_if(loadedImages.begin(), loadedImages.end(), [&args](const LoadedImage& x) { return x.alias == args[0] || x.path == args[0]; });
	if (loadedImage == loadedImages.end())
	{
		std::println(stderr, "Cannot find the loaded image '{}'.", args[0]);
		return 2;
	}

	auto filter = std::find_if(loadedImage->filters.begin(), loadedImage->filters.end(), [&args](const LoadedFilter& x) { return x.alias == args[1] || x.filter->GetName() == args[1]; });
	if (filter == loadedImage->filters.end())
	{
		int index;
		if (TryParse(args[1].c_str(), &index))
		{
			if (index < 0)
				index = loadedImage->filters.size() + index;

			loadedImage->filters.erase(loadedImage->filters.begin() + index);
			return 0;
		}

		std::println(stderr, "Cannot find the applied filter '{}'.", args[1]);
		return 2;
	}

	loadedImage->filters.erase(filter);
	return 0;
}

static int ShowFilters(const ProgramArguments& args)
{
	// show-filters <image>

	if (args.size() != 1)
	{
		std::println(stderr, "Invalid arguments.");
		return 1;
	}

	auto loadedImage = std::find_if(loadedImages.begin(), loadedImages.end(), [&args](const LoadedImage& x) { return x.alias == args[0] || x.path == args[0]; });
	if (loadedImage == loadedImages.end())
	{
		std::println(stderr, "Cannot find the loaded image '{}'.", args[0]);
		return 2;
	}

	std::print("Image '{}'", loadedImage->path);
	if (loadedImage->alias) std::print(" (as '{}')", loadedImage->alias.value());
	std::println(":");

	for (size_t i = 0; i < loadedImage->filters.size(); i++)
	{
		std::print("[{}]: '{}'", i, loadedImage->filters[i].filter->GetName());
		if (loadedImage->filters[i].alias) std::print(" (as '{}')", loadedImage->filters[i].alias.value());

		std::println();
	}

	return 0;
}

static int ShowAllFilters(const ProgramArguments& args)
{
	// show-all-filters

	std::array<std::string, 1> newArgs;

	for (const LoadedImage& x : loadedImages)
	{
		newArgs[0] = x.path;
		ShowFilters(newArgs);
	}

	return 0;
}

static int Run(const ProgramArguments& args)
{
	// run <image>

	if (args.size() != 1)
	{
		std::println(stderr, "Invalid arguments.");
		return 1;
	}

	auto loadedImage = std::find_if(loadedImages.begin(), loadedImages.end(), [&args](const LoadedImage& x) { return x.alias == args[0] || x.path == args[0]; });
	if (loadedImage == loadedImages.end())
	{
		std::println(stderr, "Cannot find the loaded image '{}'.", args[0]);
		return 2;
	}

	std::print("Running filters for image '{}'", loadedImage->path);
	if (loadedImage->alias) std::print(" (as '{}')", loadedImage->alias.value());
	std::println(":");

	for (size_t i = 0; i < loadedImage->filters.size(); i++)
	{
		std::print("Filter: ");
		std::print("[{}]: '{}'", i, loadedImage->filters[i].filter->GetName());
		if (loadedImage->filters[i].alias) std::print(" (as '{}')", loadedImage->filters[i].alias.value());
		std::println();

		for (const auto& filter : loadedImage->filters)
			filter.filter->Apply(loadedImage->image);

		std::println("Done");
	}

	return 0;
 }

static int RunAll(const ProgramArguments& args)
{
	// run-all

	std::array<std::string, 1> newArgs;

	for (const LoadedImage& x : loadedImages)
	{
		newArgs[0] = x.path;
		Run(newArgs);
	}

	return 0;
}

static int Save(const ProgramArguments& args)
{
	// save <image> [file]

	if (args.size() != 1 && args.size() != 2)
	{
		std::println(stderr, "Invalid arguments.");
		return 1;
	}

	auto loadedImage = std::find_if(loadedImages.begin(), loadedImages.end(), [&args](const LoadedImage& x) { return x.alias == args[0] || x.path == args[0]; });
	if (loadedImage == loadedImages.end())
	{
		std::println(stderr, "Cannot find the loaded image '{}'.", args[0]);
		return 2;
	}

	std::string path = (args.size() == 1) ? loadedImage->path : args[1];

	std::print("Saving image '{}'", args[0]);
	if (loadedImage->alias) std::print(" (as '{}')", loadedImage->alias.value());
	std::println(" to '{}'.", path);

	NetPBM::Save(path, loadedImage->image);
	std::println("Done.");

	return 0;
}

static int Quit(const ProgramArguments& args)
{
	exit(0);
}

int main(int argc, char** argv)
{
	auto programs =
	{
		Program::PrintWorkingDirectory,
		Program::ChangeWorkingDirectory,
		Program::ListDirectoryContents,
		Program("help", &Help),
		Program("load", &Load),
		Program("add-filter", &AddFilter),
		Program("remove-filter", &RemoveFilter),
		Program("show-filters", &ShowFilters),
		Program("show-all-filters", &ShowAllFilters),
		Program("run", &Run),
		Program("run-all", &RunAll),
		Program("save", &Save),
		Program("quit", &Quit),
	};

	Shell shell = programs;
	std::string prompt = "> ";
	std::string input;

	while (true)
	{
		std::print("{}", prompt);
		std::getline(std::cin, input);

		auto code = shell.Run(input);
		if (!code)
		{
			std::println(stderr, "Failed to run program.");
			std::println(stderr, "{}", code.error());
		}
	}
}
