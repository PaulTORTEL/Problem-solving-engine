
#include <iostream>
#include <assert.h>

#include "./tokens.hpp"

namespace parser {
	
	void Token::initTables() {
		static bool isInit = false;
		if(isInit)
			return;

		isInit = true;

		#define REGISTER(type, value, ...) registerToken##__VA_ARGS__(TokenType::type, value)

		REGISTER(Number, "number", Special);
		REGISTER(Identifier, "identifier", Special);

		REGISTER(KeywordVar, "var");
		REGISTER(KeywordIn, "in");
		REGISTER(KeywordFor, "for");
		REGISTER(KeywordDistinct, "distinct");

		REGISTER(Comma, ",");		REGISTER(Semicolon, ";");
		REGISTER(Ellipsis, "...");

		REGISTER(Equals, "==");		REGISTER(NotEquals, "!=");
		REGISTER(LessThan, "<");	REGISTER(LessThanOrEqualsTo, "<=");
		REGISTER(GreaterThan, ">");	REGISTER(GreaterThanOrEqualsTo, ">=");

		REGISTER(LBracket, "[");	REGISTER(RBracket, "]");
		REGISTER(LBrace, "{");		REGISTER(RBrace, "}");

		REGISTER(EndOfStream, "EOS", Special);
		REGISTER(ErrorStream, "Error", Special);
		REGISTER(ErrorUnexpected, "Unexpected", Special);

		#undef REGISTER
	}

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
	std::vector<Token::Action> Token::TABLE_TYPES;
	std::unordered_map<std::string, TokenType> Token::TABLE_KEYWORDS;


	void Token::registerToken(TokenType type, const std::string& name) {
		assert(name.size() > 0);

		TABLE_TYPE_NAMES[static_cast<int>(type)] = name;

		//C'est un mot-clé
		if(isCharAlphabetic(name[0])) {
			TABLE_KEYWORDS[name] = type;
			return;
		}

		//On ajoute le délimiteur s'il n'est pas présent
		if(TABLE_TYPES.empty())
			TABLE_TYPES.push_back(Token::Action {
				'\0',
				-1,
				TokenType::ErrorUnexpected
			});

		unsigned int i = 0;
		int pos = 0;
		bool middle = false;
		//On se positionne dans le tableau
		for(; i < TABLE_TYPES.size(); i++) {
			Token::Action& cur = TABLE_TYPES[i];

			if(cur.level < pos)
				break;

			if(cur.level == pos && cur.c == name[pos])
				pos++;

			if(pos == (int) name.size()) {
				middle = true;
				break;
			}
		}

		//On ajoute les éléments restants
		for(; pos < (int) name.size(); pos++) {
			TABLE_TYPES.insert(TABLE_TYPES.begin()+i, Token::Action {
				name[pos],
				pos,
				TokenType::ErrorUnexpected
			});
			i++;
		}

		Token::Action& action = TABLE_TYPES[i - (middle ? 0 : 1)];
		if(action.token != TokenType::ErrorUnexpected) {
			std::cerr << "Erreur: '" << name << "' existe deja!" << std::endl;
			exit(1);
		}
		
		action.token = type;

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

		if(isCharAlphabetic(c))
			return readIdOrKeywordToken();

		if(isCharNumeric(c) || c == '-')
			return readNumberToken();

		return readStringToken();

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


	Token Tokenizer::readStringToken() {
		Token::Action* last = &Token::TABLE_TYPES.back();
		Token::Action* lastMatch = last;
		for(Token::Action& a: Token::TABLE_TYPES) {
			int pos = last->level+1;
			int c = _stream->peek();

			if(a.level == pos) {
				if(a.c == c) {
					last = &a;
					if(a.token != TokenType::ErrorUnexpected)
						lastMatch = &a;
					
					_stream->get();
				}
			} else if (a.level < pos) {
				break;
			}
		}

		int diff = last->level - lastMatch->level;
		for(int i = 1; i < diff; i++)
			_stream->unget();

		_curCol += lastMatch->level;

		int c = _stream->peek();
		if(c == EOF || lastMatch->token == TokenType::ErrorUnexpected)
			return createErrorToken(c);

		readChar();
		return createToken(lastMatch->token);
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
		if(c == EOF) {
			createToken(_stream->eof() ? TokenType::EndOfStream : TokenType::ErrorStream);
		} else { 
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