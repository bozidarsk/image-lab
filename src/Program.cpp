#include <print>
#include <array>
#include <cassert>

#include "Program.h"

#ifndef _WIN32
#include <unistd.h>
#include <limits.h>

static int pwd(const ProgramArguments& args) 
{
	std::array<char, PATH_MAX> buffer;
	std::println("{}", std::string(getcwd(buffer.data(), sizeof(buffer))));

	return 0;
}

static int cd(const ProgramArguments& args) 
{
	if (args.size() == 0)
		return 1;

	return chdir(args[0].c_str()) ? errno : 0;
}
#else
#include <windows.h>

static int pwd(const ProgramArguments& args) 
{
	std::array<char, MAX_PATH> buffer;

	if (!GetCurrentDirectory(sizeof(buffer), buffer.data()))
		return 1;

	std::println("{}", buffer.data());

	return 0;
}

static int cd(const ProgramArguments& args) 
{
	if (args.size() == 0)
		return 1;

	return !SetCurrentDirectory(args[0].c_str());
}
#endif

/*static*/ const Program Program::PrintWorkingDirectory = Program("pwd", &pwd);
/*static*/ const Program Program::ChangeWorkingDirectory = Program("cd", &cd);

const std::string& Program::GetName() const { return name; }
ProgramEntryPoint Program::GetEntryPoint() const { return entryPoint; }

int Program::Run(const ProgramArguments& args) const { return entryPoint(args); }

Program::Program(const char* name, ProgramEntryPoint entryPoint) : name(name), entryPoint(entryPoint)
{
	assert(entryPoint);
}

Program::Program(const std::string& name, ProgramEntryPoint entryPoint) : name(name), entryPoint(entryPoint)
{
	assert(entryPoint);
}

Program::Program(std::string&& name, ProgramEntryPoint entryPoint) : name(std::move(name)), entryPoint(entryPoint)
{
	assert(entryPoint);
}
