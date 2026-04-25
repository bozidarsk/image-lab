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
	static const Program PrintWorkingDirectory;
	static const Program ChangeWorkingDirectory;

	const std::string& GetName() const;
	ProgramEntryPoint GetEntryPoint() const;

	int Run(const ProgramArguments& args) const;

	Program(const char* name, ProgramEntryPoint entryPoint);
	Program(const std::string& name, ProgramEntryPoint entryPoint);
	Program(std::string&& name, ProgramEntryPoint entryPoint);
};
