#include "Engine.h"
#include "Node.h"

Engine::Engine(std::string fileName)
{
    _fileName = fileName;
}

Engine::~Engine()
{
    //dtor
}

void Engine::createDumbTree(int index) {

    //On crée le premier noeud (qui sert uniquement de point de jonction donc value inutile et index inutile)
    _root = new Node(index);

    Domain& d = _variables[0].getDomain();

    std::vector<int> values = d.getPossibleValues();
    for (unsigned int i = 0; i < values.size(); i++) {
        _root->addChild(new Node(index+1));
        std::vector<int> chosenValues;
        chosenValues.push_back(values[i]);
        if (_root->createDumbNode(values[i], _variables, chosenValues)) {
            // C'est ok il faut enregistrer la réponse (chosenValues à afficher)
        }
    }

}
