#include "Engine.h"
#include "Node.h"

Engine::Engine(std::vector<Variable>&& variables):
	_variables(variables),
	_constraints(variables.size()) {
}

Constraints& Engine::getConstraints() {
	return _constraints;
}

const Constraints& Engine::getConstraints() const {
	return _constraints;
}


std::vector<Variable>::iterator Engine::beginVars() {
	return _variables.begin();
}
std::vector<Variable>::iterator Engine::endVars() {
	return _variables.end();
}

void Engine::createTree(int index) {

    //On crée le premier noeud (qui sert uniquement de point de jonction donc value inutile et index inutile)

    _root = new Node(index);

    Domain& d = _variables[0].getDomain();


    for (int value: d) {

        Node *new_node = new Node(index+1);
        _root->addChild(new_node);
        std::vector<int> chosenValues;
        chosenValues.push_back(value);

        if (new_node->createNode(value, _variables, chosenValues, &_constraints)) {
            // C'est ok il faut enregistrer la réponse (chosenValues à afficher)
            std::cout << "Nombre de noeuds cree dans l'arbre : " << _root->getCount() - 1 << std::endl;

            for (unsigned int j = 0; j < chosenValues.size(); j++)
                std::cout << "Pour var " << j << " value => " << chosenValues[j] << std::endl;
            break;
        }
    }

}
