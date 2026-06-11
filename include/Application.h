#pragma once

#include <optional>

#include "Image.h"
#include "Filter.h"
#include "Program.h"

class Application
{
private:
	struct LoadedFilter
	{
		std::unique_ptr<Filter> filter;
		std::optional<std::string> alias = std::nullopt;
	};

	struct LoadedImage
	{
		Image image;
		std::string path;
		std::vector<LoadedFilter> filters;
		std::optional<std::string> alias = std::nullopt;
	};

	std::vector<LoadedImage> loadedImages;
	bool isRunning = false;

	int Help(const ProgramArguments& args);
	int Load(const ProgramArguments& args);
	int AddFilter(const ProgramArguments& args);
	int RemoveFilter(const ProgramArguments& args);
	int ShowFilters(const ProgramArguments& args);
	int ShowAllFilters(const ProgramArguments& args);
	int Run(const ProgramArguments& args);
	int RunAll(const ProgramArguments& args);
	int Save(const ProgramArguments& args);
	int Quit(const ProgramArguments& args);

public:
	bool IsRunning() const;
	void Run();
};
