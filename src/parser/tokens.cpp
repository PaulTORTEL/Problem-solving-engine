
#include <iostream>

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

	std::ostream& operator<<(std::ostream& o, Token t) {
	    o << "T" << static_cast<int>(t.type);
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

		createToken(Token::fromSymbol(c));

		if(_nextTok.type != TokenType::ErrorUnexpected) {
			readChar();
			return _nextTok;
		}

		if(c == '.')
			return readStringToken("...", TokenType::Ellipsis);

		if(c == '!')
			return readStringToken("!=", TokenType::NotEquals);

		if(c == '=')
			return readStringToken("==", TokenType::Equals);

		if(c == '-' || isCharNumeric(c))
			return readNumberToken();

		if(isCharAlphabetic(c))
			return readIdOrKeywordToken();

		return createErrorToken(c);
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

		_nextTok.type = Token::fromKeyword(str);
		if(_nextTok.type != TokenType::ErrorUnexpected)
			return _nextTok;

		_nextTok.type = TokenType::Identifier;

		auto it = _symbolsMap.find(str);
		
		if(it == _symbolsMap.end()) {
			int newId = _symbolsMap.size();
			_nextTok.identifier = newId;
			_symbolsMap[str] = newId;
		} else _nextTok.identifier = it->second;

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