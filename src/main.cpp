#include "Engine.h"
#include "XMLParser.h"
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

	for(auto it = engine.beginVars(); it != engine.endVars(); it++) {
		std::cout << *it << std::endl;
	}


    /*constraints.addBinConstraint(0,1, Constraints::BIN_CON_GREATER);
    constraints.addBinConstraint(1,2, Constraints::BIN_CON_GREATER);
    constraints.addBinConstraint(2,3, Constraints::BIN_CON_GREATER);*/

    constraints.addBinConstraint(0, 1, Constraints::BIN_CON_GREATER);
    constraints.addBinConstraint(0, 5, Constraints::BIN_CON_EQUALS);
    constraints.addBinConstraint(0, 2, Constraints::BIN_CON_GREATER);
    constraints.addBinConstraint(0, 9, Constraints::BIN_CON_GREATER);
    constraints.addBinConstraint(1, 2, Constraints::BIN_CON_GREATER);
    constraints.addBinConstraint(1, 3, Constraints::BIN_CON_GREATER);
    constraints.addBinConstraint(1, 4, Constraints::BIN_CON_GREATER);
    constraints.addBinConstraint(1, 6, Constraints::BIN_CON_EQUALS);
    constraints.addBinConstraint(1, 5, Constraints::BIN_CON_LESS);
    constraints.addBinConstraint(1, 8, Constraints::BIN_CON_GREATER);
    constraints.addBinConstraint(2, 3, Constraints::BIN_CON_GREATER);
    constraints.addBinConstraint(2, 1, Constraints::BIN_CON_LESS);
    constraints.addBinConstraint(2, 7, Constraints::BIN_CON_GREATER);
    constraints.addBinConstraint(2, 8, Constraints::BIN_CON_GREATER);
    constraints.addBinConstraint(2, 9, Constraints::BIN_CON_GREATER);
    constraints.addBinConstraint(3, 4, Constraints::BIN_CON_GREATER);
    constraints.addBinConstraint(3, 7, Constraints::BIN_CON_GREATER);
    constraints.addBinConstraint(3, 8, Constraints::BIN_CON_EQUALS);
    constraints.addBinConstraint(3, 9, Constraints::BIN_CON_LESS);
    constraints.addBinConstraint(4, 7, Constraints::BIN_CON_GREATER);
    constraints.addBinConstraint(4, 8, Constraints::BIN_CON_LESS);
    constraints.addBinConstraint(5, 2, Constraints::BIN_CON_GREATER | Constraints::BIN_CON_LESS);
    constraints.addBinConstraint(5, 6, Constraints::BIN_CON_GREATER);
    constraints.addBinConstraint(5, 7, Constraints::BIN_CON_GREATER);
    constraints.addBinConstraint(5, 8, Constraints::BIN_CON_GREATER);
    constraints.addBinConstraint(5, 9, Constraints::BIN_CON_GREATER);
    constraints.addBinConstraint(6, 2, Constraints::BIN_CON_GREATER | Constraints::BIN_CON_LESS);
    constraints.addBinConstraint(6, 0, Constraints::BIN_CON_LESS);
    constraints.addBinConstraint(6, 1, Constraints::BIN_CON_EQUALS);
    constraints.addBinConstraint(7, 4, Constraints::BIN_CON_LESS);
    constraints.addBinConstraint(8, 3, Constraints::BIN_CON_EQUALS);
    constraints.addBinConstraint(9, 0, Constraints::BIN_CON_GREATER | Constraints::BIN_CON_LESS);
    constraints.addBinConstraint(9, 2, Constraints::BIN_CON_LESS);
    constraints.addBinConstraint(9, 6, Constraints::BIN_CON_LESS);
    constraints.addBinConstraint(9, 3, Constraints::BIN_CON_GREATER);
    constraints.addBinConstraint(9, 4, Constraints::BIN_CON_GREATER);

    std::cout << "\tMethode + contrainte d'abord : " << std::endl;
    engine.createTree(1);
    std::cout << std::endl;

    std::cout << "\tMethode - contrainte d'abord : " << std::endl;
    engine.createTree(2);
    std::cout << std::endl;

    std::cout << "\tMethode classique :" << std::endl;
    engine.createTree(0);


    return 0;
}
