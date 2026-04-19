#pragma once

#include <span>
#include <string>

using ProgramArguments = std::span<std::string>;

typedef int(*ProgramEntryPoint)(const ProgramArguments& args);

class Program 
{
private:
	std::string name;
	ProgramEntryPoint entryPoint;

public:
	const std::string& GetName() const;
	ProgramEntryPoint GetEntryPoint() const;

	int Run(const ProgramArguments& args) const;

	Program(const std::string& name, ProgramEntryPoint entryPoint);
};
