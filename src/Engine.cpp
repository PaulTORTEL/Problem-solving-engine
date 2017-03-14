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

void Engine::createDumbTree(int index) {

    //On crée le premier noeud (qui sert uniquement de point de jonction donc value inutile et index inutile)
    _root = new Node(index);

    Domain& d = _variables[0].getDomain();

    std::vector<int> values = d.getValues();

    for (unsigned int i = 0; i < values.size(); i++) {
        Node *new_node = new Node(index+1);
        _root->addChild(new_node);
        std::vector<int> chosenValues;
        chosenValues.push_back(values[i]);

        if (new_node->createDumbNode(values[i], _variables, chosenValues, &_constraints)) {
            // C'est ok il faut enregistrer la réponse (chosenValues à afficher)
            std::cout << "c'est ok " << std::endl;

            for (unsigned int j = 0; j < chosenValues.size(); j++)
                std::cout << "Pour var " << j << " value => " << chosenValues[j] << std::endl;
            break;
        }
    }

}
