#include <iterator>
#include <algorithm>
#include <cstring>

#include "Shell.h"

/*static*/ void Shell::Parser::ProcessEnvironmentVariables(std::vector<Token>& tokens)
{
	auto findStart = [&tokens](size_t begin, size_t end, bool* canBeExpanded)
	{
		auto dollar = end;

		for (auto i = begin; i < end; i++)
		{
			if (tokens[i].value == '$' && (tokens[i].flags & Token::Flags::Escaped) == 0)
			{
				dollar = i;
				break;
			}
		}

		if (dollar == end)
		{
			*canBeExpanded = false;
			return end;
		}

		if ((tokens[dollar].flags & Token::Flags::SingleQuoted) == 0)
		{
			*canBeExpanded = true;
			return dollar;
		}

		auto position = dollar;

		while (position != begin && (tokens[dollar].flags & Token::Flags::Quoted) == (tokens[position].flags & Token::Flags::Quoted))
			position--;

		*canBeExpanded = (tokens[position].flags & Token::Flags::SingleQuoted) == 0;
		return dollar;
	};

	bool canBeExpanded;

	for (
		auto start = findStart(0, tokens.size(), &canBeExpanded);
		start < tokens.size();
		start = findStart(start + 1, tokens.size(), &canBeExpanded)
	)
	{
		if (!canBeExpanded)
			continue;

		auto stop = tokens.size();

		for (auto i = start + 1; i < tokens.size(); i++)
		{
			if (!std::isalnum(tokens[i].value) && tokens[i].value != '_')
			{
				stop = i;
				break;
			}
		}

		std::string name;
		name.reserve(stop - start);
		std::transform(tokens.begin() + start + 1, tokens.begin() + stop, std::back_inserter(name), [](const Token& x) { return x.value; });

		if (name.length() == 0)
			continue;

		const char* value = getenv(name.c_str());
		if (!value) value = "";

		const Token::Flags tokenFlags = tokens[start].flags;

		tokens.erase(tokens.begin() + start, tokens.begin() + stop);
		std::transform(value, value + strlen(value), std::inserter(tokens, tokens.begin() + start), [&tokenFlags](char x) { return Token(x, tokenFlags | Token::Flags::Escaped); });
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
		if (*position == '\\' && (flags & Token::Flags::Escaped) == 0)
		{
			flags |= Token::Flags::Escaped;
			continue;
		}

		if ((flags & Token::Flags::Escaped) != 0)
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
				if ((flags & Token::Flags::DoubleQuoted) == 0)
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
				if ((flags & Token::Flags::SingleQuoted) == 0)
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

/*static*/ bool Shell::Parser::TryParse(const std::string& command, std::vector<std::string>* values)
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
			if ((position->flags & Token::Flags::Escaped) == 0 && (position->flags & Token::Flags::Quoted) == 0)
			{
				const char quote = position->value;
				position++;

				std::transform(
					position,
					std::find_if(position, tokens.end(), [&quote](const Token& x) { return x.value == quote && (x.flags & Token::Flags::Escaped) == 0; }),
					std::back_inserter(chunk),
					[&position](const Token& x) { position++; return x.value; }
				);

				continue;
			}
		}

		if (position->value == ' ' && (position->flags & Token::Flags::Escaped) == 0)
		{
			chunks.emplace_back(chunk.begin(), chunk.end());
			chunk.clear();

			continue;
		}

		chunk.push_back(position->value);
	}

	if (!chunk.empty())
		chunks.emplace_back(chunk.begin(), chunk.end());

	*values = std::move(chunks);
	return true;
}
