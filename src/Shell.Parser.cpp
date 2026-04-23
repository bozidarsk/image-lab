#include <algorithm>
#include <cstring>

#include "Shell.h"

/*static*/ void Shell::Parser::ProcessEnvironmentVariables(std::vector<Token>& tokens) 
{
	// TODO: figure out if it is '"$VAR"' or "'$VAR'" - currently both are ignored, but the second one must be expanded

	auto startFindCondition = [](const Token& x) { return x.value == '$' && !(x.flags & (Token::Flags::Escaped | Token::Flags::SingleQuoted)); };
	auto stopFindCondition = [](const Token& x) { return !std::isalnum(x.value) && x.value != '_'; };

	for (
		auto start = std::find_if(tokens.begin(), tokens.end(), startFindCondition);
		start != tokens.end();
		start = std::find_if(start + 1, tokens.end(), startFindCondition)
	)
	{
		auto stop = std::find_if(start + 1, tokens.end(), stopFindCondition);

		std::string name;
		std::transform(start + 1, stop, std::back_inserter(name), [](const Token& x) { return x.value; });

		if (name.length() == 0)
			continue;

		const char* value = getenv(name.c_str());
		if (!value) value = "";

		const Token::Flags tokenFlags = start->flags;

		tokens.erase(start, stop);
		std::transform(value, value + strlen(value), std::inserter(tokens, start), [&tokenFlags](char x) { return Token(x, tokenFlags | Token::Flags::Escaped); });
	}
}

/*static*/ std::vector<Shell::Parser::Token> Shell::Parser::Tokenize(const std::string& input) 
{
	std::vector<Token> tokens;
	Token::Flags flags = Token::Flags::None;

	for (
		auto position = std::find_if(input.begin(), input.end(), [](char x) { return !std::isspace(x); });
		position != input.end();
		position++
	)
	{
		if (*position == '\\' && !(flags & Token::Flags::Escaped)) 
		{
			flags |= Token::Flags::Escaped;
			continue;
		}

		if (flags & Token::Flags::Escaped) 
		{
			switch (*position) 
			{
				case 'n': tokens.emplace_back('\n', flags); break;
				case 'r': tokens.emplace_back('\r', flags); break;
				case 't': tokens.emplace_back('\t', flags); break;
				default: tokens.emplace_back(*position, flags); break;
			}

			flags &= ~Token::Flags::Escaped;
			continue;
		}

		switch (*position) 
		{
			case '\"':
				if (!(flags & Token::Flags::DoubleQuoted)) 
				{
					tokens.emplace_back(*position, flags);
					flags |= Token::Flags::DoubleQuoted;
				}
				else 
				{
					flags &= ~Token::Flags::DoubleQuoted;
					tokens.emplace_back(*position, flags);
				}
				continue;
			case '\'':
				if (!(flags & Token::Flags::SingleQuoted)) 
				{
					tokens.emplace_back(*position, flags);
					flags |= Token::Flags::SingleQuoted;
				}
				else 
				{
					flags &= ~Token::Flags::SingleQuoted;
					tokens.emplace_back(*position, flags);
				}
				continue;
			case '{':
				tokens.emplace_back(*position, flags);
				flags |= Token::Flags::CurlyBraced;
				continue;
			case '}':
				flags &= ~Token::Flags::CurlyBraced;
				tokens.emplace_back(*position, flags);
				continue;
		}

		if (*position == ' ' && tokens.back().value == ' ' && flags == Token::Flags::None && tokens.back().flags == Token::Flags::None)
			continue;

		tokens.emplace_back(*position, flags);
	}

	return tokens;
}

/*static*/ std::vector<std::string> Shell::Parser::Parse(const std::string& command) 
{
	std::vector<Token> tokens = Tokenize(command);

	ProcessEnvironmentVariables(tokens);

	std::vector<std::string> chunks;
	std::vector<char> chunk;
	chunk.reserve(tokens.size());

	for (auto position = tokens.begin(); position != tokens.end(); position++) 
	{
		if (position->value == '\"' || position->value == '\'') 
		{
			if (!(position->flags & Token::Flags::Escaped) && !(position->flags & Token::Flags::Quoted)) 
			{
				const char quote = position->value;
				position++;

				std::transform(
					position,
					std::find_if(position, tokens.end(), [&quote](const Token& x) { return x.value == quote && !(x.flags & Token::Flags::Escaped); }),
					std::back_inserter(chunk),
					[&position](const Token& x) { position++; return x.value; }
				);

				continue;
			}
		}

		if (position->value == ' ' && !(position->flags & Token::Flags::Escaped)) 
		{
			chunks.emplace_back(chunk.begin(), chunk.end());
			chunk.clear();

			continue;
		}

		chunk.push_back(position->value);
	}

	if (!chunk.empty())
		chunks.emplace_back(chunk.begin(), chunk.end());

	return chunks;
}
