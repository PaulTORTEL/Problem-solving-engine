
#include <iostream>
#include <assert.h>

#include "./tokens.hpp"

namespace parser {
	
	bool isCharNumeric(char c) {
		return c >= '0' && c <= '9';
	}

	bool isCharAlphabetic(char c) {
		return c == '_'
		   || (c >= 'A' && c <= 'Z')
		   || (c >= 'a' && c <= 'z');
	}

	bool isCharAlphanumeric(char c) {
		return isCharNumeric(c) || isCharAlphabetic(c);
	}

	std::string Token::TABLE_TYPE_NAMES[static_cast<int>(TokenType::_Count)];
	TokenType Token::TABLE_TYPE_FROM_CHAR[256];
	std::unordered_map<std::string, TokenType> Token::TABLE_KEYWORDS;

	void Token::initTables() {
		static bool isInit = false;
		if(isInit)
			return;

		isInit = true;

		for(int i = 0; i < 256; i++)
			TABLE_TYPE_FROM_CHAR[i] = TokenType::ErrorUnexpected;

		registerTokenNumber(TokenType::Number, "number");
		registerTokenId(TokenType::Identifier, "identifier");

		registerTokenKeyword(TokenType::KeywordVar, "var");
		registerTokenKeyword(TokenType::KeywordIn, "in");
		registerTokenKeyword(TokenType::KeywordFor, "for");
		registerTokenKeyword(TokenType::KeywordDistinct, "distinct");

		registerToken(TokenType::Comma, ",");
		registerToken(TokenType::Semicolon, ";");
		registerToken(TokenType::Ellipsis, "...");
		registerToken(TokenType::Equals, "==");
		registerToken(TokenType::NotEquals, "!=");

		registerToken(TokenType::LBracket, "[");
		registerToken(TokenType::RBracket, "]");
		registerToken(TokenType::LBrace, "{");
		registerToken(TokenType::RBrace, "}");

		registerTokenSpecial(TokenType::EndOfStream, "EOS");
		registerTokenSpecial(TokenType::ErrorStream, "Error");
		registerTokenSpecial(TokenType::ErrorUnexpected, "Unexpected");
	}

	void Token::assignTokenFirstChar(TokenType type, const std::string& name, char c) {
		unsigned char uc = c;
		if(TABLE_TYPE_FROM_CHAR[uc] != TokenType::ErrorUnexpected) {
			std::cerr << "ERROR: multiple tokens for one char : " << name << std::endl;
			exit(1); 
		}

		TABLE_TYPE_FROM_CHAR[uc] = type;
	}

	void Token::registerToken(TokenType type, const std::string& name) {
		assert(name.size() > 0);

		TABLE_TYPE_NAMES[static_cast<int>(type)] = name;

		assignTokenFirstChar(type, name, name[0]);

	}

	void Token::registerTokenNumber(TokenType type, const std::string& name) {
		TABLE_TYPE_NAMES[static_cast<int>(type)] = name;

		for(int c = 0; c < 256; c++) {
			if(isCharNumeric(c) || c == '-')
				assignTokenFirstChar(type, name, c);
		}
	}


	void Token::registerTokenId(TokenType type, const std::string& name) {
		TABLE_TYPE_NAMES[static_cast<int>(type)] = name;

		for(int c = 0; c < 256; c++) {
			if(isCharAlphabetic(c))
				assignTokenFirstChar(type, name, c);
		}
	}

	void Token::registerTokenKeyword(TokenType type, const std::string& name) {
		TABLE_TYPE_NAMES[static_cast<int>(type)] = name;
		TABLE_KEYWORDS[name] = type;
	}

	void Token::registerTokenSpecial(TokenType type, const std::string& name) {
		TABLE_TYPE_NAMES[static_cast<int>(type)] = name;
	}


	std::ostream& operator<<(std::ostream& o, const Token& t) {
		const std::string& name = Token::TABLE_TYPE_NAMES[static_cast<int>(t.type)];

	    if(name.size() > 0)
	    	o << name;
	    else o << "unnamed[" << t.getName() << "]";

	    if(t.type == TokenType::Number)
	    	o << "(" << t.number << ")";

	    else if(t.type == TokenType::Identifier)
	    	o << "(" << t.identifier << ")";

	    else if(t.type == TokenType::ErrorUnexpected)
	    	o << "(" << t.unexpected << ")";

	    o << "@" << t.line << ":" << t.col;
	    return o;
	}

