#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "parser/tokens.hpp"

bool readWholeFile(const std::string& filename, std::string& buf) {
	std::ifstream file;
	file.open(filename, std::ios::in);

	if(!file.is_open())
		return false;

	std::ostringstream ss;
	ss << file.rdbuf();
	buf = ss.str();

	bool ok = !file.fail();
	file.close();
	return ok;
}

int main(int argc, char** argv) {

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


	/*if(!readWholeFile(filename, contents)) {
		std::cerr << "Impossible de lire le fichier " << filename << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << contents << std::endl;
	*/

	return EXIT_SUCCESS;
}