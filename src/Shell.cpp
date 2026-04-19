#include <print>
#include <array>
#include <utility>
#include <optional>

#include "Shell.h"

/*static*/ std::vector<std::string> Shell::Parse(const std::string& input) 
{
	std::vector<std::string> parts;
	std::vector<char> part(input.length());

	bool isEscaped = false;
	char delim = 0;

	for (
		auto position = std::find_if(input.begin(), input.end(), [](char x) { return !std::isspace(x); });
		position != input.end();
		position++
	)
	{
		if (!isEscaped && !delim && *position != delim && (*position == '\"' || *position == '\'')) 
		{
			delim = *position;
			continue;
		}

		if (((delim && *position == delim) || (!delim && *position == ' ' && !part.empty())) && !isEscaped) 
		{
			parts.emplace_back(part.begin(), part.end());
			part.clear();
			delim = 0;
			continue;
		}

		if (*position == '\\' && !isEscaped) 
		{
			isEscaped = true;
			continue;
		}

		if (isEscaped) 
		{
			switch (*position) 
			{
				case 'n': part.push_back('\n'); break;
				case 'r': part.push_back('\r'); break;
				case 't': part.push_back('\t'); break;
				default: part.push_back(*position); break;
			}

			isEscaped = false;
			continue;
		}

		if (delim || (!delim && *position != ' '))
			part.push_back(*position);
	}

	if (!delim && !part.empty())
		parts.emplace_back(part.begin(), part.end());

	return parts;
}

int Shell::Run(const std::string& command) const 
{
	auto parts = Parse(command);

	return Run(parts[0], ProgramArguments(parts.begin() + 1, parts.end()));
}

int Shell::Run(const std::string& name, const ProgramArguments& args) const 
{
	const auto program = std::find_if(programs.begin(), programs.end(), [&name](const Program& x) { return x.GetName() == name; });

	if (program != programs.end())
		return program->Run(args);
	else 
	{
		std::println(stderr, "Unknown program '{}'.", name);
		return -1;
	}
}

Shell::Shell(const std::vector<Program>& programs) : programs(programs) {}
Shell::Shell(std::vector<Program>&& programs) : programs(std::move(programs)) {}
Shell::Shell(std::initializer_list<Program> programs) : programs(programs) {}
