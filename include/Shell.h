#pragma once

#include <string>
#include <vector>
#include <initializer_list>
#include <cstdint>

#include "Program.h"
#include "EnumFlags.hpp"

class Shell 
{
private:
	std::vector<Program> programs;

public:
	int Run(const std::string& command) const;
	int Run(const std::string& name, const ProgramArguments& args) const;

	Shell(const std::vector<Program>& programs);
	Shell(std::vector<Program>&& programs);
	Shell(std::initializer_list<Program> programs);

	class Parser 
	{
	private:
		struct Token 
		{
			enum class Flags : uint8_t
			{
				None         = 0,
				Escaped      = 1 << 0,
				SingleQuoted = 1 << 1,
				DoubleQuoted = 1 << 2,
				CurlyBraced  = 1 << 3,
				Quoted = SingleQuoted | DoubleQuoted,
				Surounded = Quoted | CurlyBraced,
			};

			char value;
			Flags flags;

			Token(char value, Flags flags = Flags::None);
		};

		static std::vector<Token> Tokenize(const std::string& input);
		static void ProcessEnvironmentVariables(std::vector<Token>& tokens);

	public:
		static std::vector<std::string> Parse(const std::string& command);
	};
};
