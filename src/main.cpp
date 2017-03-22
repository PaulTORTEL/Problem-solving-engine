#include "Engine.h"
#include "XMLParser.h"
#include "parser/Parser.h"
#include <string>
using namespace std;


Engine createEngine(const std::string& file) {
	try {
		return XMLParser::fromFile(file);
		//return Parser::fromFile(file);
	} catch(EngineCreationException e) {
		std::cerr << "Erreur lors de la creation de l'engine :" << std::endl;
		std::cerr << e.what() << std::endl;
	}

	exit(1);
}

int main(int argc, char** argv)
{
	std::string file = "problems/test2.xml";
	if(argc > 1)
		file = argv[1];
	

	Engine engine = createEngine(file);
	Constraints& constraints = engine.getConstraints();


	std::vector<Domain> domains;	
	(engine.beginVars() + 1)->getDomain().remove(1);


	for(auto it = engine.beginVars(); it != engine.endVars(); it++) {
		std::cout << *it << std::endl;
		domains.push_back(it->getDomain());
	}

	/*constraints.addBinConstraint(1, 2, Constraints::BIN_CON_LESS | Constraints::BIN_CON_GREATER);
    constraints.addBinConstraint(1, 3, Constraints::BIN_CON_LESS | Constraints::BIN_CON_GREATER);*/
    constraints.addBinConstraint(0, 1, /*Constraints::BIN_CON_LESS | */Constraints::BIN_CON_GREATER);

    /*constraints.addBinConstraint(2, 1, Constraints::BIN_CON_LESS | Constraints::BIN_CON_GREATER);
    constraints.addBinConstraint(2, 3, Constraints::BIN_CON_LESS | Constraints::BIN_CON_GREATER);*/
    //constraints.addBinConstraint(2, 1, /*Constraints::BIN_CON_LESS | */Constraints::BIN_CON_GREATER);

    /*constraints.addBinConstraint(3, 1, Constraints::BIN_CON_LESS | Constraints::BIN_CON_GREATER);
    constraints.addBinConstraint(3, 2, Constraints::BIN_CON_LESS | Constraints::BIN_CON_GREATER);*/
    //constraints.addBinConstraint(3, 2, /*Constraints::BIN_CON_LESS |*/ Constraints::BIN_CON_GREATER);

    /*constraints.addBinConstraint(0, 1, Constraints::BIN_CON_LESS | Constraints::BIN_CON_GREATER);
    constraints.addBinConstraint(0, 2, Constraints::BIN_CON_LESS | Constraints::BIN_CON_GREATER);
    constraints.addBinConstraint(0, 3, Constraints::BIN_CON_LESS | Constraints::BIN_CON_GREATER);*/




	std::cout << constraints.isValuePossible(domains, 0, 1) << std::endl;
	std::cout << constraints.isValuePossible(domains, 0, 2) << std::endl;
	std::cout << constraints.isValuePossible(domains, 0, 3) << std::endl;


    //engine.createDumbTree(-1);

    return 0;
}
