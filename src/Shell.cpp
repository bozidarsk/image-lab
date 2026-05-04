#include <expected>
#include <format>
#include <algorithm>

#include "Shell.h"

std::expected<int, std::string> Shell::Run(const std::string& command) const
{
	std::vector<std::string> values;

	if(!Parser::TryParse(command, &values))
		return std::unexpected("Syntax error.");

	return (values.size() > 0) ? Run(values[0], ProgramArguments(values.begin() + 1, values.end())) : 0;
}

std::expected<int, std::string> Shell::Run(const std::string& name, const ProgramArguments& args) const
{
	const auto program = std::find_if(programs.begin(), programs.end(), [&name](const Program& x) { return x.GetName() == name; });

	if (program != programs.end())
		return program->Run(args);
	else
		return std::unexpected(std::format("Unknown program '{}'.", name));
}

Shell::Shell(const std::vector<Program>& programs) : programs(programs) {}
Shell::Shell(std::vector<Program>&& programs) : programs(std::move(programs)) {}
Shell::Shell(std::initializer_list<Program> programs) : programs(programs) {}