	Tokenizer::Tokenizer(std::istream* stream):
		_stream(stream),
		_curLine(1),
		_curCol(1),
		_hasNextTok(false)
		{
	}

	Tokenizer::~Tokenizer() {
		if(_stream)
			delete _stream;
	}

	bool Tokenizer::hasNextToken() {
		peekToken();
		return _stream != nullptr;
	}


	Token Tokenizer::peekToken() {
		if(_hasNextTok)
			return _nextTok;

		_hasNextTok = true;
		return readToken();
	}

	Token Tokenizer::nextToken() {
		if(_hasNextTok) {
			if(_stream)
				_hasNextTok = false;
			return _nextTok;
		}

		return readToken();
	}

	Token Tokenizer::readToken() {
		int c = skipWhitespace();

		if(c == EOF)
			return createErrorToken(c);

		TokenType type = Token::TABLE_TYPE_FROM_CHAR[c];

		switch(type) {
		case TokenType::Identifier:
			return readIdOrKeywordToken();

		case TokenType::Number:
			return readNumberToken();

		case TokenType::ErrorUnexpected:
			return createErrorToken(c);

		default:
			const std::string& name = Token::getNameOf(type);
			if(name.size() == 1) {
				createToken(type);
				readChar();
				return _nextTok;	
			
			} else return readStringToken(name, type);
		}
	}

	int Tokenizer::readChar() {
		_curCol++;
		return _stream->get();
	}

	int Tokenizer::skipWhitespace() {
		while(true) {
			int c = _stream->peek();

			if(c == '\n') {
				_curCol = 1;
				_curLine++;
			
			} else if(c == '#') {
				while(c != '\n' && c != EOF)
					c = readChar();

				if(c == '\n') {
					_curCol = 1;
					_curLine++;
					_stream->putback(c);
				} else return EOF;

			} else if(c != '\t' && c != ' ')
				return c;

			readChar();
		}
	}

	Token Tokenizer::readIdOrKeywordToken() {
		std::string str = "";
	
		while(isCharAlphanumeric(_stream->peek())) {
			str.append(1, readChar());	
		}

		auto it = Token::TABLE_KEYWORDS.find(str);
		if(it != Token::TABLE_KEYWORDS.end()) {
			_nextTok.type = it->second;
			return _nextTok;
		}

		_nextTok.type = TokenType::Identifier;

		auto it2 = _symbolsMap.find(str);
		
		if(it2 == _symbolsMap.end()) {
			int newId = _symbolsMap.size();
			_nextTok.identifier = newId;
			_symbolsMap[str] = newId;
		} else _nextTok.identifier = it2->second;

		return _nextTok;

	}

	Token Tokenizer::readNumberToken() {
		bool negative = false;

		if(_stream->peek() == '-') {
			negative = true;
			readChar();
		}
		
		int value = 0;

		while(isCharNumeric(_stream->peek())) {
			int i = readChar() - '0';
			value = value*10 + i;
		}

		if(negative)
			value *= -1;

		_nextTok.type = TokenType::Number;
		_nextTok.number = value;
		return _nextTok;
	}


	Token Tokenizer::readStringToken(const std::string& str, TokenType type) {
		static char buf[20];
		_stream->read(buf, str.size());

		if(_stream->fail() || _stream->eof()) {
			_curCol += _stream->gcount();
			return createErrorToken(EOF);
		}

		for(unsigned int i = 0; i < str.size(); i++) {
			if(buf[i] != str[i]) {
				_curCol += i;
				return createErrorToken(buf[i]);
			}
		}

		_nextTok.type = type;
		_curCol += str.size();
		return _nextTok;
	}

	Token Tokenizer::createToken(TokenType type) {
		Token t;
		t.type = type;
		t.line = _curLine;
		t.col = _curCol;

		_nextTok = t;
		return t;
	}


	Token Tokenizer::createErrorToken(int c) {
		if(c == EOF)
			createToken(_stream->fail() ? TokenType::ErrorStream : TokenType::EndOfStream);
		else { 
			createToken(TokenType::ErrorUnexpected);
			_nextTok.unexpected = c;
		}

		_hasNextTok = true;
		delete _stream;
		_stream = nullptr;
		return _nextTok;
	}

	const std::string& Tokenizer::getSymbolName(SymbolId id) const {
		return _symbols.at(id);
	}

}