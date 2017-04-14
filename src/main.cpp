#include "Engine.h"
#include "XMLParser.h"
#include <string>
using namespace std;


Engine createEngine(const std::string& file) {
	try {
		return XMLParser::fromFile(file);
	} catch(EngineCreationException e) {
		std::cerr << "Erreur lors de la creation de l'engine :" << std::endl;
		std::cerr << e.what() << std::endl;
	}

	exit(1);
}

int main(int argc, char** argv)
{
	std::string file = "problems/Probleme7.xml";

	//std::string file = "problems/test6.xml";

	if(argc > 1)
		file = argv[1];


	Engine engine = createEngine(file);

	Constraints& constraints = engine.getConstraints();

	for(auto it = engine.beginVars(); it != engine.endVars(); it++) {
		std::cout << *it << std::endl;
	}


    std::cout << "\tMethode + contrainte d'abord : " << std::endl;
    engine.createTree(TraversingOrder::CONSTRAINTS_MOST);
    std::cout << std::endl;

   std::cout << "\tMethode - contrainte d'abord : " << std::endl;
    engine.createTree(TraversingOrder::CONSTRAINTS_LEAST);
    std::cout << std::endl;

    std::cout << "\tMethode classique :" << std::endl;
    engine.createTree(TraversingOrder::VARIABLES);
    std::cout << std::endl;

    std::cout << "\tMethode domaine le + petit :" << std::endl;
    engine.createTree(TraversingOrder::DOMAINS);


    return 0;
}
