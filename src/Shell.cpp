#include <print>
#include <algorithm>

#include "Shell.h"

int Shell::Run(const std::string& command) const 
{
	auto values = Parser::Parse(command);

	return Run(values[0], ProgramArguments(values.begin() + 1, values.end()));
}

int Shell::Run(const std::string& name, const ProgramArguments& args) const 
{
	const auto program = std::find_if(programs.begin(), programs.end(), [&name](const Program& x) { return x.GetName() == name; });

	if (program != programs.end())
		return program->Run(args);
	else 
	{
		std::println(stderr, "Shell::Run - Unknown program '{}'.", name);
		return -1;
	}
}

Shell::Shell(const std::vector<Program>& programs) : programs(programs) {}
Shell::Shell(std::vector<Program>&& programs) : programs(std::move(programs)) {}
Shell::Shell(std::initializer_list<Program> programs) : programs(programs) {}
