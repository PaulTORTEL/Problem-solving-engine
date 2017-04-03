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

const std::vector<Variable>& Engine::getVariables() const {
	return _variables;
}

void Engine::createTree(int choice) {

    //On crée le premier noeud (qui sert uniquement de point de jonction donc index inutile (mis à -1))
    int index = -1;
    _root = new Node(index);


    if (choice == 1 || choice == 2) {
        std::vector<int> varsOrderedByMostOrLeastConstrained;

        /** METHODE VARIABLE LA PLUS CONTRAINTE **/
        if (choice == 1)
            varsOrderedByMostOrLeastConstrained = _constraints.getVariablesIndexOrderedByMostOrLeastConstrained(_variables, true);

        /** METHODE VARIABLE LA MOINS CONTRAINTE **/
        else // choice == 2
            varsOrderedByMostOrLeastConstrained = _constraints.getVariablesIndexOrderedByMostOrLeastConstrained(_variables, false);

        for (unsigned int i = 0; i < varsOrderedByMostOrLeastConstrained.size(); i++) {
            for (unsigned int j = 0; j < _variables.size(); j++) {
                if ((unsigned)varsOrderedByMostOrLeastConstrained[i] == j) {
                    _variables[j].setLevel(i);
                }
            }
        }
    }


    /** METHODE CLASSIQUE (premiere var à la derniere) **/
    if (choice == 0) {
        for (unsigned int i = 0; i < _variables.size(); i++)
            _variables[i].setLevel(i);
    }

    int indexByLevel = getIndexByLevel(_variables, 0);

    if (indexByLevel == -1)
        return;

    Domain& d = _variables[indexByLevel].getDomain();



    std::vector<int> values = d.getValues();

    bool success = false;
    std::vector<int> chosenValues; // Stocke la solution

    for (unsigned int i = 0; i < values.size(); i++) {
        Node *new_node = new Node(index+1);
        _root->addChild(new_node);

        chosenValues.push_back(values[i]);

        if (new_node->createNode(values[i], _variables, chosenValues, &_constraints)) {
            success = true;
            break;
        }
        else {
            chosenValues.pop_back();
            delete(new_node);
        }
    }


    delete(_root);
    refreshVarsLevels();

    std::cout << "Nombre de noeuds cree : " << _root->getCount() - 1 << std::endl;

    /** Calcul et affichage du nombre de noeuds élagués : **/
    unsigned long long countPrunedNodes = 1; // var qui va être multipliée donc doit être = 1 pour commencer

    for (Variable v : _variables)
        countPrunedNodes *= v.getDomain().getSize();  // compte le nombre total de noeuds qui auraient pu être créé (sans prendre en compte les contraintes)

    countPrunedNodes -= (_root->getCount() - 1); // Nb noeuds élagués = nb max de noeuds créés - les noeuds créés (car les noeuds élagués n'ont pas été créé)
    std::cout << "Nombre de noeuds elagues : " << countPrunedNodes << std::endl;
    /** -- -- -- -- **/

    std::cout << "Moyenne des elagages : " << (_root->getCountDepth()/_root->getCountPruning()) << std::endl;
    if (success) {
            for (unsigned int j = 0; j < chosenValues.size(); j++)
                std::cout << "Pour var " << j+1 << " value => " << chosenValues[j] << std::endl;
    }

    else {
        std::cout << "Aucune solution trouvee !" << std::endl;
    }

    _root->refreshCount();
    _root->refreshCountPruning();
    _root->refreshCountDepth();
}

int Engine::getIndexByLevel(const std::vector<Variable>& variables, int level) {
    for (unsigned int i = 0; i < variables.size(); i++) {
        if (variables[i].getLevel() == level)
            return (int)i;
    }
    return -1;
}

void Engine::refreshVarsLevels() {
    for (Variable& v : _variables)
        v.setLevel(-1);
}
