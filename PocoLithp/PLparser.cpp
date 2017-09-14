#include "stdafx.h"

namespace PocoLithp {
	////////////////////// parse, read and user interaction
	// return given mumber as a string
	std::string str(long n) { std::ostringstream os; os << n; return os.str(); }

	// return true iff given character is '0'..'9'
	bool isdig(char c) { return isdigit(static_cast<unsigned char>(c)) != 0; }

	// convert given string to list of tokens
	std::list<std::string> tokenize(const std::string & str)
	{
		std::list<std::string> tokens;
		const char *s = str.c_str();
		while (*s) {
			while (*s == ' ')
				++s;
			if (*s == '\t')
				++s;
			else if (*s == '\'' || *s == '"') {
				// Read a string
				const char *t = s, sp = *s;
				unsigned escape = 0;
				do {
					++t;
					if (escape)
						escape--;
					if (*t == '\\')
						escape = 2;
				} while (*t && (escape != 0 || *t != sp));
				++t;
				tokens.push_back(std::string(s, t));
				s = t;
			} else if (*s == '(' || *s == ')') {
				tokens.push_back(*s++ == '(' ? "(" : ")");
			} else {
				const char *t = s;
				while (*t && *t != ' ' && *t != '(' && *t != ')')
					++t;
				tokens.push_back(std::string(s, t));
				s = t;
			}
		}
		return tokens;
	}

	PocoVar parseNumber(const std::string &token) {
		// TODO: This is hacky

		if (token.find('.', 0) != std::string::npos) {
			// Float
			return NumberParser::parseFloat(token);
		} else {
			// Integer or unsigned hex / octal

			// Force signed for standard numbers (hex and octal will fail)
			if (token[0] == '-')
			{
#if defined(POCO_HAVE_INT64)
				Poco::Int64 i64;
				if (NumberParser::tryParse64(token, i64))
					return i64;
#else
				long i;
				if (NumberParser::tryParse(token, i))
					return i;
#endif
			}
			// Unsigned (including hex and octal)
#if defined(POCO_HAVE_INT64)
			Poco::UInt64 ui64;
			if (NumberParser::tryParseUnsigned64(token, ui64))
				return ui64;
			if (NumberParser::tryParseHex64(token, ui64))
				return ui64;
			if (NumberParser::tryParseOct64(token, ui64))
				return ui64;
#else
			unsigned long ui;
			if (NumberParser::tryParseUnsigned(token, ui))
				return ui;
			if (NumberParser::tryParseHex(token, ui))
				return ui;
			if (NumberParser::tryParseOct(token, ui))
				return ui;
#endif
		}
		throw InvalidArgumentException("Not a number: " + token);
	}

	// numbers become Numbers; every other token is an Atom
	LithpCell atom(const std::string &token)
	{
		if (isdig(token[0]) || (token[0] == '-' && isdig(token[1]))) {
			LithpCell V = LithpCell(Var, parseNumber(token));
			return V;
		} else if (token[0] == '\'') {
			return LithpCell(Atom, token.substr(1, token.size() - 2));
		} else if (token[0] == '"') {
			return LithpCell(Var, token.substr(1, token.size() - 2));
		}
		return LithpCell(Atom, token);
	}

	// return the Lisp expression in the given tokens
	LithpCell read_from(std::list<std::string> & tokens)
	{
		const std::string token(tokens.front());
		tokens.pop_front();
		if (token == "(") {
			LithpCells c;
			while (tokens.size() > 0 && tokens.front() != ")")
				c.push_back(read_from(tokens));
			if (tokens.size() == 0)
				throw SyntaxException("Missing ')'");
			tokens.pop_front();
			return LithpCell(List, c);
		} else
			return atom(token);
	}

	// return the Lisp expression represented by the given string
	LithpCell read(const std::string & s)
	{
		auto start = std::chrono::steady_clock::now();
		std::list<std::string> tokens(tokenize(s));
		LithpCell result = read_from(tokens);
		auto end = std::chrono::steady_clock::now();
		parseTime += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		return result;
	}
}
