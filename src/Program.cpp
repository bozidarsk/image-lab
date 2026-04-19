#include <cassert>

#include "Program.h"

const std::string& Program::GetName() const { return name; }
ProgramEntryPoint Program::GetEntryPoint() const { return entryPoint; }

int Program::Run(const ProgramArguments& args) const { return entryPoint(args); }

Program::Program(const std::string& name, ProgramEntryPoint entryPoint) : name(name), entryPoint(entryPoint)
{
	assert(entryPoint);
}
