#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "parser/tokens.hpp"

int main(int argc, char** argv) {
	parser::Token::initTables();

	if(argc < 2) {
		std::cerr << "Il faut specifier le fichier a charger." << std::endl;
		return EXIT_FAILURE;
	}

	std::string contents;
	std::string filename = argv[1];

	std::ifstream* file = new std::ifstream();
	file->open(filename, std::ios::in);

	parser::Tokenizer tokens(file);

	parser::Token tok;
	do {
		tok = tokens.nextToken();
		std::cout << tok << std::endl;
		if(tok.type == parser::TokenType::EndOfStream)
			break;
	} while(!tok.isError());


	return EXIT_SUCCESS;
}