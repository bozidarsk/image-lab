#pragma once

#include <string>
#include <vector>
#include <initializer_list>
#include <cstdint>

#include "Program.h"

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
			struct Flags 
			{
			private:
				using UnderlyingType = uint8_t;

				UnderlyingType value;

			public:
				enum : UnderlyingType
				{
					None         = 0,
					Escaped      = 1 << 0,
					SingleQuoted = 1 << 1,
					DoubleQuoted = 1 << 2,
					CurlyBraced  = 1 << 3,
					Quoted = SingleQuoted | DoubleQuoted,
					Surounded = Quoted | CurlyBraced,
				};

				inline bool HasFlags(const Flags x) const { return (value & x.value) == x.value; }

				constexpr Flags operator ~ () const { return ~value; }
				constexpr Flags operator ! () const { return !value; }
				constexpr Flags operator & (const Flags x) const { return value & x.value; }
				constexpr Flags operator | (const Flags x) const { return value | x.value; }
				constexpr Flags operator ^ (const Flags x) const { return value ^ x.value; }
				constexpr Flags& operator &= (const Flags x) { value &= x.value; return *this; }
				constexpr Flags& operator |= (const Flags x) { value |= x.value; return *this; }
				constexpr Flags& operator ^= (const Flags x) { value ^= x.value; return *this; }
				constexpr bool operator == (const Flags x) const { return value == x.value; }
				constexpr bool operator != (const Flags x) const { return value != x.value; }

				constexpr explicit operator bool () const { return value != 0; }
				constexpr explicit operator UnderlyingType () const { return value; }

				constexpr Flags(UnderlyingType value) : value(value) {}
			};

			char value;
			Flags flags;

			Token(char value, Flags flags = Flags::None);
		};

		static std::vector<Token> Tokenize(const std::string& input);
		static void ProcessEnvironmentVariables(std::vector<Token>& tokens);
		static void ProcessBraceExpansions(std::vector<Token>& tokens);
		static void ProcessWildcards(std::vector<Token>& tokens);

	public:
		static std::vector<std::string> Parse(const std::string& command);
	};
};
