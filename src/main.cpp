#include "Engine.h"
#include <string>
using namespace std;

int main(int argc, char** argv)
{
	std::string file = "problems/test.xml";
	if(argc > 1)
		file = argv[1];

    Engine engine(file);
    engine.readVariables();

    return 0;
}
