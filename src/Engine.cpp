#include "Engine.h"
#include "Node.h"

Engine::Engine(std::vector<Variable>&& variables):
	_variables(variables),
	_constraints(variables.size()) {
}

void Engine::setIndexes(std::map<std::string, int>& indexes) {
    _varIndexes = indexes;
}
int Engine::getIndex(std::string varName) {
    if (_varIndexes.count( varName ) == 1)
        return _varIndexes[varName];
    return -1;
}

Variable* Engine::getVariableByIndex(int index) {
    return &_variables[index];
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

void Engine::createTree(TraversingOrder order) {

    //On crée le premier noeud (qui sert uniquement de point de jonction donc index inutile (mis à -1))
    int index = -1;
    _root = new Node(index);

    if (order == TraversingOrder::CONSTRAINTS_MOST || order == TraversingOrder::CONSTRAINTS_LEAST) {
        std::vector<int> varsOrder;

        /** METHODE VARIABLE LA PLUS CONTRAINTE **/
        if (order == TraversingOrder::CONSTRAINTS_MOST)
            varsOrder = _constraints.getVariablesIndexOrderedByMostOrLeastConstrained(_variables, true);

        /** METHODE VARIABLE LA MOINS CONTRAINTE **/
        else // TraversingOrder::CONSTRAINTS_LEAST
            varsOrder = _constraints.getVariablesIndexOrderedByMostOrLeastConstrained(_variables, false);

        for (unsigned int i = 0; i < varsOrder.size(); i++) {
            _variables[varsOrder[i]].setLevel(i);
        }
    }

    /** METHODE DOMAINE DE VALEURS LE PLUS PETIT **/
    bool domain_method = false;
    if (order == TraversingOrder::DOMAINS) {
        domain_method = true;
        int indexByDomain = Engine::getIndexBySmallestDomain(_variables);
        if (indexByDomain != -1)
            _variables[indexByDomain].setLevel(0);

    }

    /** METHODE CLASSIQUE (premiere var à la derniere) **/
    if (order == TraversingOrder::VARIABLES) {
        for (unsigned int i = 0; i < _variables.size(); i++)
            _variables[i].setLevel(i);
    }

    int indexByLevel = getIndexByLevel(_variables, 0);

    if (indexByLevel == -1)
        return;

    Domain& d = _variables[indexByLevel].getDomain();




    bool success = false;
    std::vector<int> chosenValues; // Stocke la solution

    for (int value : d) {
        Node *new_node = new Node(index+1);
        _root->addChild(new_node);

        chosenValues.push_back(value);

        if (new_node->createNode(value, _variables, chosenValues, &_constraints, domain_method)) {
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

    if (_root->getCountPruning() == 0)
        std::cout << "Profondeur moyenne des elagages : aucun elagage" << std::endl;
    else
        std::cout << "Profondeur moyenne des elagages : " << (_root->getCountDepth()/_root->getCountPruning()) << std::endl;

    std::cout << "Profondeur maximum d'elagage : " << _root->getMaxPruningDepth() << std::endl;

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
    _root->refreshMaxPruningDepth();
}

int Engine::getIndexByLevel(const std::vector<Variable>& variables, int level) {
    for (unsigned int i = 0; i < variables.size(); i++) {
        if (variables[i].getLevel() == level)
            return (int)i;
    }
    return -1;
}

int Engine::getIndexBySmallestDomain(std::vector<Variable>& variables) {
    int index_found = -1;
    unsigned int smallest_domain_size = 0;

    for (unsigned int i = 0; i < variables.size(); i++) {
        if (variables[i].getLevel() == -1) { // On a pas encore défini son niveau dans le traitement pour la résolution
            if (index_found == -1) {
                index_found = i;
                smallest_domain_size = variables[i].getDomain().getSize();
            }
            else {
                unsigned int temp_size = variables[i].getDomain().getSize();
                if (temp_size < smallest_domain_size) {
                    index_found = i;
                    smallest_domain_size = temp_size;
                }
            }
        }
    }
    return index_found;
}

void Engine::refreshVarsLevels() {
    for (Variable& v : _variables)
        v.setLevel(-1);
}

