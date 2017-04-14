#include "Node.h"
#include <stdlib.h>
#include "Engine.h"

Node::Node(int index)
{
    _index = index;
    _count++;
}

Node::~Node()
{

}

std::vector<Node*> Node::getChildren() {
    return _children;
}

void Node::removeChild(Node* child) {
    if (!child)
        return;
    for (std::vector<Node*>::iterator it = _children.begin(); it != _children.end(); ++it) {
        if (*it == child) {
            _children.erase(it);
            break;
        }
    }
}

unsigned int Node::_count = 0;
float Node::_countPruning = 0.0;
float Node::_countDepth = 0.0;
unsigned int Node::_maxPruningDepth = 0;


bool Node::createNode(int value, std::vector<Variable> vars, std::vector<int>& chosenValues, Constraints* constraints, const bool& domain_method) {

    if (domain_method) {
        for (unsigned int i = 0; i < vars.size(); i++) {
            if (vars[i].getLevel() > _index)
                vars[i].setLevel(-1);
        }
        for (unsigned int i = _index+1; i < vars.size(); i++) {
            int indexBySmallestDomain = Engine::getIndexBySmallestDomain(vars);
            vars[indexBySmallestDomain].setLevel(i);
        }
    }

    Domain& domain_current_var = vars[Engine::getIndexByLevel(vars, _index)].getDomain();

    // Le domaine de la variable traitée est transformé en un singleton
    domain_current_var.removeLessThan(chosenValues.back());
    domain_current_var.removeGreaterThan(chosenValues.back());

    if ((unsigned)_index + 1 >= vars.size()) {

        std::vector<Domain> all_vars_domain;
        for (Variable& var: vars) {
            Domain& dom = var.getDomain();

            all_vars_domain.push_back(dom); // On récupère tous les domaines
        }

        // Il faut remettre dans l'ordre chosenValues dans le cas où on aurait traité les vars dans le désordre (ex avec variables + contraintes)
        std::vector<int> chosenValuesSave = chosenValues;

        sortChosenValuesByNaturalOrder(vars, chosenValues);

        for (unsigned int i = 0; i < chosenValues.size(); i++) {
            if (!constraints->isValuePossible(all_vars_domain, i, chosenValues[i])) {
                chosenValues = chosenValuesSave;
                return false;
            }
        }

        return true;
    }

    /** REDUCTION DE DOMAINE **/
    if (!reduceDomains(vars, constraints)) { // Si on a trouvé un ou plusieurs domaines de valeurs vides
        _countPruning++;                        // On trouve la pronfondeur max et on augmente de 1 le nombre d'élagages faits
        _countDepth += _index;
        if ((unsigned)_index > _maxPruningDepth)
            _maxPruningDepth = _index;

        return false;
    }
    /** === **/

    Domain& d = vars[Engine::getIndexByLevel(vars, _index+1)].getDomain(); // récupération du domaine de la valeur suivante

    /** COHERENCE D'ARETE **/
    if (vars.size() < 20) {
        bool success = false;
        for (unsigned int cpt = 0; cpt < d.getSize(); cpt++) {
            if (edgeConsistency(vars, constraints, _index+1, d[cpt]))
                success = true;
        }
        if (!success) {
            _countPruning++;        // On trouve la pronfondeur max et on augmente de 1 le nombre d'élagages faits
            _countDepth += _index;
            if ((unsigned)_index > _maxPruningDepth)
                _maxPruningDepth = _index;

            return false;
        }
    }

    /** === **/

    for (unsigned int cpt = 0; cpt < d.getSize(); cpt++) {

        Node * new_node = new Node(_index+1);
        this->addChild(new_node);
        chosenValues.push_back(d[cpt]);

        if (new_node->createNode(d[cpt], vars, chosenValues, constraints, domain_method)) {
            return true;
        }
        else {
            chosenValues.pop_back();
            removeChild(new_node);
            delete(new_node);
        }
    }

    return false;
}

bool Node::reduceDomains(std::vector<Variable>& vars, Constraints* constraints) {

    if ((unsigned)_index + 1 >= vars.size())
        return true;

    std::vector<Domain> all_vars_domain;
    for (Variable& var : vars) {
        Domain& dom = var.getDomain();

        all_vars_domain.push_back(dom); // On récupère tous les domaines
    }

    for (unsigned int i = _index + 1; i < vars.size(); i++) {
        int indexByLevel = Engine::getIndexByLevel(vars, i);
        Domain& d = vars[indexByLevel].getDomain();

        std::vector<int> toRemove;

        for (unsigned int cpt = 0; cpt < d.getSize(); cpt++) {
            if (!constraints->isValuePossible(all_vars_domain, indexByLevel, d[cpt])) {
                toRemove.push_back(d[cpt]);
            }
        }

        for(int value : toRemove)
            d.remove(value);

        if (d.isEmpty())  // 1 ou plusieurs domaines vides
            return false;
    }

    return true;
}

void Node::addChild(Node * new_node) {
    _children.push_back(new_node);
}

bool Node::edgeConsistency(std::vector<Variable> vars, Constraints* constraints, unsigned int level, int valuePossibleNextLevel) {

    if (level >= vars.size()) // On est tout en bas, on a testé toutes les variables sans détecter de problèmes
        return true;

    Domain& d = vars[Engine::getIndexByLevel(vars, level)].getDomain();
    d.removeLessThan(valuePossibleNextLevel);  // On passe le domaine
    d.removeGreaterThan(valuePossibleNextLevel); // en singleton

    if (!reduceDomains(vars, constraints))
        return false;

    else if (level + 1 < vars.size()) {

        Domain& dNextLevel = vars[Engine::getIndexByLevel(vars, level+1)].getDomain(); // récupération du domaine de la valeur suivante
        bool success = false;
       for (unsigned int cpt = 0; cpt < dNextLevel.getSize(); cpt++) {
            if (edgeConsistency(vars, constraints, level+1, dNextLevel[cpt]))
                success = true;
        }
        if (!success)
            return false;
    }

    return true;
}


void Node::sortChosenValuesByNaturalOrder(const std::vector<Variable>& vars, std::vector<int>& chosenValues) {

    int *temp = new int[chosenValues.size()];
    unsigned int size = chosenValues.size();

    for (unsigned int i = 0; i < vars.size(); i++)
        temp[i] = chosenValues[vars[i].getLevel()];

    chosenValues.clear();

    for (unsigned int i = 0; i < size; i++) {
        chosenValues.push_back(temp[i]);
    }
}
