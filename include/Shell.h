#pragma once

#include <string>
#include <vector>
#include <expected>
#include <initializer_list>
#include <cstdint>

#include "Program.h"
#include "EnumFlags.hpp"

class Shell
{
private:
	std::vector<Program> programs;

public:
	std::expected<int, std::string> Run(const std::string& command) const;
	std::expected<int, std::string> Run(const std::string& name, const ProgramArguments& args) const;

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
		static bool TryParse(const std::string& command, std::vector<std::string>* values);
	};
};
