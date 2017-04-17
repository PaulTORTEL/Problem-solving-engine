#include "Engine.h"
#include "XMLParser.h"
#include <string>
#include <dirent.h>
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

std::vector<std::string> GetProblems(const std::string& path) {
    std::vector<std::string> files;
    struct dirent *entry;
    DIR *dp;

    dp = opendir(path.c_str());
    if (dp == NULL) {
        return files;
    }

    while ((entry = readdir(dp))) {
        std::string filename(entry->d_name);

        if (filename.find(".xml") != std::string::npos) {
            files.push_back(filename);
        }
    }
    closedir(dp);
    return files;
}

const std::string* ChooseProblems(const std::vector<std::string>& problems) {
    std::cout << "Choisir un fichier a charger :" << std::endl;
    std::cout << "0. Quitter" << std::endl;
    for (unsigned int i = 0; i < problems.size(); i++) {
        std::cout << (i + 1) << ". " << problems[i] << std::endl;
    }

    unsigned int choice = 0;
    while (choice <= 0 || choice > problems.size()) {
        std::cout << "Choix : ";
        std::cin >> choice;
        if(std::cin.fail()) {
            std::cin.clear();
        } else {
            std::cin.ignore(1, '\n');
            if(choice == 0)
                return NULL;
        }
    }
    return &problems[choice-1];
}

int main(int argc, char** argv)
{
    std::string folder = "problems";
    if(argc > 1) 
        folder = std::string(argv[1]);
    
    std::vector<std::string> problems = GetProblems(folder);

    while(true){

        std::cout << std::endl << "\t    *=====*=====* Programme de resolution de probleme *=====*=====*"<< std::endl << std::endl;
        std::cout << "\t*=====*=====* Propagation et Satisfaction de Contraintes *=====*=====*" << std::endl << std::endl;
        std::cout << "\tDevs: Vincent COURSAC, Paul TORTEL, Matthieu PILAUDEAU,\n\t      Arthur HEUILLARD & Thomas CORNIER " << std::endl << std::endl;

        const std::string* choice = ChooseProblems(problems);
        if(choice==NULL)
            return 0;
        std::string file = folder + "/" + *choice;

        Engine engine = createEngine(file);

        int edgeConsistencyThreshold = -1;


        while (edgeConsistencyThreshold < 0) {
            std::cout << "Quel est le seuil a ne pas depasser pour utiliser de la coherence d'arete ? (nombre de variables)" << std::endl;
            std::cin >> edgeConsistencyThreshold;
            if(std::cin.fail()) {
                std::cin.clear();
            }
        }

        std::cout << std::endl << std::endl;
        for(auto it = engine.beginVars(); it != engine.endVars(); it++) {
            std::cout << *it << std::endl;
        }

        std::cout << std::endl << "\tMethode \"la variable la + contrainte d'abord\" : " << std::endl<<std::endl;
        engine.createTree(TraversingOrder::CONSTRAINTS_MOST, edgeConsistencyThreshold);
        std::cout << std::endl;

        std::cout << std::endl << "\tMethode \"la variable la - contrainte d'abord\" : " << std::endl<<std::endl;
        engine.createTree(TraversingOrder::CONSTRAINTS_LEAST, edgeConsistencyThreshold);
        std::cout << std::endl;


        std::cout << std::endl << "\tMethode \"les variables dans leur ordre de declaration\" :" << std::endl<<std::endl;
        engine.createTree(TraversingOrder::VARIABLES, edgeConsistencyThreshold);
        std::cout << std::endl;



        std::cout << std::endl << "\tMethode \"la variable ayant domaine le + petit d'abord\" :" << std::endl<<std::endl;
        engine.createTree(TraversingOrder::DOMAINS, edgeConsistencyThreshold);

        std::cout << std::endl;
        system("PAUSE");
        system("CLS");

    }

    return 0;
}
