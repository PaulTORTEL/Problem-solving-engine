#include <stdlib.h>
#include <istream>
#include <unordered_map>
#include <vector>

namespace parser {

	typedef unsigned int SymbolId;

	enum class TokenType {
		Number,
		Identifier,

		KeywordIn,
		KeywordFor,
		KeywordDistinct,

		Comma,
		Ellipsis,
		Equals,
		NotEquals,

		LBracket,
		RBracket,
		LBrace,
		RBrace,

		Semicolon,

		EndOfStream,

		ErrorUnexpected,
		ErrorStream,
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

		inline bool isError() const {
			return this->type >= TokenType::ErrorUnexpected;
		}

		inline static TokenType fromSymbol(char c) {
			if(c == '[')
				return TokenType::LBracket;
			if(c == ']')
				return TokenType::RBracket;
			if(c == '{')
				return TokenType::LBrace;
			if(c == '}')
				return TokenType::RBrace;
			if(c == ';')
				return TokenType::Semicolon;
			if(c == ',')
				return TokenType::Comma;

			return TokenType::ErrorUnexpected;
		} 

		inline static TokenType fromKeyword(std::string str) {
			if(str == "in")
				return TokenType::KeywordIn;
			if(str == "for")
				return TokenType::KeywordFor;
			if(str == "distinct")
				return TokenType::KeywordDistinct;

			return TokenType::ErrorUnexpected;
		}
	};

	std::ostream& operator<<(std::ostream& o, Token t);
	
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
		Token readStringToken(const std::string& str, TokenType type);
		Token readIdOrKeywordToken();
		Token readNumberToken();
		Token createToken(TokenType type);
		Token createErrorToken(int c);
	};
}