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

void Engine::createTree() {

    //On crée le premier noeud (qui sert uniquement de point de jonction donc index inutile (mis à -1))
    int index = -1;
    _root = new Node(index);

    Domain d = _variables[0].getDomain();


    const std::vector<int> varsOrderedByMostConstrained = _constraints.getVariablesIndexOrderedByMostConstrained();
    for (int i : varsOrderedByMostConstrained)
        std::cout << " var " << i << std::endl;


    bool success = false;
    std::vector<int> chosenValues; // Stocke la solution

    for (int value: d) {
        Node *new_node = new Node(index+1);
        _root->addChild(new_node);

        chosenValues.push_back(value);

        if (new_node->createNode(value, _variables, chosenValues, &_constraints)) {
            success = true;
            break;
        }
        else {
            chosenValues.pop_back();
            delete(new_node);
        }
    }


    delete(_root);

    std::cout << "Nombre de noeuds cree : " << _root->getCount() - 1 << std::endl;

    /** Calcul du nombre de noeuds élagués : **/
    unsigned long long countPrunedNodes = 1; // var qui va être multipliée donc doit être = 1 pour commencer

    for (Variable v : _variables)
        countPrunedNodes *= v.getDomain().getSize();  // compte le nombre total de noeuds qui auraient pu être créé (sans prendre en compte les contraintes)

    countPrunedNodes -= (_root->getCount() - 1); // Nb noeuds élagués = nb max de noeuds créés - les noeuds créés (car les noeuds élagués n'ont pas été créé)
    /** -- -- -- -- **/

    std::cout << "Nombre de noeuds elagues : " << countPrunedNodes << std::endl;

    if (success) {
            for (unsigned int j = 0; j < chosenValues.size(); j++)
                std::cout << "Pour var " << j+1 << " value => " << chosenValues[j] << std::endl;
    }

    else {
        std::cout << "Aucune solution trouvee !" << std::endl;
    }
}

