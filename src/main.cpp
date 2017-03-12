#include "Engine.h"
#include <string>
using namespace std;

int main(int argc, char** argv)
{
	std::string file = "problems/test2.xml";
	if(argc > 1)
		file = argv[1];

	try {
		Engine engine = Engine::fromXMLFile(file);
		  engine.tempConstFulfiller();
        engine.createDumbTree(-1);
	} catch(EngineCreationException e) {
		std::cerr << "Erreur lors de la creation de l'engine :" << std::endl;
		std::cerr << e.what() << std::endl;
	}


    return 0;
}
