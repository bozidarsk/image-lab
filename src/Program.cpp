#include <cstdio>
#include <print>
#include <array>
#include <filesystem>
#include <cassert>
#include <streambuf>
#include <string>

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
	static std::string fallbackPath = getenv("HOME");
	std::string& path = (args.size() != 0) ? args[0] : fallbackPath;

	if (!std::filesystem::is_directory(path))
	{
		std::println(stderr, "No such directory '{}'.", path);
		return 1;
	}

	return chdir(path.c_str()) ? errno : 0;
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
	static std::string fallbackPath = getenv("USERPROFILE");
	std::string& path = (args.size() != 0) ? args[0] : fallbackPath;

	if (!std::filesystem::is_directory(path))
	{
		std::println(stderr, "No such directory '{}'.", path);
		return 1;
	}

	return !SetCurrentDirectory(path.c_str());
}
#endif

static int ls(const ProgramArguments& args)
{
	static std::array<std::string, 1> fallbackPaths = { "." };
	std::span<std::string> paths = (args.size() >= 1) ? args : fallbackPaths;

	for (auto& path : paths)
	{
		if (paths.size() > 1)
			std::println("{}:", path);

		if (!std::filesystem::is_directory(path))
		{
			std::println(stderr, "No such directory '{}'.", path);
			return 1;
		}

		for (auto& item : std::filesystem::directory_iterator(path))
		{
			auto status = item.status();
			auto permissions = status.permissions();

			if (item.is_symlink()) std::print("l");
			else if (item.is_block_file()) std::print("b");
			else if (item.is_character_file()) std::print("c");
			else if (item.is_socket()) std::print("s");
			else if (item.is_fifo()) std::print("p");
			else if (item.is_directory()) std::print("d");
			else std::print("-");

			std::print("{}", ((permissions & std::filesystem::perms::owner_read) != std::filesystem::perms::none) ? 'r' : '-');
			std::print("{}", ((permissions & std::filesystem::perms::owner_write) != std::filesystem::perms::none) ? 'w' : '-');
			std::print("{}", ((permissions & std::filesystem::perms::owner_exec) != std::filesystem::perms::none) ? 'x' : '-');
			std::print("{}", ((permissions & std::filesystem::perms::group_read) != std::filesystem::perms::none) ? 'r' : '-');
			std::print("{}", ((permissions & std::filesystem::perms::group_write) != std::filesystem::perms::none) ? 'w' : '-');
			std::print("{}", ((permissions & std::filesystem::perms::group_exec) != std::filesystem::perms::none) ? 'x' : '-');
			std::print("{}", ((permissions & std::filesystem::perms::others_read) != std::filesystem::perms::none) ? 'r' : '-');
			std::print("{}", ((permissions & std::filesystem::perms::others_write) != std::filesystem::perms::none) ? 'w' : '-');
			std::print("{}", ((permissions & std::filesystem::perms::others_exec) != std::filesystem::perms::none) ? 'x' : '-');

			std::print(" {}", item.path().filename().string());

			if (item.is_symlink())
				std::print(" -> {}", std::filesystem::read_symlink(item.path()).string());

			std::println();
		}

		if (paths.size() > 1)
			std::println("");
	}

	return  0;
}

/*static*/ const Program Program::PrintWorkingDirectory = Program("pwd", &pwd);
/*static*/ const Program Program::ChangeWorkingDirectory = Program("cd", &cd);
/*static*/ const Program Program::ListDirectoryContents = Program("ls", &ls);

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
