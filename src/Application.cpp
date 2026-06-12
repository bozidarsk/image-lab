#include <print>
#include <array>
#include <string>
#include <iostream>
#include <filesystem>

#include "Shell.h"
#include "NetPBM.h"
#include "Program.h"
#include "Application.h"

auto Application::FindImage(const std::string& id)
{
	return std::find_if(loadedImages.begin(), loadedImages.end(), [&id](const LoadedImage& x) { return x.alias == id || x.path == id; });
}

auto Application::LoadedImage::FindFilter(const std::string& id)
{
	return std::find_if(filters.begin(), filters.end(), [&id](const LoadedFilter& x) { return x.alias == id || x.filter->GetName() == id; });;
}

int Application::Help(const ProgramArguments& args)
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

int Application::Load(const ProgramArguments& args)
{
	// load <path> [as <alias>]

	if (args.size() != 1 && args.size() != 3)
	{
		std::println(stderr, "Invalid arguments.");
		return 1;
	}
	else if (args.size() == 1)
	{
		args[0] = std::filesystem::absolute(args[0]).string();

		auto image = NetPBM::Load(args[0]);
		if (!image)
		{
			std::println(stderr, "Failed to load image.");
			std::println(stderr, "{}", image.error());

			return 2;
		}

		if (FindImage(args[0]) != loadedImages.end())
		{
			std::println(stderr, "Image with the same path '{}' already exists. (give it an alias)", args[0]);
			return 2;
		}

		loadedImages.push_back({ .image = std::move(image.value()), .path = args[0] });
	}
	else if (args.size() == 3)
	{
		args[0] = std::filesystem::absolute(args[0]).string();

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

		if (FindImage(args[2]) != loadedImages.end())
		{
			std::println(stderr, "Image with the same alias '{}' already exists.", args[2]);
			return 2;
		}

		loadedImages.push_back({ .image = std::move(image.value()), .path = args[0], .alias = args[2] });
	}

	return 0;
}

int Application::AddFilter(const ProgramArguments& args)
{
	// add-filter <image> <filter> [as <alias>] [filter-specific-args ...]

	if (args.size() < 2)
	{
		std::println(stderr, "Invalid arguments.");
		return 1;
	}

	auto loadedImage = FindImage(args[0]);
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

		try
		{
			_int = std::stoi(args[i]);
			arguments.push_back(_int);
			continue;
		}
		catch (...) {}

		try
		{
			_float = std::stof(args[i]);
			arguments.push_back(_float);
			continue;
		}
		catch (...) {}

		if (Color::TryParse(args[i], &_Color))
		{
			arguments.push_back(_Color);
			continue;
		}

		std::println(stderr, "Cannot parse argument '{}'.", args[i]);
		return 2;
	}

	if (auto filter = Filter::Parse(args[1], arguments))
	{
		if (alias && loadedImage->FindFilter(alias.value()) != loadedImage->filters.end())
		{
			std::println(stderr, "Filter with the same alias '{}' already exists.", alias.value());
			return 2;
		}

		loadedImage->filters.push_back({ .filter = std::move(filter.value()), .alias = alias });
	}
	else
	{
		std::println(stderr, "Failed to load filter.");
		std::println(stderr, "{}", filter.error());
		return 2;
	}

	return 0;
}

int Application::RemoveFilter(const ProgramArguments& args)
{
	// remove-filter <image> {<filter>|<filter-index>}

	if (args.size() != 2)
	{
		std::println(stderr, "Invalid arguments.");
		return 1;
	}

	auto loadedImage = FindImage(args[0]);
	if (loadedImage == loadedImages.end())
	{
		std::println(stderr, "Cannot find the loaded image '{}'.", args[0]);
		return 2;
	}

	auto filter = loadedImage->FindFilter(args[1]);
	if (filter == loadedImage->filters.end())
	{
		int index;

		try
		{
			index = std::stoi(args[1]);
		}
		catch (...)
		{
			std::println(stderr, "Cannot find the applied filter '{}'.", args[1]);
			return 2;
		}

		if (index < 0)
			index = loadedImage->filters.size() + index;

		if (index < 0 || index >= loadedImage->filters.size())
		{
			std::println(stderr, "Index is out of range.");
			return 2;
		}

		loadedImage->filters.erase(loadedImage->filters.begin() + index);
		return 0;
	}

	loadedImage->filters.erase(filter);
	return 0;
}

int Application::ShowFilters(const ProgramArguments& args)
{
	// show-filters <image>

	if (args.size() != 1)
	{
		std::println(stderr, "Invalid arguments.");
		return 1;
	}

	auto loadedImage = FindImage(args[0]);
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

int Application::ShowAllFilters(const ProgramArguments& args)
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

int Application::Run(const ProgramArguments& args)
{
	// run <image>

	if (args.size() != 1)
	{
		std::println(stderr, "Invalid arguments.");
		return 1;
	}

	auto loadedImage = FindImage(args[0]);
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

int Application::RunAll(const ProgramArguments& args)
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

int Application::Save(const ProgramArguments& args)
{
	// save <image> [file]

	if (args.size() != 1 && args.size() != 2)
	{
		std::println(stderr, "Invalid arguments.");
		return 1;
	}

	auto loadedImage = FindImage(args[0]);
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

int Application::Quit(const ProgramArguments& args)
{
	isRunning = false;
	return 0;
}

bool Application::IsRunning() const { return isRunning; };

void Application::Run()
{
	auto programs =
	{
		Program::PrintWorkingDirectory,
		Program::ChangeWorkingDirectory,
		Program::ListDirectoryContents,
		Program("help", [=, this](const ProgramArguments& args) -> int { return this->Help(args); }),
		Program("load", [=, this](const ProgramArguments& args) -> int { return this->Load(args); }),
		Program("add-filter", [=, this](const ProgramArguments& args) -> int { return this->AddFilter(args); }),
		Program("remove-filter", [=, this](const ProgramArguments& args) -> int { return this->RemoveFilter(args); }),
		Program("show-filters", [=, this](const ProgramArguments& args) -> int { return this->ShowFilters(args); }),
		Program("show-all-filters", [=, this](const ProgramArguments& args) -> int { return this->ShowAllFilters(args); }),
		Program("run", [=, this](const ProgramArguments& args) -> int { return this->Run(args); }),
		Program("run-all", [=, this](const ProgramArguments& args) -> int { return this->RunAll(args); }),
		Program("save", [=, this](const ProgramArguments& args) -> int { return this->Save(args); }),
		Program("quit", [=, this](const ProgramArguments& args) -> int { return this->Quit(args); }),
	};

	isRunning = true;

	Shell shell = programs;
	std::string prompt = "> ";
	std::string input;

	while (isRunning)
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
