#include <print>
#include <string>
#include <vector>
#include <optional>
#include <iostream>
#include <algorithm>
#include <initializer_list>
#include <cstring>

#include "Shell.h"
#include "Image.h"
#include "Material.h"
#include "NetPBM.h"

template<typename T>
struct Resource 
{
	T resource;
	std::optional<std::string> alias = std::nullopt;
};

struct LoadedImage 
{
	std::string path;
	Resource<Image> image;
	std::vector<Resource<Material>> filters;
};

static std::vector<LoadedImage> loadedImages;

static bool TryParse(const std::string& str, int* result) 
{
	if (!result)
		return false;

	bool isNegative = str[0] == '-';
	*result = 0;

	for (int i = (isNegative || str[0] == '+') ? 1 : 0; i < str.length(); i++) 
	{
		if (str[i] < '0' || str[i] > '9')
			return false;

		*result *= 10;
		*result += str[i] - '0';
	}

	if (isNegative)
		*result = -(*result);

	return true;
}


static const Material* GetMaterialByName(const std::string&	name) 
{
	if (!strcasecmp(name.c_str(), "Inverse")) return &Material::Inverse;
	if (!strcasecmp(name.c_str(), "Grayscale")) return &Material::Grayscale;
	if (!strcasecmp(name.c_str(), "ContrastStretch")) return &Material::ContrastStretch;
	if (!strcasecmp(name.c_str(), "Blur")) return &Material::Blur;
	if (!strcasecmp(name.c_str(), "Sharpen")) return &Material::Sharpen;
	if (!strcasecmp(name.c_str(), "Emboss")) return &Material::Emboss;
	if (!strcasecmp(name.c_str(), "Outline")) return &Material::Outline;
	if (!strcasecmp(name.c_str(), "TopSobel")) return &Material::TopSobel;
	if (!strcasecmp(name.c_str(), "BottomSobel")) return &Material::BottomSobel;
	if (!strcasecmp(name.c_str(), "LeftSobel")) return &Material::LeftSobel;
	if (!strcasecmp(name.c_str(), "RightSobel")) return &Material::RightSobel;

	return nullptr;
}

static int Help(const ProgramArguments& args) 
{
	std::println("help - prints commands usage");
	std::println("pwd - prints the working directory");
	std::println("cd <dir> - changes the working directory");
	std::println("load <path> [as <alias>] - loads an image file with an optional name/alias");
	std::println("add-filter <image> <filter> [as <alias>] - adds a filter with an optional name/alias to be applied to the image");
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

		loadedImages.push_back({ .path = args[0], .image = { .resource = image.value() } });
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

		loadedImages.push_back({ .path = args[0], .image = { .resource = image.value(), .alias = args[2] } });
	}

	return 0;
}

static int AddFilter(const ProgramArguments& args) 
{
	// add-filter <image> <filter> [as <alias>]

	if (args.size() != 2 && args.size() != 4) 
	{
		std::println(stderr, "Invalid arguments.");
		return 1;
	}
	else if (args.size() == 2) 
	{
		auto loadedImage = std::find_if(loadedImages.begin(), loadedImages.end(), [&args](const LoadedImage& x) { return x.path == args[0] || x.image.alias == args[0]; });
		if (loadedImage == loadedImages.end()) 
		{
			std::println(stderr, "Cannot find the loaded image '{}'.", args[0]);
			return 2;
		}

		auto filter = GetMaterialByName(args[1]);
		if (!filter) 
		{
			std::println(stderr, "Cannot find filter '{}'.", args[1]);
			return 2;
		}

		loadedImage->filters.push_back({ .resource = *filter, .alias = args[1] });
	}
	else if (args.size() == 4) 
	{
		if (args[2] != "as") 
		{
			std::println(stderr, "Invalid arguments.");
			return 1;
		}

		auto loadedImage = std::find_if(loadedImages.begin(), loadedImages.end(), [&args](const LoadedImage& x) { return x.path == args[0] || x.image.alias == args[0]; });
		if (loadedImage == loadedImages.end()) 
		{
			std::println(stderr, "Cannot find the loaded image '{}'.", args[0]);
			return 2;
		}

		auto filter = GetMaterialByName(args[1]);
		if (!filter) 
		{
			std::println(stderr, "Cannot find filter '{}'.", args[1]);
			return 2;
		}

		loadedImage->filters.push_back({ .resource = *filter, .alias = args[3] });
	}

	return 0;
}

static int RemoveFilter(const ProgramArguments& args) 
{
	// remove-filter <image> {{<filter>|<filter-index>}}

	if (args.size() != 2) 
	{
		std::println(stderr, "Invalid arguments.");
		return 1;
	}

	auto loadedImage = std::find_if(loadedImages.begin(), loadedImages.end(), [&args](const LoadedImage& x) { return x.path == args[0] || x.image.alias == args[0]; });
	if (loadedImage == loadedImages.end()) 
	{
		std::println(stderr, "Cannot find the loaded image '{}'.", args[0]);
		return 2;
	}

	auto filter = std::find_if(loadedImage->filters.begin(), loadedImage->filters.end(), [&args](const Resource<Material>& x) { return x.alias == args[1]; });
	if (filter == loadedImage->filters.end()) 
	{
		int index;
		if (TryParse(args[1], &index)) 
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

	auto loadedImage = std::find_if(loadedImages.begin(), loadedImages.end(), [&args](const LoadedImage& x) { return x.path == args[0] || x.image.alias == args[0]; });
	if (loadedImage == loadedImages.end()) 
	{
		std::println(stderr, "Cannot find the loaded image '{}'.", args[0]);
		return 2;
	}

	std::print("Image '{}'", loadedImage->path);
	if (loadedImage->image.alias) std::print(" (as '{}')", loadedImage->image.alias.value());
	std::println(":");

	for (size_t i = 0; i < loadedImage->filters.size(); i++)
		std::println("[{}]: '{}'", i, loadedImage->filters[i].alias.value());

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

	auto loadedImage = std::find_if(loadedImages.begin(), loadedImages.end(), [&args](const LoadedImage& x) { return x.path == args[0] || x.image.alias == args[0]; });
	if (loadedImage == loadedImages.end()) 
	{
		std::println(stderr, "Cannot find the loaded image '{}'.", args[0]);
		return 2;
	}

	Image source(0, 0);
	Image destination = loadedImage->image.resource;

	std::print("Running filters for image '{}'", loadedImage->path);
	if (loadedImage->image.alias) std::print(" (as '{}')", loadedImage->image.alias.value());
	std::println(":");

	for (size_t i = 0; i < loadedImage->filters.size(); i++) 
	{
		std::println("Filter: [{}] '{}'", i, loadedImage->filters[i].alias.value());

		const Material& material = loadedImage->filters[i].resource;

		source = destination;
		source.ApplyMaterial(material, destination);

		std::println("Done");
	}

	loadedImage->image.resource = destination;

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

	auto loadedImage = std::find_if(loadedImages.begin(), loadedImages.end(), [&args](const LoadedImage& x) { return x.path == args[0] || x.image.alias == args[0]; });
	if (loadedImage == loadedImages.end()) 
	{
		std::println(stderr, "Cannot find the loaded image '{}'.", args[0]);
		return 2;
	}

	std::string path = (args.size() == 1) ? loadedImage->path : args[1];

	std::println("Saving image '{}'", args[0]);
	if (loadedImage->image.alias) std::print(" (as '{}')", loadedImage->image.alias.value());
	std::println(" to '{}'.", path);

	NetPBM::Save(path, loadedImage->image.resource);
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
		shell.Run(input);
	}
}
