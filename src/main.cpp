#include "Engine.h"
#include <string>
using namespace std;

int main(int argc, char** argv)
{
	std::string file = "problems/test.xml";
	if(argc > 1)
		file = argv[1];
	
	try {
		Engine engine = Engine::fromXMLFile(file);
	} catch(EngineCreationException e) {
		std::cerr << "Erreur lors de la creation de l'engine :" << std::endl;
		std::cerr << e.what() << std::endl;
	}
    
    return 0;
}
