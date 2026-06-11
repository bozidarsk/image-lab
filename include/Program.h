#pragma once

#include <span>
#include <string>
#include <functional>

using ProgramArguments = std::span<std::string>;
using ProgramEntryPoint = std::function<int(const ProgramArguments&)>;

class Program
{
private:
	std::string name;
	ProgramEntryPoint entryPoint;

public:
	static const Program PrintWorkingDirectory;
	static const Program ChangeWorkingDirectory;
	static const Program ListDirectoryContents;

	const std::string& GetName() const;
	ProgramEntryPoint GetEntryPoint() const;

	int Run(const ProgramArguments& args) const;

	Program(const std::string& name, const ProgramEntryPoint& entryPoint);
};
