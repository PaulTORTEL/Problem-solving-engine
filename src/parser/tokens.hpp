#pragma once

#include <stdlib.h>
#include <istream>
#include <unordered_map>
#include <vector>

namespace parser {

	typedef unsigned int SymbolId;

	enum class TokenType {
		Number,
		Identifier,

		KeywordVar,
		KeywordIn,
		KeywordFor,
		KeywordDistinct,

		Comma,		Semicolon,
		Ellipsis,

		Equals,			NotEquals,
		LessThan,		LessThanOrEqualsTo,
		GreaterThan,	GreaterThanOrEqualsTo,

		LBracket, 	RBracket,
		LBrace, 	RBrace,

		EndOfStream,

		//Erreurs
		ErrorUnexpected,
		ErrorStream,
		_Count
	};

	struct Token {
		TokenType type;
		int line;
		int col;

		union {
			int number;
			SymbolId identifier;
			char unexpected;
		};


		static void initTables();

		static const std::string& getNameOf(TokenType type) {
			return TABLE_TYPE_NAMES[static_cast<int>(type)];
		}

		const std::string& getName() const {
			return getNameOf(this->type);
		}
		bool isError() const {
			return this->type >= TokenType::ErrorUnexpected;
		}

		friend class Tokenizer;
		friend std::ostream& operator<<(std::ostream& o, const Token& t);

	private:
		struct Action {
			char c;
			int level;
			TokenType token;
		};

		static std::string TABLE_TYPE_NAMES[static_cast<int>(TokenType::_Count)];
		static std::vector<Action> TABLE_TYPES;
		static std::unordered_map<std::string, TokenType> TABLE_KEYWORDS;

		static void registerToken(TokenType type, const std::string& name);
		static void registerTokenSpecial(TokenType type, const std::string& name);
	};

	std::ostream& operator<<(std::ostream& o, const Token& t);
	
	class Tokenizer {
	private:
		std::istream* _stream;
		int _curLine;
		int _curCol;

		bool _hasNextTok;
		Token _nextTok;

		std::unordered_map<std::string, SymbolId> _symbolsMap;
		std::vector<std::string> _symbols;

	public:
		Tokenizer(std::istream* stream);
		~Tokenizer();

		bool hasNextToken();
		Token peekToken();
		Token nextToken();

		const std::string& getSymbolName(SymbolId id) const;


	private:

		int readChar();
		int skipWhitespace();
		Token readToken();
		Token readStringToken();
		Token readIdOrKeywordToken();
		Token readNumberToken();
		Token createToken(TokenType type);
		Token createErrorToken(int c);
	};
}