#pragma once

#include <string>
#include <vector>
#include <initializer_list>

#include "Program.h"

class Shell 
{
private:
	std::vector<Program> programs;

public:
	static std::vector<std::string> Parse(const std::string& command);

	int Run(const std::string& command) const;
	int Run(const std::string& name, const ProgramArguments& args) const;

	Shell(const std::vector<Program>& programs);
	Shell(std::vector<Program>&& programs);
	Shell(std::initializer_list<Program> programs);
};
